# Analyse — anch-device

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre : symboles définis dans les headers publics, `network.cpp`, `networkInterface.cpp` (complété le 2026-10-30), `uuidProvider.cpp` et `test/testUuid.cpp` (le 2026-10-31, modifications précédemment en cours commitées).

## Constats

### DV1. `anch_cpuid` défini sans `inline` dans un header — vérifié — **corrigé le 2026-10-29**
`inc/device/cpu.hpp:33` : `void anch_cpuid(int info[4], int InfoType){...}` est une fonction libre définie dans le header. `nm` montre un symbole fort (`T`) dans tout fichier qui inclut `cpu.hpp` : inclure ce header dans deux `.cpp` fait échouer l'édition de liens.
Piste : `inline` (ou `static inline`), ou déplacer la définition dans un `.cpp`.

### DV2. Une seule interface atypique empêche de lister toutes les interfaces — vérifié — **corrigé le 2026-11-10**
`src/device/networkInterface.cpp` : le constructeur de `NetworkInterface` enchaîne six `ioctl` (drapeaux, adresse, MTU, adresse matérielle, broadcast, masque) et lève `DeviceException` au premier échec. `Network::load()` ne rattrape pas l'exception : une seule interface qui ne supporte pas l'un de ces appels (par exemple `SIOCGIFBRDADDR`, ligne 84, sur une interface point à point comme un tunnel VPN `tun0` / `wg0`) fait échouer `getInterface()` et `getInterfaces()` pour **toutes** les interfaces. Non reproduit ici (interfaces `lo`, `eno1`, `wlo1` uniquement).
Piste : rendre les informations facultatives (adresse de broadcast absente) ou ignorer l'interface fautive.
Vérification (2026-11-10, espace de noms réseau non privilégié `unshare -rn`) : l'hypothèse du tunnel est **fausse**, Linux ne contrôle pas `IFF_BROADCAST` pour `SIOCGIFBRDADDR` et un `tun0` point à point ne fait échouer aucun `ioctl`. La vraie cause est une interface supprimée entre `SIOCGIFCONF` et les `ioctl` suivants : **7 échecs sur 20 000** `reload()` pendant qu'une boucle crée et supprime une interface dummy (« Error while retrieving IP address for interface dumx », DV6 pour le reste du message).
Correction : `Network::load()` repose sur un seul appel à `getifaddrs` (instantané cohérent, DV4). Le MTU, la métrique et le broadcast restent lus par `ioctl`, mais sont facultatifs : une interface supprimée entre-temps garde ses adresses avec un MTU à `0`. Aucun échec sur 20 000 `reload()` dans les mêmes conditions. `NetworkInterface` devient un type valeur rempli par `Network` (constructeur depuis `ifreq` supprimé, `networkInterface.cpp` supprimé).

### DV3. `_metric` lu sans appel système — lecture — **corrigé le 2026-11-10**
`src/device/networkInterface.cpp:99` : `ifr_metric` fait partie de l'union de `struct ifreq`, qui contient à ce moment le résultat de `SIOCGIFNETMASK`. Sans `ioctl(SIOCGIFMETRIC)`, la valeur lue est un morceau du masque réseau.
Correction : `ioctl(SIOCGIFMETRIC)`. Linux renvoie toujours `0` (documenté, vérifié par le test `network-sys-information`).

### DV4. Énumération limitée et IPv4 uniquement — vérifié — **corrigé le 2026-11-10**
`src/device/network.cpp:110` : `SIOCGIFCONF` avec un tampon de 1024 octets (environ 25 interfaces au maximum, troncature silencieuse). `SIOCGIFCONF` ne renvoie que les interfaces ayant une adresse IPv4 ; les adresses sont décodées en `sockaddr_in`.
Piste : `getifaddrs()`, qui couvre IPv4, IPv6 et les interfaces sans adresse, sans limite de taille.
Vérifié : une interface dummy sans adresse IPv4 n'était pas listée.
Correction (`getifaddrs`) :
- toutes les interfaces sont listées, avec ou sans adresse ; les étiquettes d'adresse IPv4 (`eth0:1`) restent des entrées à part, comme avec `SIOCGIFCONF` ;
- nouvelles informations : `getIpv6Addresses()`, avec la portée pour les adresses de lien local (`fe80::…%eth0`), et `isUp()` ;
- adresse matérielle depuis `AF_PACKET`, vide quand l'interface n'en a pas (tunnel), au lieu de `00:00:00:00:00:00` ;
- `isLocalhost()` vient du drapeau `IFF_LOOPBACK` et non plus du nom `lo` ;
- `inet_ntoa` (non réentrant) est remplacé par `getnameinfo(NI_NUMERICHOST)` ;
- broadcast lu par `SIOCGIFBRDADDR` quand l'interface a `IFF_BROADCAST` : la glibc place l'adresse elle-même dans `ifa_broadaddr` quand aucun broadcast n'est configuré (vérifié sur une adresse sans `brd`). Vide si `0.0.0.0` ou sur une interface point à point.

### DV5. `reload()` invalide les références déjà distribuées — lecture — **corrigé le 2026-11-10**
`getInterface()` renvoie un pointeur et `getInterfaces()` une référence (ligne 82) vers la map statique, après avoir relâché `MUTEX`. `reload()` vide cette map (ligne 89) : les pointeurs et références obtenus auparavant deviennent invalides, et un parcours de `getInterfaces()` concurrent d'un `reload()` est une course.
Piste : renvoyer des copies, ou un `std::shared_ptr` vers une map immuable remplacée à chaque `reload()`.
Correction : instantané immuable (`std::shared_ptr<const map>`) remplacé à chaque `reload()`, chargé hors verrou. **API modifiée** : `getInterfaces()` renvoie une copie de la map et `getInterface()` un `std::optional<NetworkInterface>` (au lieu d'un pointeur `NULL`). En cas d'erreur, `reload()` garde l'instantané précédent. Plus de membres statiques dans le header, et `Network()` est supprimé (`= delete`). Test `network-concurrent-reload` (4 lecteurs, 500 `reload()`), propre sous TSan.

### DV6. Codes d'erreur sans `errno` — vérifié — **corrigé le 2026-11-10**
`DeviceException(..., ret)` reçoit la valeur de retour (`-1`) au lieu d'`errno`, et le constructeur la passe à `gai_strerror` : le message est toujours « Bad value for ai_flags » (vérifié).
Correction : `DeviceException` dérive de `std::system_error` (`errno`, `std::generic_category`), comme `IOException` dans `anch-network`. L'errno courant est utilisé par défaut. Le constructeur sans code est remplacé par ce paramètre par défaut. Test `device-exception`.

### DV7. UUID v6 faux : bits inversés, version 1, doublons — vérifié — **corrigé le 2026-10-31**
`generateUUIDv6` (`src/device/uuidProvider.cpp`) inverse les 64 bits du timestamp un par un (`std::bitset`, bit `i` vers bit `63 - i`), puis appelle la même fonction que la v1.
- La v6 (RFC 9562, § 5.6) ne s'obtient pas en inversant les bits : elle réordonne les champs du timestamp de 60 bits (32 bits de poids fort, puis 16, puis la version et les 12 bits de poids faible) pour que l'ordre des octets suive l'ordre chronologique.
- `generateUUID` fixe toujours `Version::MAC_ADDRESS` : l'UUID « v6 » porte la version `1`.
- Après inversion, les 4 bits de poids faible du timestamp (les plus changeants) tombent dans les bits masqués par `TIME_HIGH_MASK` : la résolution passe de 100 ns à 1,6 µs, et deux UUID générés dans le même intervalle ne diffèrent que par la séquence d'horloge aléatoire (14 bits).
Reproduction (2026-10-31) : version `1` au lieu de `6` ; horodatage décodé selon la disposition v6 incohérent (≈ 58 000 ans dans le futur) ; **7 doublons sur 200 000** UUID générés à la suite (≈ 1 µs par UUID) ; une paire successive sur deux n'est pas croissante (alors que la v6 doit l'être).
Piste : `time_high = ts >> 28`, `time_mid = (ts >> 12) & 0xFFFF`, `time_low = ts & 0xFFF` avec la version `6` ; garantir l'unicité dans un même intervalle de 100 ns (compteur ou séquence d'horloge incrémentée, comme pour la v7 d'`anch-uuid`, UU8).
Correction :
- `generateUUIDv6` : disposition RFC 9562 (§ 5.6) : 32 bits de poids fort du timestamp, 16 bits suivants, version `6` et 12 bits de poids faible. Plus d'inversion de bits.
- `generateUUIDv1` et `generateUUIDv6` construisent chacun leur UUID avec leur version ; la partie commune est découpée en `getNode` (adresse MAC, inchangée : DV8, DV9) et `randomClockSequence`.
- Unicité : `nextTimestamp()` renvoie l'heure, ou le dernier timestamp + 1 si l'horloge n'a pas avancé (plusieurs UUID dans le même intervalle de 100 ns) ou a reculé. État dans un `std::atomic<uint64_t>` mis à jour par compare-and-swap : timestamps strictement croissants dans le processus, entre threads et entre v1 et v6. Cela couvre aussi la v1 (DV11).
- Les valeurs des UUID « v6 » changent (elles étaient fausses).
- Tests (nouvel exécutable `anch-device-uuid-provider-test`, avec `anch-ut`, adresse MAC fixe) : `uuid-v6-format` et `uuid-v1-format` (version, variant, nœud, horodatage décodé encadré par l'heure système), `uuid-v6-order` (200 000 UUID v6 strictement croissants en ordre binaire et de chaîne, tous distincts), `uuid-v1-v6-unique-threads` (4 threads × 50 000 UUID v1 / v6 mélangés, tous distincts).
- Vérifié : avec l'ancien `uuidProvider.cpp`, les trois tests v6 échouent (version `1`, ordre non croissant, 9 doublons sur 200 000) et `uuid-v1-format` passe ; avec le correctif, le programme de reproduction donne version `6`, horodatage exact, 0 doublon et 0 paire non croissante sur 200 000.

### DV8. Choix du nœud (adresse MAC) fragile — vérifié / lecture — **corrigé le 2026-10-31**
`generateUUID` parcourt `Network::getInterfaces()` et garde la **dernière** interface non `localhost` (ordre alphabétique des noms).
- Seules les interfaces ayant une adresse IPv4 sont listées (DV4) : ici `eno1` (sans adresse) est absente et `wlo1` est choisie ; le nœud change selon l'état du réseau (vérifié).
- Une interface sans adresse matérielle (tunnel `tun0`, `wg0`) a l'adresse `00:00:00:00:00:00`, qui serait retenue si elle est la dernière (lecture).
- Sans interface autre que `lo`, la chaîne vide est convertie par `std::stoull` : `std::invalid_argument` (lecture). La RFC 9562 (§ 6.10) prévoit dans ce cas un nœud aléatoire avec le bit multicast à 1.
- Une exception de DV2 (une interface atypique) fait aussi échouer toute génération d'UUID v1 / v6.
Piste : choisir une interface physique de façon stable (première par nom, adresse non nulle) via `getifaddrs` (DV4), mettre le nœud en cache, et basculer sur un nœud aléatoire (bit multicast) à défaut.
Correction (`selectDefaultNode` / `getDefaultNode` dans `uuidProvider.cpp`) :
- Les adresses matérielles viennent de `getifaddrs` (`AF_PACKET`) : toutes les interfaces sont vues, avec ou sans adresse IP. Le fournisseur ne dépend plus de `Network` (ni de DV2, ni de DV4).
- Sont ignorées : interfaces de boucle locale, adresses de longueur différente de 6 octets, nulles, de diffusion ou multicast.
- Une adresse universelle (IEEE) est préférée à une adresse administrée localement (interfaces virtuelles, adresses Wi-Fi aléatoires) ; dans chaque catégorie, la première interface par nom est retenue.
- À défaut, nœud aléatoire (48 bits de `UUID::random()`) avec le bit multicast à 1 (RFC 9562, § 6.10), au lieu de `std::invalid_argument`.
- Le nœud est calculé une fois par processus (statique locale).
- `AF_PACKET` est propre à Linux, comme les `ioctl` déjà utilisés par `anch-device` (`SIOCGIFHWADDR`).
- **Le nœud par défaut change** : ici `eno1` (`d0ad08244ba4`, sans adresse IP) au lieu de `wlo1` (`200b74442dd9`).
- Test `uuid-default-node` : nœud égal à l'adresse attendue selon ces règles (recalculée avec `getifaddrs`), identique pour v1 et v6 et stable sur 100 appels.
- Vérifié aussi : avec un `getifaddrs` qui échoue (bibliothèque `LD_PRELOAD`), nœud aléatoire avec le bit multicast, différent à chaque processus et stable dans un processus.

### DV9. Adresse MAC passée en paramètre mal validée — vérifié — **corrigé le 2026-10-31**
`generateUUID(Version::MAC_ADDRESS, mac)` utilise `mac` comme nœud. Le découpage sur `:` puis `std::stoull` accepte des entrées invalides ou lève des exceptions brutes.
Reproduction (2026-10-31) : `12:zz:00:00:00:00` donne le nœud `000000000012` ; `01-23-45-67-89-ab` donne `000000000001` (arrêt au premier `-`) ; `xyz` lève `std::invalid_argument("stoull")` ; une adresse de 9 octets lève `std::out_of_range("stoull")`.
Piste : exiger 6 paires hexadécimales séparées par `:` ou `-`, et lever `std::invalid_argument` avec un message.
Correction : `getNode` exige exactement 17 caractères, 6 paires hexadécimales (majuscules ou minuscules) séparées par `:` ou `-` (le même séparateur partout), et lève `std::invalid_argument` (« Invalid MAC address '…': 6 hexadecimal pairs separated by ':' or '-' expected ») sinon. Plus de `substr` ni de `std::stoull`.
- `01-23-45-67-89-ab` est maintenant accepté et donne le nœud `0123456789ab` (avant : `000000000001`).
- Test `uuid-mac-address` : 4 adresses valides (séparateurs `:` et `-`, majuscules) et 12 invalides (caractère non hexadécimal, trop longue, trop courte, séparateurs mélangés ou autres, sans séparateur, espaces, signe, paire vide), refusées en v1 et en v6. Avec l'ancien `uuidProvider.cpp`, il échoue (`01-23-45-67-89-ab` donne le nœud `1`).

### DV10. Génération v1 coûteuse — vérifié (coût) / lecture (cause) — **corrigé le 2026-10-31**
~1,9 µs par UUID v1 (200 000 en ~370 ms). Cause probable (lecture) : chaque appel copie la map des interfaces (`for(auto iter : Network::getInterfaces())` copie chaque paire `std::string` / `NetworkInterface`), prend le verrou de `Network` et reconstruit l'adresse MAC par concaténations de `substr`.
Piste : calculer le nœud une fois (cache), parcourir par référence (`const auto&`).
Depuis DV8 (nœud en cache) : ~0,8 µs par UUID v1 / v6 (au lieu de ~1,9 µs). Reste surtout la séquence d'horloge tirée avec `std::random_device` (~435 ns par appel, voir UU3 dans `anch-uuid/ANALYSE.md`).
Correction : `randomClockSequence` prend ses 14 bits dans les octets 8 et 9 de `UUID::random()` (source cryptographique tirée en un appel, `arc4random_buf` ici, UU3) au lieu de `getDistSeq()` / `getRandomEngine()`.
- Résultat : ~99 ns par UUID v1 (génération seule, 640 000 UUID), contre ~1,9 µs avant DV8 : **19 fois plus rapide**.
- Vérifié : variant toujours correct ; les 16 384 valeurs de séquence apparaissent sur 640 000 UUID (entre 17 et 68 fois, ~39 attendues : dispersion d'un tirage uniforme).
- Test `uuid-clock-sequence` : variant et répartition uniforme des 6 bits de poids fort de la séquence sur 64 000 UUID v1 / v6.
- La séquence reste tirée à chaque UUID (DV11) : une séquence stable par processus serait conforme à la recommandation de la RFC, mais après `fork` le parent et le fils auraient le même nœud, la même séquence et le même dernier timestamp (doublons possibles dans un même intervalle de 100 ns) sans régénération dans le fils (`pthread_atfork`).
- `UUID::getRandomEngine()` et `UUID::getDistSeq()` (publiques, `anch-uuid`) ne sont plus utilisées dans le dépôt : supprimées.

### DV11. Séquence d'horloge v1 tirée à chaque appel, pas de garde sur un même intervalle — lecture — **garde corrigée le 2026-10-31 (avec DV7)**
La séquence d'horloge est aléatoire à chaque UUID (`getDistSeq`). La RFC 9562 (§ 6.3) la veut stable et incrémentée quand l'horloge recule, et demande de garantir l'unicité de deux UUID générés dans le même intervalle de 100 ns (compteur ou attente). Ici, l'unicité dans un même intervalle repose sur les 14 bits aléatoires.
Non reproduit : 0 doublon sur 1 000 000 d'UUID v1 générés sur 4 threads avec une adresse MAC fixe (le timestamp avance en général entre deux appels).
Depuis DV7 : timestamps strictement croissants dans le processus (`nextTimestamp`), l'unicité ne repose plus sur la séquence aléatoire. Reste : la séquence d'horloge est toujours tirée à chaque appel (la RFC la veut stable), sans conséquence sur l'unicité dans un processus.

### DV12. Tests UUID sans vérification — lecture — **traité le 2026-10-31**
`test/testUuid.cpp` (test `anch-device-uuid`) affiche un UUID v1 et un UUID « v6 » sans rien vérifier : DV7 passe inaperçu.
Piste : vérifier version, variant, horodatage décodé (proche de l'heure système), ordre croissant (v6), unicité sur un grand nombre d'UUID, nœud égal à l'adresse MAC passée en paramètre.
Depuis DV7 : `anch-device-uuid-provider-test` vérifie tout cela avec une adresse MAC fixe, et le choix du nœud par défaut depuis DV8 (`uuid-default-node`). Depuis DV9 : la validation de l'adresse passée en paramètre est testée (`uuid-mac-address`). `anch-device-uuid` (affichage seul) est conservé.

### DV13. `CPU` : membres non initialisés et compilation limitée à x86 — lecture — **corrigé le 2026-11-10**
- Si la feuille CPUID 1 n'est pas disponible, les 9 booléens ne sont pas initialisés.
- `<cpuid.h>` est inclus sans condition alors que `ANCH_CPU_DETECTION` est `ON` par défaut : sur une autre architecture, tout module qui inclut `cpu.hpp` (dont `anch-crypto`) ne compile pas.
- `anch_cpuid`, une fonction libre publique hors namespace, n'est utilisée que par le constructeur.
Correction : membres initialisés à `false`, `__get_cpuid` (qui contrôle la feuille maximale) avec les constantes `bit_*` de `<cpuid.h>`, protégé par `__x86_64__` / `__i386__` : toutes les fonctionnalités sont à `false` sur les autres architectures. `anch_cpuid` et sa variante Windows sont supprimées. Test `cpu-features` : chaque fonctionnalité est comparée à `__builtin_cpu_supports` de GCC.

### Tests réseau — **2026-11-10**
`anch-device-valid-network-interface` et `anch-device-invalid-network-interface` (affichage et code de retour, incompatibles avec la nouvelle API) sont remplacés par `anch-device-network-test` (anch-ut). Chaque information est comparée à une source indépendante :
- noms : `if_nameindex` ;
- IPv4, masque et broadcast : `ioctl` ;
- IPv6 : `/proc/net/if_inet6` ;
- adresse matérielle, MTU, drapeaux et métrique : `/sys/class/net`.
Les tests passent sur l'hôte, et dans un espace de noms réseau avec un tun point à point, une dummy sans IPv4, une étiquette `dum0:1`, plusieurs IPv4 sur une interface, un `/32 brd +` et de l'IPv6. Ils sont propres sous ASan / UBSan.

Remarque : la RFC 9562 recommande la v7 plutôt que la v1 / v6 pour les nouveaux usages (pas d'exposition de l'adresse MAC, tri chronologique) ; `anch-uuid` la fournit (`UUID::randomTime`).

## À approfondir
- [x] Analyser `uuidProvider` (DV7 à DV12).
