# Analyse — anch-crypto

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

Hors périmètre : la librairie est un projet « pour le fun », destiné à devenir un wrapper sur OpenSSL. Les algorithmes n'ont pas été analysés.

## Constats

### C1. Dépendance au `ThreadPool` défectueux — vérifié — **résolu le 2026-10-29** (via U1)
Tous les headers de modes de chiffrement (`cbc`, `cfb`, `ctr`, `ecb`, `ofb`, `pcbc`, `aes128/192/256`, via `bcModOp.hpp`) incluent `threadPool.hpp`. Ils héritent du constat U1 de [../anch-utils/ANALYSE.md](../anch-utils/ANALYSE.md) : définitions multiples si deux `.cpp` les incluent.
Le 2026-11-02 : include et `friend class anch::ThreadPool` supprimés de `bcModOp.hpp` (les modes de chiffrement créent leurs propres `std::thread`) ; ajout des includes standards dont il profitait par transitivité (`<deque>`, `<vector>`...).

### C2. Lecture hors limites lors de l'expansion de clé AES-192 (AES-NI) — vérifié — **corrigé le 2026-10-29**
`inc/crypto/cipher/aes.hpp` : la clé AES-192 fait 24 octets, mais `aesni192ExpandKey` charge 16 octets à partir de l'octet 16 (`_mm_loadu_si128(key + 16)`) : 8 octets lus au-delà du tampon `aesKey[4*K]`. GCC le détecte en `-O3` (`-Warray-bounds`), ce qui bloquait le build Release.
Correction : le tampon local est arrondi au multiple de 16 octets supérieur et initialisé à zéro (`uint8_t aesKey[(4*K + 15) / 16 * 16] = {}`), dans les deux branches (`ANCH_CPU_DETECTION` et `ANCH_CPU_AES`).

### C3. SHA-2 : inversion des octets de mots non initialisés — vérifié (`-Wuninitialized`) — **corrigé le 2026-11-02**
`inc/crypto/hash/impl/sha2.hpp`, `transform` : `B` est la taille du bloc en octets (64 ou 128), mais `bytesSwap(chunk->words, B)` inversait `B` mots alors que le bloc n'en contient que 16 (64/4 et 128/8). Les mots suivants n'étaient pas initialisés (comportement indéfini), puis réécrits par le calcul du message schedule : le hash était juste. Détecté par GCC quand `anch::byteSwap` est passé à `std::byteswap` + `memcpy`.
Correction : inverser `B / sizeof(W)` mots. Reste : la boucle du message schedule va jusqu'à `B` (128 pour SHA-384/512) au lieu de `R` (80) : calculs inutiles.

### C4. SHA-224 / SHA-384 : écriture au-delà du condensat — vérifié (ASan) — **corrigé le 2026-11-12**
`inc/crypto/hash/impl/sha2.hpp`, `finalize` : les 8 mots d'état étaient toujours copiés dans le condensat (32 octets en SHA-224, 64 octets en SHA-384), qui ne fait que 28 ou 48 octets. En SHA-384, 16 octets étaient écrits au-delà de l'objet (`stack-buffer-overflow`). En SHA-224, les 4 octets en trop tombaient par chance dans le bourrage d'alignement du contexte. La branche gros-boutiste copiait `sizeof(W) * O` octets et lisait 16 octets dans un entier de 8 pour la taille du message en SHA-384/512.
Correction : seuls les `O / sizeof(W)` premiers mots sont copiés (`static_assert` sur la taille), `memcpy` de `O` octets et taille du message sur 8 octets en gros-boutiste ; `if constexpr` sur le boutisme.

### C5. `Hash::digest(const std::string&)` : référence sur un pointeur nul en SHA-1 — vérifié (UBSan) — **corrigé le 2026-11-12**
`inc/crypto/hash/impl/hash.hpp` renvoyait `*_digest`, pointeur vers le condensat que `SHA1` ne renseignait jamais (les tests passaient par chance). Ce pointeur interne devenait aussi invalide après copie d'un objet de hachage (il pointait vers le condensat de l'original).
Correction : `_digest` est supprimé de `Hash` (et de `SHA2`, `SHA224_256`, `MD5`) ; les méthodes `digest(...)` renvoient le résultat de la méthode virtuelle `digest()`.

### C6. Tests : lecture hors limites et fuites — vérifié (ASan) — **corrigé le 2026-11-12**
`test/testSHA1.cpp` (`sha1-sec-password`) copiait `getBlockSize()` (64) octets du condensat SHA-1 au lieu de `getOutputSize()` (20). `test/benchAnchOpenssl.cpp` ne libérait pas ses tampons `new uint8_t[]` (remplacés par `std::vector`).

### C11. Méthodes `digest(données)` de `Hash` masquées par les classes de hachage — vérifié (compilation) — **corrigé le 2026-11-19**
`MD5`, `SHA1` et `SHA2` redéfinissent `digest()` : la redéfinition masque les surcharges héritées `digest(const std::string&)`, `digest(const uint8_t*, std::size_t)`, `digest(std::istream&)` et `digest(const std::array&)`, inaccessibles par le type dérivé (seuls les constructeurs prenaient les données, un objet ne pouvait pas être réutilisé).
Correction : `using Hash<...>::digest;` dans les trois classes (SHA-224 à SHA-512 en héritent) ; test `hash-digest-overloads` (MD5, SHA-1, SHA-256, SHA-512, objet réutilisé), qui ne compile pas sans la correction.

### C12. MD5 : arithmétique de pointeur en mots au lieu d'octets dans `addData` — vérifié (Clang UBSan, 2026-11-19) — corrigé (2026-11-21)
`src/crypto/hash/md5.cpp:164` : `std::memcpy(_context.input + 64 - word, data, len)` calcule sur un `uint32_t*`, donc le décalage est compté en mots de 4 octets ; la branche `else` (ligne 168) convertit bien en `uint8_t*`.
- `_context.input + 64` pointe hors du tableau `uint32_t[16]` : comportement indéfini à chaque appel, même quand le résultat final est correct (`word == 64`). Clang UBSan : « index 64 out of bounds for type 'uint32_t[16]' » ; tests `hash-digest-overloads`, `UUID-MD5` et `UUID-MD5-RFC` en échec sous Clang + ASan / UBSan, en `-O2` comme en `-O3`.
- Quand le tampon est partiellement rempli (`word < 64`), les données sont copiées `(64 - word) * 4` octets plus loin : hors de `input`, condensat faux et écriture hors limites. Pas atteint par l'API actuelle : chaque `digest()` fait un seul `addData` après `reset()`, et `digest(std::istream&)` lit des blocs de 1024 octets (multiple de 64).
- Non vu par la CI : sanitizers en `Debug` avec GCC.
Correction : décalages calculés sur `reinterpret_cast<uint8_t*>(_context.input)` dans les trois copies ; test `md5-chunks` (sous-classe qui appelle `addData` par morceaux de 1 à 1000 octets et compare au condensat en une fois), en échec sans la correction.

### C13. CTR et AES logiciel : violations d'aliasing strict — lecture (2026-11-19) — corrigé (2026-11-21)
- `inc/crypto/cipher/ctr.hpp:145` et `:165` : `reinterpret_cast<uint16_t*>(&ctxtVect.data()[14])` écrit un `uint16_t` dans un `std::array<uint8_t,16>` (aliasing, et alignement non garanti) ; le résultat dépend aussi de l'ordre des octets de la machine.
- `inc/crypto/cipher/aes.hpp:944` (`addRoundKey`, voie logicielle : `ANCH_CPU_DETECTION` ou sans `ANCH_CPU_AES`) : `reinterpret_cast<uint32_t*>(_state)` sur `uint8_t _state[4][4]`, aligné sur 1 octet.
Comportements indéfinis exploitables dès `-O2` (aliasing strict), non signalés par les tests ni par UBSan aujourd'hui. Piste : `std::memcpy` (ou décomposition octet par octet du compteur, qui fixe aussi l'ordre des octets).
Correction : le compteur CTR est calculé octet par octet (voir C14) ; `addRoundKey` passe par `std::memcpy` vers un `uint32_t[4]` local (même résultat).

### C14. CTR : compteur sur 16 bits — lecture (2026-11-21) — corrigé (2026-11-21)
L'ancien code ne gardait que les 16 bits de poids faible de l'indice de bloc, écrits dans les 2 derniers octets du nonce dans l'ordre des octets du processeur : au-delà de 65 536 blocs (1 Mio avec AES), le flux de clé se répétait, ce qui révèle le XOR de deux portions du clair. L'indice des modes d'opération était aussi un `uint32_t` (répétition après 2^32 blocs, 64 Gio). Le format différait du CTR usuel (NIST SP 800-38A).
Correction (changement incompatible, version 0.3) : `CTR::setCounter` ajoute l'indice au nonce, lu comme un entier gros-boutiste de la taille du bloc (modulo 2^128), comme la fonction d'incrémentation standard de NIST SP 800-38A et OpenSSL ; l'indice de bloc est un `uint64_t` dans `BlockCipherModeOfOperation` et tous les modes. Tests `nist-128` et `nist-256` (vecteurs F.5.1 et F.5.5 de SP 800-38A, compteur initial `f0…ff` qui propage la retenue sur plusieurs octets, séquentiel et 3 threads, bloc final partiel), `counter-wrap` (`ff…ff` + 1 donne `00…00`) et `no-key-stream-reuse` (blocs 0 et 65 536 différents) ; les 4 échouent avec l'ancien compteur.

## Points positifs
- 117 tests passent, plus un benchmark comparatif avec OpenSSL (`benchAnchOpenssl.cpp`), utile pour la future migration.

### C7. Base64 : données binaires tronquées au premier octet nul — vérifié (2026-11-14) — **corrigé le 2026-11-16**
`Base64::decode(const char*, uint64_t, std::ostream&, uint8_t)` (`src/crypto/b2t/base64.cpp`) écrit chaque groupe décodé avec `output << buffer`, c'est-à-dire comme une chaîne C : l'écriture s'arrête au premier octet nul du groupe.
Reproduction : les 6 octets `00 01 02 41 00 42` sont encodés en `AAECQQBC` ; leur décodage rend 1 octet (`41`). Seul le texte sans octet nul fait l'aller-retour.
Correction : le décodage d'un groupe renvoie le nombre d'octets décodés, écrits avec `output.write`.

### C8. Base64 : lecture hors limites sans remplissage — vérifié (ASan, 2026-11-14) — **corrigé le 2026-11-16**
La même fonction avance de 4 caractères par groupe avec `length -= 4` sur un `uint64_t` : quand la longueur n'est pas un multiple de 4, elle passe sous zéro et la boucle continue au-delà des données, jusqu'à un `=` ou un caractère invalide (`std::bad_cast`).
Reproduction : `Base64::encode("AB", URL | NOPADDING)` donne `QUI` ; `Base64::decode` de ces 3 caractères (tampon de 3 octets) : `heap-buffer-overflow` en lecture. La bibliothèque ne relit donc pas sa propre sortie sans remplissage.
Un groupe final d'un seul caractère (invalide) lisait aussi `bytes[1]` non initialisé : le tableau est maintenant initialisé à zéro (avertissement `-Wmaybe-uninitialized` devenu bloquant en release après X1) ; le reste n'est pas corrigé.
Correction : chaque groupe est borné à `min(4, length)` ; un groupe d'un seul caractère ou composé uniquement de `=` lève `std::bad_cast`.

### C9. Base64 : encodage sans remplissage, caractères du groupe précédent écrits — vérifié (2026-11-16) — **corrigé le 2026-11-16**
Trouvé par le test d'aller-retour de C8. Avec `NOPADDING`, le dernier groupe incomplet ne renseigne que 2 ou 3 caractères du tampon mais le tampon entier était écrit (`output << buffer`) : les caractères restants venaient du groupe précédent. Exemple : 4 octets `F0 F1 F2 F3` donnaient `8PHy8wHy` au lieu de `8PHy8w`. Seules les entrées de moins de 3 octets (tampon encore vide) étaient correctes.
Correction : l'encodage d'un groupe renvoie le nombre de caractères écrits, écrits avec `output.write`.

### C10. Base64 : encodage d'un flux, remplissage au milieu de la sortie — vérifié (2026-11-16) — **corrigé le 2026-11-16**
`Base64::encode(std::istream&, ...)` lisait des blocs de 1024 octets et encodait chacun séparément : 1024 n'étant pas un multiple de 3, chaque bloc se terminait par un groupe d'un octet complété par `==`. Un flux de 5 000 octets donnait 6 680 caractères au lieu de 6 668, illisibles par un autre décodeur (et sans remplissage, des données fausses).
Correction : blocs de 1023 octets (multiple de 3). Le décodage d'un flux lit des blocs de 1024 caractères (multiple de 4) et n'est pas concerné, tant que les données ne contiennent ni espace ni saut de ligne (non pris en charge).

Tests ajoutés : `base64-dec-binary` (octets nuls, 256 valeurs), `base64-nopadding` (longueurs 0 à 31, alphabets standard et URL, tampon de taille exacte pour ASan), `base64-dec-invalid`, `base64-stream`. 123/123 tests anch-crypto en release et sous ASan / UBSan.
Reste : `base64-enc-makefile` lit `Makefile`, absent avec d'autres générateurs que Make (n'encode alors rien, sans échec).

## À approfondir
- [ ] Définir l'API du wrapper OpenSSL (EVP) en gardant les interfaces actuelles (`Hash`, `BlockCipher`, `HMAC`).
