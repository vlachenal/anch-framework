# Analyse — anch-uuid

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : `src/uuid.cpp`, `inc/uuid.hpp`, `inc/impl/uuid.hpp` (complété le 2026-10-30), fournisseurs v3 / v5 d'`anch-crypto` (`md5.cpp`, `sha1.cpp`, le 2026-10-31).
Fournisseurs v1 / v6 d'`anch-device` : voir `anch-device/ANALYSE.md` (DV7 à DV12).

## Constats

### UU1. `parse` ne valide pas l'entrée — vérifié — **corrigé le 2026-10-31**
`UUID::parse` découpe par positions fixes (`substr`) sans vérifier la longueur (36), les tirets ni les caractères hexadécimaux. Une chaîne courte lève `std::out_of_range` et une chaîne mal formée `std::invalid_argument` (via `stoul`), sans message métier. Certaines entrées invalides sont même acceptées (par exemple `"1234zzzz-..."`, `stoul` s'arrêtant au premier caractère non hexadécimal).
Reproduction (2026-10-30) : `UUID("1234zzzz-e89b-42d3-a456-426614174000")` est accepté et vaut `00001234-e89b-42d3-a456-426614174000`.
Piste : vérifier la longueur, la position des tirets et les caractères hexadécimaux avant conversion (ou `std::from_chars` en contrôlant `ptr`).
Reproduction complémentaire (2026-10-31) : une chaîne de 35 caractères (`deb94ebc-be28-4899-981a-29199b7a487`) était aussi acceptée.
Correction :
- `parse` vérifie d'abord la longueur (36), les `-` aux positions 8, 13, 18 et 23 et les chiffres hexadécimaux (majuscules ou minuscules) ailleurs, puis convertit chaque partie avec `std::from_chars` (plus de `substr` ni de `stoul`). Une espace ou un signe en tête, acceptés par `stoul`, sont refusés.
- Toute entrée invalide lève `std::invalid_argument` avec un message (UUID et position). Avant, une entrée trop courte pouvait lever `std::out_of_range` : un code qui attrape `std::out_of_range` doit attraper `std::invalid_argument` (ou `std::logic_error`, parent des deux).
- La valeur n'est pas modifiée quand l'entrée est invalide (avant : modifiée partie par partie jusqu'à l'erreur).
- Tests : `uuid-parse` (majuscules, version, node) et `uuid-parse-ko` (9 entrées invalides, dont `1234zzzz-…`, valeur inchangée après échec). Avec l'ancienne bibliothèque, `uuid-parse-ko` échoue (chaîne trop courte acceptée).
- Résultat : 100 000 `parse` de ~16,8 ms à ~5,6 ms (3 fois plus rapide).
- Côté `anch-json`, un UUID invalide dans un document lève `MappingError` (`INVALID_FORMAT`) depuis le 2026-10-31 (J12 dans `anch-json/ANALYSE.md`).

### UU2. Registre des fournisseurs non protégé — vérifié — **corrigé le 2026-10-31**
`registerProvider` écrit dans `_providers` sans `_mutex`, alors que `generateUUID` lit la map. Un enregistrement concurrent d'une génération est une course.
Reproduction (2026-10-31, ThreadSanitizer) : un thread appelle `registerProvider(MD5_HASH, …)` pendant qu'un autre appelle `generateUUID(RANDOM, "")` : « data race » entre `_providers.find` (`generateUUID`) et l'insertion (`registerProvider`).
Piste : `std::shared_mutex` (écriture exclusive dans `registerProvider` et `registerRandomUUID`, lecture partagée dans `generateUUID`), et copier le fournisseur trouvé avant de l'appeler (hors verrou). `generateUUID` fait aussi deux recherches (`find` puis `operator[]`) : une seule suffit.
Correction :
- `UUID::_mutex` devient un `std::shared_mutex` : verrou exclusif dans `registerProvider` et `registerRandomUUID`, verrou partagé dans `generateUUID`.
- `generateUUID` fait une seule recherche, copie le fournisseur sous verrou et l'appelle hors verrou (un fournisseur peut lui-même générer un UUID ou enregistrer un fournisseur sans interblocage).
- La variable inutilisée `std::random_device rand;` de `registerRandomUUID` est supprimée.
- Vérifié : le programme de reproduction ne déclenche plus d'alerte ThreadSanitizer. Test `uuid-provider-concurrency` (enregistrements pendant des générations sur 3 threads) ; il n'a pas été lancé sous ThreadSanitizer (le test unitaire dépend d'`anch-ut` / `anch-cli`).
- Reste : `_providers` et `_mutex` sont des objets statiques globaux ; un enregistrement depuis l'initialisation statique d'une autre unité de compilation dépend de l'ordre d'initialisation.

### UU3. `std::random_device` utilisé comme moteur — vérifié (coût) ; partage entre threads non reproduit — **corrigé le 2026-10-31**
`getRandomEngine()` renvoie un `std::random_device`. La variable `std::random_device rand;` dans `registerRandomUUID` n'est pas utilisée. Les distributions `static` sont partagées entre threads.
Mesures (2026-10-31) :
- `std::random_device` utilise ici l'instruction matérielle (`rdrand` / `rdseed`, aucun appel système) : ~435 ns par appel, contre < 2 ns pour `std::mt19937_64`. `UUID::random()` fait 6 appels : ~2,6 µs par UUID (100 000 en ~263 ms).
- ThreadSanitizer ne signale rien sur 4 threads appelant `UUID::random()` en parallèle : avec libstdc++, ni `random_device` ni les distributions entières n'écrivent d'état partagé. Ce n'est pas garanti par la norme (appel d'une fonction non `const` sur un objet partagé).
Piste : garder une source cryptographique (la RFC 9562 recommande un générateur sûr pour la v4 : `mt19937_64` rendrait les UUID prévisibles), mais tirer 16 octets d'un coup par UUID (`getrandom(buf, 16, 0)` ou `arc4random_buf`) au lieu de 6 tirages, et supprimer les distributions partagées.
Mesure des sources (16 octets, 2026-10-31, glibc 2.44, noyau 7.2) : 4 appels à `std::random_device` ~1 740 ns ; `getentropy` ~460 ns (appel système) ; `arc4random_buf` ~40 ns (vDSO `getrandom`).
Correction :
- `random()` et `randomTime()` tirent 16 octets en un appel (`fillRandomBytes`), puis forcent la version et le variant (`random()`) ou remplacent les 48 premiers bits par l'horodatage (`randomTime()`), et construisent l'UUID avec `fromBytes`. Plus de distributions `static` partagées dans ces deux fonctions.
- Source : `arc4random_buf` (aléa cryptographique, sûr entre threads et après `fork`) quand CMake la trouve (`check_cxx_symbol_exists`, définition privée `ANCH_UUID_ARC4RANDOM` d'`anch-uuid` : glibc ≥ 2.36, BSD, macOS), sinon `getentropy` (POSIX ; `std::system_error` en cas d'échec).
- Résultat : 100 000 `random()` de ~261 ms à ~4,7 ms (**56 fois plus rapide**, ~47 ns par UUID) ; ~49 ms avec `getentropy` (5 fois plus rapide).
- Vérifié : 200 000 UUID v4 distincts, version `4`, variants `8` à `b` uniformes (avec les deux sources) ; octet de séquence uniforme (UU6) ; v7 : version `7`, horodatage exact ; ThreadSanitizer sans alerte sur 4 threads appelant `random()`. Test ajouté : `uuid-random-format` (bits de version et de variant pour v4 et v7, horodatage v7 encadré par l'heure système).
- `getRandomEngine()` et `getDistSeq()` (publiques) ne sont plus utilisées dans le dépôt depuis DV10 (`anch-device`) : supprimées le 2026-10-31 (ainsi que l'inclusion de `<random>` dans `uuid.hpp`, qui ne servait qu'à elles). Pour obtenir des bits aléatoires : `UUID::random().toBytes()`.

### UU4. `toString` passe par un `std::ostringstream` — **corrigé le 2026-10-31**
`toString` formatait chaque partie avec `std::setw` / `std::setfill` dans un `std::ostringstream` : création d'un flux (locale) et allocations à chaque appel. `anch-json` l'appelait pour chaque UUID écrit.
Correction :
- Nouvelle méthode `char* toChars(char* first) const` : écrit les `UUID::STRING_SIZE` (36) caractères sans allocation, en hexadécimal minuscule à largeur fixe. `toString` l'utilise (une seule allocation, celle de la chaîne).
- Différence possible : une partie trop grande pour sa largeur (possible seulement avec le constructeur par parties, par exemple `highTime > 0xFFF`) était écrite en entier par l'ancienne version (chaîne de plus de 36 caractères) ; ses bits de poids fort sont maintenant ignorés.
- Test `uuid-to-string` : `toString` et `toChars` comparés à l'ancienne mise en forme (`ostringstream`) sur 5 UUID fixes et 20 000 UUID aléatoires (versions 4 et horodatée), aller-retour `parse` / `toString`, et rien n'est écrit après les 36 caractères.
- Résultat : 100 000 `toString` de ~23 ms à ~1,7 ms (14 fois plus rapide). Côté `anch-json` (`uuidMapper.cpp` utilise `toChars` dans un tableau local) : écriture de 100 000 UUID de ~26,5 ms à ~3,7 ms (7 fois plus rapide).

### UU5. UUID v3 et v5 non conformes à la RFC 9562 (fournisseurs d'`anch-crypto`) — vérifié — **corrigé le 2026-10-31**
`generateMD5UUID` (`anch-crypto/src/crypto/hash/md5.cpp`) et `generateSHA1UUID` (`sha1.cpp`) :
- pas d'espace de noms : seul `data` est haché, alors que la RFC hache l'UUID de l'espace de noms (16 octets) suivi du nom ;
- octets dans le mauvais ordre : chaque partie est assemblée en petit-boutiste (`digest[0]` en poids faible) au lieu de gros-boutiste ;
- version : `highTime = (digest[6] | digest[7] << 8) & 0xFFF` au lieu de `((digest[6] & 0x0F) << 8) | digest[7]` ;
- variant : `digest[8] | 0x80` sans effacer le bit 6 : le variant vaut `c` à `f` (variant Microsoft réservé) une fois sur deux.
Reproduction (2026-10-31), espace de noms DNS et nom `www.example.com` passés concaténés dans `data` (références : Python `uuid.uuid3` / `uuid.uuid5`, identiques aux exemples de la RFC) :
- v3 : `8118f45d-ed3a-3505-c8a7-9ef04c814a2f` au lieu de `5df41881-3aed-3515-88a7-2f4a814cf09e` (on retrouve les octets attendus, inversés dans chaque partie) ;
- v5 : `7d65d62e-27e9-5b46-d5e1-a2a6aea86526` au lieu de `2ed6657d-e927-568b-95e1-2665a8aea6a2` ;
- variant sur 10 000 noms : `c` à `f` dans ~50 % des cas (v3 et v5).
Piste : construire l'UUID directement à partir des 16 premiers octets du condensat (gros-boutiste), forcer la version (`(b[6] & 0x0F) | 0x30` ou `0x50`) et le variant (`(b[8] & 0x3F) | 0x80`), et ajouter un paramètre espace de noms (par exemple `generateUUID(version, namespaceUUID, name)` et des constantes `NAMESPACE_DNS`, `NAMESPACE_URL`, …). Tests à partir des vecteurs de la RFC. Changement de valeurs pour les UUID déjà générés.
Les tests `anch-crypto` (`MD5`, `SHA1` dans `test/testUuid.cpp`) affichent l'UUID sans vérifier sa valeur.
Correction :
- `anch-uuid` : `UUID::toBytes()` et `UUID::fromBytes()` (représentation binaire de 16 octets, gros-boutiste, version lue dans l'octet 6), espaces de noms `UUID::namespaceDNS()`, `namespaceURL()`, `namespaceOID()`, `namespaceX500()` (statiques locales : pas de problème d'ordre d'initialisation), et surcharge `generateUUID(version, nameSpace, name)` qui passe au fournisseur les 16 octets de l'espace de noms suivis du nom.
- `anch-crypto` : `generateMD5UUID` et `generateSHA1UUID` prennent les 16 premiers octets du condensat, forcent la version (`(b[6] & 0x0F) | 0x30` ou `0x50`) et le variant (`(b[8] & 0x3F) | 0x80`), puis appellent `UUID::fromBytes`.
- `generateUUID(version, data)` reste disponible : `data` est haché tel quel (un nom seul donne un UUID de forme correcte, mais hors RFC faute d'espace de noms).
- **Les valeurs changent** pour tous les UUID v3 / v5 générés avant ce correctif (ils n'étaient pas conformes).
- Tests : `UUID-MD5-RFC` et `UUID-SHA1-RFC` (`anch-crypto`) : exemples de la RFC 9562 (A.2, A.4) et 4 autres valeurs de référence calculées avec Python (espaces URL, OID, X.500, nom UTF-8), plus version et variant sur 10 000 noms ; `uuid-bytes` (`anch-uuid`) : disposition de `toBytes`, aller-retour sur 10 000 UUID, valeurs des espaces de noms.
- Vérifié aussi : le programme de reproduction, relancé avec les bibliothèques corrigées, donne `5df41881-3aed-3515-88a7-2f4a814cf09e` et `2ed6657d-e927-568b-95e1-2665a8aea6a2`, variant toujours `8` à `b`.

### UU6. Séquence d'horloge biaisée — vérifié — **corrigé le 2026-10-31**
`getDistSeq()` tire uniformément entre `0` et `SEQ_HIGH_MASK` (`0x3F00`) au lieu de `0x3FFF` (14 bits). Avec `0x3F` dans l'octet haut, l'octet bas ne peut valoir que `00`.
Reproduction (2026-10-31) : sur 2 000 000 de `UUID::random()`, le premier octet du 4ᵉ groupe vaut `bf` 129 fois au lieu de ~31 250 (les 63 autres valeurs `80` à `be` sont uniformes). Les valeurs `bf01` à `bfff` ne sont jamais générées.
Concerne `random()`, `randomTime()` et les fournisseurs v1 / v6 d'`anch-device` (même `getDistSeq()`). Effet faible sur l'unicité (≈ 0,02 bit d'entropie en moins), mais la sortie n'est pas uniforme.
Piste : `std::uniform_int_distribution<uint16_t>(0, 0x3FFF)` (ou tirer les 14 bits avec le reste des octets aléatoires, cf. UU3).
Correction : `getDistSeq()` tire entre `0` et `SEQ_HIGH_MASK | SEQ_LOW_MASK` (`0x3FFF`). Concerne aussi `randomTime()` et `anch-device`, qui utilisent la même distribution.
- Vérifié : le programme de reproduction donne maintenant `bf` 31 148 fois sur 2 000 000 (~31 250 attendus).
- Test `uuid-clock-seq` : sur 200 000 UUID, l'octet 8 est toujours entre `0x80` et `0xbf`, et chacune des 64 valeurs apparaît entre la moitié et 1,5 fois la fréquence attendue.

### UU7. `randomTime` (v7) : inversion d'octets sur les machines gros-boutistes, horloge non spécifiée — lecture — **corrigé le 2026-10-31**
- `randomTime()` applique `std::byteswap` au timestamp quand `std::endian::native == std::endian::big`. Le timestamp est ensuite découpé par décalages (`>> 16`, `& 0xFFFF`), indépendants de l'ordre des octets en mémoire : l'inversion produit un horodatage faux sur ces machines. Non reproductible ici (x86, petit-boutiste).
- L'horodatage vient de `std::chrono::high_resolution_clock`, dont l'origine n'est pas garantie par la norme (avec libstdc++, c'est `system_clock`). La v7 exige l'heure Unix : `std::chrono::system_clock` est la bonne horloge. Même remarque pour `getUtcTimestamp()` (v1 / v6).
Vérifié (2026-10-31) sur x86 : version `7`, horodatage des 48 premiers bits égal à l'heure Unix en millisecondes.
Piste : supprimer le `byteswap`, utiliser `system_clock`.
Correction :
- `randomTime()` : `std::byteswap` supprimé (les parties sont extraites par décalages, indépendants de l'ordre des octets) ; horloge `std::chrono::system_clock`, dont l'origine est l'époque Unix depuis C++20.
- `getUtcTimestamp()` (v1 / v6, `anch-device`) : `system_clock` aussi ; documentation précisée (intervalles de 100 ns depuis le 1582-10-15).
- Tests : `uuid-random-format` (horodatage v7 encadré par l'heure système, depuis UU3) et `uuid-utc-timestamp` (conversion inverse du timestamp v1 encadrée par l'heure système).
- Non testable ici : le comportement sur une machine gros-boutiste (x86 uniquement) ; le code ne dépend plus de l'ordre des octets.

### UU8. `randomTime` (v7) non monotone dans une même milliseconde — vérifié — **corrigé le 2026-10-31**
Les 12 bits `rand_a` sont aléatoires : deux UUID v7 générés dans la même milliseconde sont dans le désordre une fois sur deux (5 039 paires successives sur 10 000 lors du test du 2026-10-31). La RFC 9562 (§ 6.2) rend la monotonie optionnelle, mais le tri chronologique est l'intérêt principal de la v7 (clés de base de données).
Piste : méthode 1 ou 3 de la RFC (compteur dans `rand_a`, ou fraction de milliseconde), avec un état protégé entre appels.
Correction (méthode 1 de la RFC 9562, § 6.2, compteur de longueur fixe) :
- Compteur de 16 bits : les 12 bits `rand_a` et les 4 premiers bits de `rand_b` (après le variant) ; les 58 derniers bits restent aléatoires.
- À chaque nouvelle milliseconde, le compteur repart d'une valeur aléatoire sur 15 bits (bit de poids fort à 0 : au moins 32 768 incréments). Dans la même milliseconde, ou si l'horloge recule, il est incrémenté ; son débordement incrémente l'horodatage (autorisé par la RFC).
- État `(timestamp << 16) | compteur` dans un `std::atomic<uint64_t>` statique de `uuid.cpp`, mis à jour par compare-and-swap : ordre strict entre threads, sans verrou, sans changement d'en-tête.
- Un compteur de 12 bits seulement (2 048 incréments garantis) ne suffisait pas : environ 15 000 UUID par milliseconde sur un thread, l'horodatage aurait pris de l'avance pendant une rafale.
- Vérifié : 0 paire dans le désordre sur 10 000 (avant : 5 039) ; 1 000 000 de `randomTime()` en 67 ms sur 1 thread sans avance de l'horodatage, 4 × 1 000 000 en 87 ms sur 4 threads avec au plus 1 ms d'avance (débit supérieur à 32 768 par milliseconde) ; ThreadSanitizer sans alerte.
- Test `uuid-v7-monotonic` : 200 000 UUID strictement croissants (octets et chaîne) sur un thread ; 4 threads × 50 000 : croissants dans chaque thread, couple horodatage / compteur unique sur l'ensemble.
- L'ordre n'est garanti que dans un même processus (état en mémoire) : deux processus, ou un processus et son fils après `fork`, ont des compteurs indépendants (l'unicité reste assurée par les 58 bits aléatoires).

### UU9. Type valeur alourdi et incomplet — vérifié (taille) / lecture — **corrigé le 2026-10-31**
- `sizeof(UUID)` vaut 40 octets pour 16 octets d'information : destructeur `virtual` (pointeur de table virtuelle) et parties stockées dans des champs plus larges que nécessaire (`_clockSeqLow` sur 16 bits, `_version` en `int`). Rien ne dérive d'`UUID`.
- Constructeurs de copie et de déplacement, et `operator=`, écrits à la main alors que ceux générés par défaut suffisent ; pas d'`operator=` de déplacement.
- Pas d'`operator<` (ou `<=>`) ni de spécialisation de `std::hash` : un `UUID` ne peut pas servir de clé de `std::map` / `std::unordered_map` sans comparateur maison.
Piste : destructeur non virtuel (`= default`), membres spéciaux par défaut, `operator<=>` par défaut, `std::hash<anch::UUID>`. Changement d'ABI.
Correction :
- Stockage : un seul membre `std::array<uint8_t,16> _bytes` (représentation binaire RFC 9562). `sizeof(UUID)` passe de 40 à 16 octets ; le type est trivialement copiable. Les accesseurs (`getLowTime`, …, `getVersion`) calculent chaque partie depuis les octets (inline) ; `toBytes` / `fromBytes` deviennent de simples copies.
- Destructeur non virtuel, constructeurs de copie / déplacement et affectations (copie et déplacement) par défaut. Constructeur par défaut : UUID nul.
- `operator<=>` et `operator==` par défaut : ordre binaire, identique à l'ordre des chaînes (`operator!=` est généré). Spécialisation `std::hash<anch::UUID>` (utilisable dans `std::unordered_map`).
- Constructeur par parties : chaque partie est tronquée à sa largeur (avant : stockée telle quelle ; seuls `toString` / `toChars` tronquaient depuis UU4).
- `parse` : vérification et conversion en une passe avec une table de 256 valeurs (une première version par comparaisons était 2,3 fois plus lente que l'ancienne, à cause des branches imprévisibles sur des chiffres aléatoires). 100 000 `parse` de ~5,4 ms à ~2,0 ms (2,7 fois plus rapide ; ~16,8 ms avant UU1). `toString`, `random()` et l'écriture JSON d'UUID sont inchangés (±5 %).
- Test `uuid-value-type` : taille 16, copie triviale, destructeur non virtuel, UUID nul par défaut, ordre égal à l'ordre des chaînes et des octets sur 10 000 UUID, `std::set` et `std::unordered_set`, constructeur par parties et accesseurs.
- **Changement d'ABI** : tout code utilisant `UUID` (`anch-json`, `anch-crypto`, `anch-device`, applications) doit être recompilé.

## Points positifs
- UUID aléatoires conformes (vérifié le 2026-10-30 sur 10 000 tirages) : version `4` à chaque fois, variant RFC (`8`, `9`, `a`, `b`) uniformément réparti, longueur 36. Seul écart : le biais de la séquence d'horloge (UU6, corrigé).
- UUID v7 : disposition conforme (horodatage Unix en millisecondes sur 48 bits, version `7`, variant RFC) sur x86 ; strictement croissants dans un processus depuis UU8.
- Aller-retour `parse` / `toString` correct sur un UUID valide.
- 14 tests passent (dont `uuid-to-string`, `uuid-parse`, `uuid-parse-ko`, `uuid-bytes`, `uuid-clock-seq`, `uuid-random-format`, `uuid-utc-timestamp`, `uuid-v7-monotonic`, `uuid-value-type` et `uuid-provider-concurrency`, ajoutés le 2026-10-31).

## À approfondir
- [x] Tests de `parse` sur des entrées invalides (`uuid-parse-ko`).
- [x] Tester les UUID de version 3 et 5 (fournisseurs enregistrés par `anch-crypto`) par rapport aux vecteurs de la RFC 9562 : non conformes (UU5), corrigés et testés.
- [x] Fournisseurs v1 / v6 d'`anch-device` : analysés le 2026-10-31 (DV7 à DV12 dans `anch-device/ANALYSE.md`) ; la v6 est fausse (bits inversés, version `1`, doublons).
