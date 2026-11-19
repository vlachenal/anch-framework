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
| anch-json | [anch-json/ANALYSE.md](anch-json/ANALYSE.md) | corrigés : `0` refusé, dépassements numériques, chaînes non échappées / non décodées, récursion non bornée, `max_payload` ignoré ; lecture 3,7× et écriture 2× plus rapides (2026-10-31) ; objets ignorés de plus d'un champ refusés (J14, 2026-11-10) ; données après la racine refusées, virgule finale refusée et corps JSON serveur bornés (J15, J16, J19, 2026-11-16) ; reste `'\0'` comme fin de données (J4) |
| anch-date | [anch-date/ANALYSE.md](anch-date/ANALYSE.md) | révisé et corrigé le 2026-11-03 (D1 à D15) : réécrit sur `std::chrono` + `localtime_r` (sans mutex ni `std::regex`, 11 classes de formateurs remplacées par 3 internes) ; heure d'été, `timespec`, `%y`, `%h`, analyse partielle / invalide / sans millisecondes, copie de `DateFormatter`, constructeurs implicites (à l'origine de S8 dans `anch-sql`) |
| anch-device | [anch-device/ANALYSE.md](anch-device/ANALYSE.md) | corrigé le 2026-11-10 (DV2 à DV6, DV13) : `Network` sur `getifaddrs` (toutes les interfaces, IPv6, plus d'échec global quand une interface disparaît), instantané copié (`std::optional` / map par valeur), `_metric` lu, `DeviceException` en `std::system_error` ; `CPU` initialisé et compilable hors x86 ; fournisseurs UUID v1 / v6 corrigés le 2026-10-31 (DV7 à DV12) |
| anch-ut | [anch-ut/ANALYSE.md](anch-ut/ANALYSE.md) | corrigé le 2026-11-03 (T3 à T20) : format des assertions vérifié à la compilation (3 messages faux trouvés dans `anch-json`), emplacement des échecs, `ThreadErrors` pour les assertions dans les threads, ordre d'enregistrement, erreurs de `setup` / d'initialisation / de désinitialisation, couleurs hors terminal ; options de runner refusées (pas implémentées) ; reste T2 (`--gen-ctest`) |
| anch-uuid | [anch-uuid/ANALYSE.md](anch-uuid/ANALYSE.md) | tous les constats corrigés le 2026-10-31 (UU1 à UU9) : parsing validé et 8× plus rapide, `toString` 14×, `random()` 56×, v3 / v5 conformes à la RFC 9562, v7 monotone, registre protégé, type valeur de 16 octets ordonnable et hachable ; fournisseurs v1 / v6 : voir `anch-device` |
| anch-sql | [anch-sql/ANALYSE.md](anch-sql/ANALYSE.md) | corrigé le 2026-11-03 (S5, S9 à S15) : requêtes préparées (nombre de paramètres donné par la base, analyseur lexical pour les `?` PostgreSQL, `NULL`, paramètres non liés refusés), erreurs fatales qui invalident la connexion (MySQL ne les détectait jamais), transaction terminée au retour dans le pool, erreurs de lecture des résultats ; cache des requêtes préparées borné (LRU, désallocation côté serveur, S17, 2026-11-14) ; transaction non validée annulée au retour dans le pool (S16, 2026-11-16) |
| anch-crypto | [anch-crypto/ANALYSE.md](anch-crypto/ANALYSE.md) | non analysé (hors périmètre) ; lecture hors limites AES-192 et lecture non initialisée SHA-2 corrigées ; débordement SHA-224 / SHA-384 et pointeur nul SHA-1 trouvés par ASan / UBSan, corrigés le 2026-11-12 (C4 à C6) ; Base64 corrigé le 2026-11-16 (C7 à C10 : octets nuls, lecture hors limites et encodage sans remplissage, flux) ; masquage de `Hash::digest(données)` corrigé le 2026-11-19 (C11) |
| anch-conf | [anch-conf/ANALYSE.md](anch-conf/ANALYSE.md) | cascade et placeholders terminés le 2026-11-12 : boucle infinie de `splitTrim` (deux profils ou includes), ordre profils / includes, résolution événementielle avec watchdog et détection de cycles |
| anch-ini | [anch-ini/ANALYSE.md](anch-ini/ANALYSE.md) | analysé le 2026-11-14 (I1 à I5) : lignes commentées contenant `=` lues comme des valeurs et valeur vide ignorée, corrigés (I1, I2) ; espaces avant `=` dans la clé et booléens invalides lus comme `false` corrigés le 2026-11-16 (I3, I4) ; I5 corrigé avec X1 |
| anch-cli | [anch-cli/ANALYSE.md](anch-cli/ANALYSE.md) | corrigé le 2026-11-03 (CL1 à CL10) : valeurs numériques hors limites ou vides, fichiers non ouverts, couleurs désactivées hors terminal / `NO_COLOR`, `operator<<` du `Formatter` hors namespace, `--no-term-fmt` déclaré par l'application, positionnel multiple non final, options répétées, codes ANSI dans les exceptions ; reste CL11 (`--`) ; CL12 (`--opt=val` / `-pval` non gérés) et CL13 (`pipe` sur un terminal) sont des choix |
| anch-cutils | [anch-cutils/ANALYSE.md](anch-cutils/ANALYSE.md) | corrigé le 2026-11-02 : tampons de lecture et d'écriture séparés, écritures partielles et erreurs gérées, vidage de `TcpStream` avant fermeture |
| anch-rest | [anch-rest/ANALYSE.md](anch-rest/ANALYSE.md) | réécrit le 2026-11-12 : modèle HTTP, serveur (`Api`), client et transport curl, module nginx ; corps en flux dans le module nginx (ABI 2, anch-cutils, contre-pression) le 2026-11-16 ; testé en CI (job `asan` et workflow `nginx` avec nginx 1.30.6) ; R1 à R4 corrigés, HTTP/2 testé et activé par défaut le 2026-11-19 |
| anch-oas | [anch-oas/ANALYSE.md](anch-oas/ANALYSE.md) | vérifié le 2026-11-14 avec un document piégeux : code généré compilable ; O1 à O3 mineurs, corrigés le 2026-11-19 |
| anch-app | [anch-app/ANALYSE.md](anch-app/ANALYSE.md) | débuté ; `after()` virtuel appelé depuis le destructeur, premier `-v` = `DEBUG` |

`anch-csv` (CMake seulement) et `anch-batch` (fichier `.todo` seulement) sont vides.

## État des tests

Le 2026-10-30, build Debug complet depuis la racine (sources actuelles, avec le correctif `_loaded` d'`anch-conf`) et serveurs MySQL / PostgreSQL démarrés :

| Module | Résultat |
|---|---|
| tous sauf anch-sql | 100 % |
| anch-sql | 75/75 avec serveurs MySQL et PostgreSQL, en séquentiel et avec `ctest -j` (2026-11-03) ; avant : 56/59 (S6) |
| anch-app | non compilé par défaut (anch-rest compilé par défaut depuis le 2026-11-19) |

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

## Passe du 2026-11-14

Après les réécritures et corrections des 2026-10-31 au 2026-11-14. Build complet (anch-rest, Protobuf, gRPC, Thrift, pilotes SQL) sous ASan / UBSan : **771/771 tests**.

Méthode : relecture du code le plus récent (anch-rest, anch-sql pour le cache des requêtes préparées, anch-conf) et du code jamais analysé (anch-ini), puis vérifications par exécution sous ASan / UBSan : fuzz par mutation des lecteurs JSON et multipart, documents invalides, document OpenAPI piégeux compilé après génération.
Non relus dans cette passe : anch-crypto, anch-utils, anch-events, anch-network, anch-cutils, anch-date, anch-uuid, anch-device, anch-ut, anch-cli (corrigés entre le 2026-10-31 et le 2026-11-10, couverts seulement par leurs tests), `anch-rest/thrift`.

| Constat | Module | Statut |
|---|---|---|
| I1 : une ligne commentée contenant `=` devient une valeur | anch-ini | corrigé le 2026-11-14 |
| I2 : `clé=` (valeur vide) ignorée | anch-ini | corrigé le 2026-11-14 |
| S18 : URI PostgreSQL sans encodage (mot de passe avec `@`, `/`, `%`) | anch-sql | corrigé le 2026-11-14 |
| J19 : `deserialize_max_field_char` jamais appliqué, corps JSON non borné | anch-json, anch-rest | corrigé le 2026-11-16 (option supprimée, corps JSON serveur bornés à 2 Mio, `413`) |
| J15 : données après la valeur racine acceptées | anch-json | corrigé le 2026-11-16 |
| J16 : virgule finale acceptée dans un objet mappé ou ignoré | anch-json | corrigé le 2026-11-16 |
| J17, J18 : caractères de contrôle, UTF-8 invalide acceptés | anch-json | vérifié |
| L17 : diagnostics d'anch-log sur la sortie standard | anch-log | corrigé le 2026-11-14 |
| X1 : fonctions exportées dans l'espace de noms global | transverse | corrigé le 2026-11-14 |
| C7, C8 : Base64, données binaires tronquées et lecture hors limites sans remplissage | anch-crypto | corrigé le 2026-11-16, avec C9 et C10 (encodage sans remplissage et d'un flux) |
| R1 à R4, O1 à O3 : mineurs, corrigés le 2026-11-19 | anch-rest, anch-oas | voir fichiers |

Sans défaut trouvé : lecteur JSON (2,7 millions de documents mutés, résultat identique pour toutes les tailles de tampon), lecteur multipart (délimiteur à cheval sur les blocs de lecture, 30 000 corps mutés), générateur anch-oas (identifiants réservés, collisions de noms, récursion).

### X1. Fonctions exportées dans l'espace de noms global — vérifié (`nm -DC`) — **corrigé le 2026-11-14**
Des fonctions utilitaires définies dans les `.cpp` sans `static` ni namespace anonyme sont exportées par les bibliothèques partagées avec des noms très génériques :
- `libanch-crypto.so` : `encode`, `decode`, `generateMD5UUID`, `generateSHA1UUID` ;
- `libanch-cli.so` : `setString`, `setTrue`, `setFalse`, `setPath`, `pushVec`, `openOFS`, … (17 fonctions) ;
- `libanch-conf.so` : `splitTrim`, `selectConfFile`, structure `ConfFile` ;
- `libanch-ini.so` : `parseValue`, `parseSection`, `parseLine`, `addIndent`, `printSection` ;
- `libanch-logger.so` : `toLevel`, `parseWriters`, `getAllWriters`, `createDefaultWriter`, `tokenizeLoggerName` ;
- `libanch-json.so` : `lexUnknownArray`, `lexUnknownObject`, `lexUnknownStr`, `parseUnknownNumber`, `computeErrorMessage` ;
- `libanch-device.so` : `generateUUIDv1`, `generateUUIDv6` ; `libanch-unit.so` : `doNothing`.

Une application (ou une autre bibliothèque) qui définit une fonction de même signature la substitue silencieusement à celle de la bibliothèque (interposition de symboles), ou viole l'ODR en liaison statique. anch-rest et anch-oas utilisent déjà des namespaces anonymes.
Correction : les 52 fonctions sont `static` (liaison interne), les classes locales `Core1` à `Core4` (MD5), `ConfFile` (anch-conf) et `MySQLInitializer` (pilote MySQL) sont dans un namespace anonyme. `nm -DC` ne montre plus, hors namespace `anch`, que les points d'entrée voulus (`create_connection`, `create_shared_connection`, `delete_connection` des pilotes SQL, `main` d'anch-ut). 774/774 tests sous ASan / UBSan, 694/694 en release.
Effet de bord : `decode` devenu `static` est intégré par GCC en release, ce qui a révélé une lecture non initialisée dans Base64 (voir C7 / C8 dans `anch-crypto`).
Visibilité masquée (`-fvisibility=hidden`) et macros d'export : faites le 2026-11-19, voir « Industrialisation, point 2 ».

## Industrialisation, point 1 : fiabilité vérifiée en continu (2026-11-16)

Outils ajoutés (presets dans `CMakePresets.json`, workflows dans `.github/workflows`) :

| Vérification | Preset / outil | Workflow | Résultat local |
|---|---|---|---|
| ThreadSanitizer | `tsan` (suppressions : `cmake/sanitizers/tsan.supp`, bibliothèques système non instrumentées : gRPC, Abseil, c-ares) | `sanitizers` | 790/790 |
| Module nginx sous ThreadSanitizer | nginx et application compilés avec `-fsanitize=thread` (`NGINX_CC_OPT`, `NGINX_LD_OPT`) | `nginx` (TSan) | 41 vérifications, aucun rapport avec des frames AnCH (rapports du cœur de nginx seulement : spinlocks et pool de threads) |
| Clang | `clang` (Clang 23) | `build` | 790/790 |
| GCC release, Ubuntu 24.04 (GCC 13) | `release`, `no-db` | `build` | release : 790/790 ; Ubuntu 24.04 (GCC 13.3, CMake 3.28, conteneur `ubuntu:24.04`) : 743/743 |
| Installation | `cmake/install-test/run-install-test.sh` (partagé et statique) | `build` | OK |
| Couverture | `coverage` + `cmake/coverage-report.sh` | `coverage` | 81,8 % des lignes (anch-sql 55,7 %, anch-cli 72,2 %, anch-network 76,8 %, anch-json 79,4 %) |
| Fuzzing | `fuzz` : 8 cibles libFuzzer (`fuzz/`) : JSON, multipart, formulaire, media type, UUID, dates, Base64, INI | `fuzz` (60 s par cible, 10 min par semaine) | 1 défaut (D17) ; 60 s par cible sans autre défaut |

Défauts trouvés :
- J20 (anch-json, TSan) : réveil perdu dans la sérialisation d'un `Flux`, et tests qui détachaient un producteur utilisant un flux local ;
- J21 (anch-json, Clang) : désérialisation d'un `Flux` résolue vers la surcharge générique ;
- D17 (anch-date, fuzzing) : dépassement signé pour une année hors de 1678-2261 ;
- B6 (build, test d'installation) : bibliothèques installées sans RPATH, une dépendance indirecte (`libanch-cli.so` pour `libanch-conf.so`) n'était pas trouvée hors des répertoires système ; corrigé par `INSTALL_RPATH` relatif (`$ORIGIN`, option `ANCH_INSTALL_RPATH`), aussi pour les pilotes SQL chargés par `dlopen` ;
- tests SQL de la fabrique (Ubuntu, preset `no-db`) : `db_con.conf` déclarait MySQL et PostgreSQL même sans leurs pilotes, et la fabrique crée un pool pour chaque base configurée ; la configuration est maintenant assemblée par CMake à partir des fragments `db_con-<pilote>.conf` des pilotes activés ;
- tests de dates (Ubuntu) : ils imposent `TZ=Europe/Paris` et ont besoin de `tzdata` (installé explicitement dans le job Ubuntu) ;
- conformité révélée par Clang : instanciations explicites par un nom introduit par `using` (128, anch-crypto et anch-json), `override` manquants (`reset` des modes de chiffrement), spécifications d'exception incohérentes entre déclaration et définition (destructeurs de `ReaderContext`, `SqlConnectionFactory`, `SQLite3Connection`), comparaison signée dans SHA-2, champ inutilisé (`MySQLPreparedStatement::_conn`).

À noter pour anch-crypto : les classes de hachage masquaient `Hash::digest(const std::string&)` (masquage de nom par `digest()`) ; corrigé le 2026-11-19 (C11).

## Industrialisation, point 2 : versions et compatibilité (2026-11-19)

Choix validés : tag `v0.2.0`, `CHANGELOG.md` (Keep a Changelog, aussi page Doxygen `changelog`), politique de dépréciation (`[[deprecated]]` + `\deprecated`, suppression à la mineure suivante avant 1.0.0, à la majeure suivante ensuite ; décrite dans `index.dox`), visibilité masquée.

Visibilité des symboles :
- `anch_add_library` compile les bibliothèques avec `CXX_VISIBILITY_PRESET hidden` (sans `VISIBILITY_INLINES_HIDDEN` : en debug, un consommateur appelle les fonctions inline des instanciations `extern template`, comme `BlockCipher<16>::getBlockSize()`, que la bibliothèque doit donc exporter ; trouvé par le preset `asan`), génère `anch-<cible>-export.hpp` (`generate_export_header`, macro `ANCH_<CIBLE>_EXPORT`, `ANCH_<CIBLE>_STATIC_DEFINE` public en statique) et l'installe ;
- annotés : 181 classes et templates de classes publics, 165 déclarations de fonctions libres et de variables `extern`, les templates instanciés explicitement (chiffrements, hachages, `HMAC`, `PrimitiveMapper`) ou porteurs d'un état statique (`json::Factory`), les points d'entrée des pilotes SQL et le `main` d'anch-ut ; `operator<<` de `SelectBuilder`, déclaré seulement en `friend`, est redéclaré au niveau du namespace ;
- `Singleton` et `events::detail::notifyingLists` (en-têtes seuls) ont `[[gnu::visibility("default")]]` : sinon chaque bibliothèque et l'application auraient leur propre instance (`LoggerFactory`, `Parsers`, `Resolvers`, `CPU`, `SQLSharedLibraries`) ;
- symboles exportés : 7 156 → 6 093 (release, anch-rest compris) ; les objets uniques perdus sont des tables constantes (`ESCAPED_CHARS`, mots-clés d'anch-oas) ; aucun symbole `anch::` déclaré dans un en-tête public n'a disparu ;
- `cmake/check-exports.sh` (job `build`) échoue si une bibliothèque exporte un symbole hors du namespace `anch` autre que les points d'entrée attendus, ou n'exporte aucun symbole `anch` ; appliqué aux anciennes bibliothèques, il signale les `setNum<T>` d'anch-cli encore exportés dans l'espace de noms global ;
- Doxygen : macros d'export définies vides (`PREDEFINED`, `MACRO_EXPANSION` limité aux macros prédéfinies) ; `fuzz/` et `cmake/` exclus (avertissements des sources du test d'installation et des cibles de fuzzing).

Vérifications : release GCC 790/790, Clang 790/790, ASan / UBSan 790/790, test d'installation partagé et statique, anch-app compilé (`ANCH_BUILD_APP=ON`).

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
