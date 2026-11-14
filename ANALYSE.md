# Analyse du framework AnCH — vue d'ensemble

Analyse réalisée le 2026-10-27, complétée le 2026-10-30, sans les répertoires `build/`.

Contexte pris en compte :
- le projet n'est pas fini et n'est pas destiné pour l'instant à la production ;
- `anch-crypto` est un projet « pour le fun », qui devrait devenir un wrapper sur OpenSSL ;
- la gestion des erreurs de `anch-sql` est manquante (connu) ;
- `anch-conf` et `anch-ini` sont en construction pour la configuration en cascade par profil.

Statut des constats :
- **vérifié** : reproduit par une compilation, un lien ou un test ;
- **lecture** : déduit de la lecture du code, non reproduit.

## Analyses par librairie

| Librairie | Fichier | Constats principaux |
|---|---|---|
| anch-utils | [anch-utils/ANALYSE.md](anch-utils/ANALYSE.md) | tous les constats corrigés le 2026-11-02 (`ThreadPool` réécrit, `ResourcePool`, `convert`, `Singleton`, `Optional`, `Fluent`, `Flux`, `endianness`)  |
| anch-events | [anch-events/ANALYSE.md](anch-events/ANALYSE.md) | corrigé le 2026-11-02 : `EventBus` (thread permanent, `fireEvent` non bloquant et appelable depuis un handler), `Observable` (plus d'interblocage, `removeObserver` attend les diffusions en cours) |
| anch-log | [anch-log/ANALYSE.md](anch-log/ANALYSE.md) | corrigé le 2026-11-02 : motif par défaut, `MDC`, `LowPriorityWriter`, `max-size` et rotation, `getLogger` concurrent, writers par logger inversés, erreurs d'écriture fatales, fichiers non vidés |
| anch-network | [anch-network/ANALYSE.md](anch-network/ANALYSE.md) | refondu le 2026-11-03 (N1 à N10) : sockets en flux C++ (`TcpStream` = `std::iostream`, `TcpServer::accept`, `UdpSocket` par datagrammes), `Socket` RAII, IPv4 / IPv6, `errno` dans `IOException` (`std::system_error`), plus d'événements ; 9 tests, propres sous ASan / UBSan |
| anch-json | [anch-json/ANALYSE.md](anch-json/ANALYSE.md) | corrigés : `0` refusé, dépassements numériques, chaînes non échappées / non décodées, récursion non bornée, `max_payload` ignoré ; lecture 3,7× et écriture 2× plus rapides (2026-10-31) ; objets ignorés de plus d'un champ refusés (J14, 2026-11-10) ; reste `'\0'` comme fin de données (J4) |
| anch-date | [anch-date/ANALYSE.md](anch-date/ANALYSE.md) | révisé et corrigé le 2026-11-03 (D1 à D15) : réécrit sur `std::chrono` + `localtime_r` (sans mutex ni `std::regex`, 11 classes de formateurs remplacées par 3 internes) ; heure d'été, `timespec`, `%y`, `%h`, analyse partielle / invalide / sans millisecondes, copie de `DateFormatter`, constructeurs implicites (à l'origine de S8 dans `anch-sql`) |
| anch-device | [anch-device/ANALYSE.md](anch-device/ANALYSE.md) | corrigé le 2026-11-10 (DV2 à DV6, DV13) : `Network` sur `getifaddrs` (toutes les interfaces, IPv6, plus d'échec global quand une interface disparaît), instantané copié (`std::optional` / map par valeur), `_metric` lu, `DeviceException` en `std::system_error` ; `CPU` initialisé et compilable hors x86 ; fournisseurs UUID v1 / v6 corrigés le 2026-10-31 (DV7 à DV12) |
| anch-ut | [anch-ut/ANALYSE.md](anch-ut/ANALYSE.md) | corrigé le 2026-11-03 (T3 à T20) : format des assertions vérifié à la compilation (3 messages faux trouvés dans `anch-json`), emplacement des échecs, `ThreadErrors` pour les assertions dans les threads, ordre d'enregistrement, erreurs de `setup` / d'initialisation / de désinitialisation, couleurs hors terminal ; options de runner refusées (pas implémentées) ; reste T2 (`--gen-ctest`) |
| anch-uuid | [anch-uuid/ANALYSE.md](anch-uuid/ANALYSE.md) | tous les constats corrigés le 2026-10-31 (UU1 à UU9) : parsing validé et 8× plus rapide, `toString` 14×, `random()` 56×, v3 / v5 conformes à la RFC 9562, v7 monotone, registre protégé, type valeur de 16 octets ordonnable et hachable ; fournisseurs v1 / v6 : voir `anch-device` |
| anch-sql | [anch-sql/ANALYSE.md](anch-sql/ANALYSE.md) | corrigé le 2026-11-03 (S5, S9 à S15) : requêtes préparées (nombre de paramètres donné par la base, analyseur lexical pour les `?` PostgreSQL, `NULL`, paramètres non liés refusés), erreurs fatales qui invalident la connexion (MySQL ne les détectait jamais), transaction terminée au retour dans le pool, erreurs de lecture des résultats ; cache des requêtes préparées borné (LRU, désallocation côté serveur, S17, 2026-11-14) ; à décider : validation implicite à la libération (S16) |
| anch-crypto | [anch-crypto/ANALYSE.md](anch-crypto/ANALYSE.md) | non analysé (hors périmètre) ; lecture hors limites AES-192 et lecture non initialisée SHA-2 corrigées ; débordement SHA-224 / SHA-384 et pointeur nul SHA-1 trouvés par ASan / UBSan, corrigés le 2026-11-12 (C4 à C6) |
| anch-conf | [anch-conf/ANALYSE.md](anch-conf/ANALYSE.md) | cascade et placeholders terminés le 2026-11-12 : boucle infinie de `splitTrim` (deux profils ou includes), ordre profils / includes, résolution événementielle avec watchdog et détection de cycles |
| anch-ini | [anch-ini/ANALYSE.md](anch-ini/ANALYSE.md) | en construction ; conversion numérique correcte (modèle pour les autres modules) |
| anch-cli | [anch-cli/ANALYSE.md](anch-cli/ANALYSE.md) | corrigé le 2026-11-03 (CL1 à CL10) : valeurs numériques hors limites ou vides, fichiers non ouverts, couleurs désactivées hors terminal / `NO_COLOR`, `operator<<` du `Formatter` hors namespace, `--no-term-fmt` déclaré par l'application, positionnel multiple non final, options répétées, codes ANSI dans les exceptions ; reste CL11 (`--`) ; CL12 (`--opt=val` / `-pval` non gérés) et CL13 (`pipe` sur un terminal) sont des choix |
| anch-cutils | [anch-cutils/ANALYSE.md](anch-cutils/ANALYSE.md) | corrigé le 2026-11-02 : tampons de lecture et d'écriture séparés, écritures partielles et erreurs gérées, vidage de `TcpStream` avant fermeture |
| anch-rest | [anch-rest/ANALYSE.md](anch-rest/ANALYSE.md) | réécrit le 2026-11-12 : modèle HTTP, serveur (`Api`), client et transport curl, module nginx ; testé en CI (job `asan` et workflow `nginx` avec nginx 1.30.6) ; désactivé par défaut |
| anch-app | [anch-app/ANALYSE.md](anch-app/ANALYSE.md) | débuté ; `after()` virtuel appelé depuis le destructeur, premier `-v` = `DEBUG` |

`anch-csv` (CMake seulement) et `anch-batch` (fichier `.todo` seulement) sont vides.

## État des tests

Le 2026-10-30, build Debug complet depuis la racine (sources actuelles, avec le correctif `_loaded` d'`anch-conf`) et serveurs MySQL / PostgreSQL démarrés :

| Module | Résultat |
|---|---|
| tous sauf anch-sql | 100 % |
| anch-sql | 75/75 avec serveurs MySQL et PostgreSQL, en séquentiel et avec `ctest -j` (2026-11-03) ; avant : 56/59 (S6) |
| anch-rest, anch-app | non compilés par défaut |

Le 2026-10-27, avec les anciens `build/` : anch-sql 39/59 (pas de serveur local), anch-rest 0/19.

Les tests passent alors que de nombreux constats sont confirmés : ils ne couvrent ni la concurrence, ni les cas limites (zéro, dépassements, chaînes échappées, heure d'été), ni les gros messages réseau. La plupart des reproductions des fichiers `ANALYSE.md` peuvent servir de base à des tests de non-régression.

## Points transverses

- **Conversions numériques** : trois implémentations différentes. `anch-ini` (`toNumber`) contrôle correctement `std::from_chars` ; `anch-cli` (CL1) oubliait `res.ec` (corrigé le 2026-11-03) ; `anch-utils` (`convert`, U9 / U10) passe par un flux ; `anch-json` (J7, J9) est corrigé. Une fonction commune dans `anch-utils`, sur le modèle de `toNumber`, réglerait les quatre.
- **Observateurs et threads** : `Observable` (E7), `EventBus` (E1, E2) et `EventHandler` (E8) appellent les handlers en tenant des verrous ou sans protéger la liste. `anch-network` et `anch-ut` n'en dépendent plus depuis le 2026-11-03.
- **Sanitizers** : N1, N3, U13 n'apparaissaient qu'avec ASan. Preset `asan` (`-fsanitize=address,undefined`) ajouté le 2026-11-12 : premier passage, 9 échecs dans anch-crypto (C4 à C6), corrigés ; 629/629 depuis.

## Système de build (transverse)

### B1. Le build debug ne compile pas — vérifié — **corrigé le 2026-10-29** (migration CMake)
`-D_FORTIFY_SOURCE=2` sans optimisation déclenche un `#warning` de la glibc, transformé en erreur par `-Werror`. L'alternative `-g` commentée dans chaque `.cmake.in` ne fonctionne donc pas.
Piste : ne définir `_FORTIFY_SOURCE` qu'avec `-O1` ou plus (par exemple via `$<$<NOT:$<CONFIG:Debug>>:...>`).

### B2. `-z relro,now` n'atteint pas le linker — vérifié — **corrigé le 2026-10-29** (migration CMake)
L'option est passée dans `add_definitions`, donc seulement à la compilation. `readelf -d anch-conf/build/lib/libanch-conf.so` ne montre pas `BIND_NOW`.
Piste : `add_link_options(-Wl,-z,relro,-z,now)`.

### B3. Options de compilation dupliquées — lecture — **corrigé le 2026-10-29** (migration CMake)
Les mêmes options sont copiées dans 18 `CMakeLists.txt`. `Anch.cmake.in` contient `-ftrap` au lieu de `-ftrapv`.

### B4. Dépendances par chemins codés en dur — lecture — **corrigé le 2026-10-29** (migration CMake)
Les `<Module>.cmake.in` pointent vers `$ENV{HOME}/projects/anch-framework/<dep>/inc` et `<dep>/build/lib`. Les modules doivent être compilés à la main, dans le bon ordre.
Piste : finir la migration vers `Anch.cmake.in` et utiliser des cibles CMake entre modules (`target_link_libraries(anch-conf PUBLIC anch-ini)`, `target_include_directories(... PUBLIC inc)`).

### B5. Build racine incomplet — lecture — **corrigé le 2026-10-29** (migration CMake)
- `anch-ini` n'est pas inclus alors que `anch-conf` en dépend ; `anch-app` et `anch-csv` non plus.
- `enable_testing()` dépend de `ANCH_JSON_UNIT_TEST`.
- `cmake_minimum_required` vaut 4.0 à la racine contre 3.10 dans les modules.

### Migration CMake (2026-10-29)
Le build est désormais un projet CMake unique fondé sur des cibles : `cmake/AnchHelpers.cmake`, options centralisées dans le `CMakeLists.txt` racine, tests dans `<module>/test/CMakeLists.txt`, presets dans `CMakePresets.json`, paquet installable (`find_package(Anch)`). Les `<Module>.cmake.in`, `FindMySQL.cmake` et `FindSQLite3.cmake` ont été supprimés.
Vérifications faites : build Release complet, build Debug complet (305/305 tests hors SQL), édition de liens de deux TU incluant `threadPool.hpp`, `cpu.hpp` et `ut/assert.hpp`, installation puis projet consommateur via `find_package(Anch)`, présence de `BIND_NOW`.
Problèmes révélés par la migration :
- `anch-crypto` C2 : lecture hors limites en AES-192 (corrigée) ;
- `anch-conf` : `Configuration::load()` ne mettait jamais `_loaded` à `true` (corrigé provisoirement le 2026-10-30, sauf en présence de placeholders) ;
- `anch-cli` : deux tests lisaient `Makefile`, qui n'existe pas avec d'autres générateurs (corrigé) ;
- `anch-rest` : ne compile plus (erreur dans `mapperRegistry.hpp`), module désactivé par défaut.

## Divers

- L'en-tête de licence indique encore « based on C++11 standard ».
- `anch-json/CMakeLists.txt.bak` traîne, non suivi par git.

## Priorités suggérées

Mise à jour le 2026-10-30, par impact sur l'usage courant :

1. ~~`anch-json` J6 : la valeur `0` est refusée.~~ Corrigé le 2026-10-30.
2. ~~`anch-cutils` CU1 : en lecture / écriture, les données envoyées sont corrompues (impacte `TcpStream`).~~ Corrigé le 2026-11-02.
3. ~~`anch-log` L8 et L9 : le motif par défaut n'écrit pas le message, et `MDC.add` / `reset` ne fonctionnent pas.~~ Corrigé le 2026-11-02.
4. ~~`anch-date` D3 : une heure d'avance en été (impacte aussi les horodatages des logs).~~ Corrigé le 2026-11-03.
5. ~~`anch-network` N1 / N3 : use-after-free et débordement de tampon.~~ Corrigé (refonte) le 2026-11-03.
6. ~~`anch-json` J1 / J2 : échappement des chaînes.~~ Corrigé.
7. ~~`anch-events` E2 / E7 : arrêt ou interblocage dès qu'un handler interagit avec le bus ou l'observable.~~ Corrigé le 2026-11-02.

Déjà corrigés : U1 (`ThreadPool`), B1 à B5 (build), C2 (AES-192), J6, J7 et J9 en JSON, `_loaded` (provisoire).
