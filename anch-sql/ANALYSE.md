# Analyse — anch-sql

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

Hors périmètre : la gestion des erreurs, que vous savez manquante.
Complété le 2026-10-30 : chargement des pilotes, conversion des colonnes, isolation des tests (S5 à S7).

## Constats

### S1. Le constructeur de requêtes utilise des paramètres liés — lecture (positif)
`src/sql/builder/clauses.cpp` génère des `?` (`column = ?`, `BETWEEN ? AND ?`...) et l'exécution passe par des requêtes préparées. Les valeurs ne sont pas concaténées dans le SQL : pas d'injection de ce côté.
À noter : les noms de colonnes et de tables sont concaténés tels quels ; ils ne doivent jamais venir d'une entrée utilisateur.

### S2. Tests dépendants de serveurs — vérifié
Le 2026-10-27, 39 tests sur 59 passaient : les 20 échecs concernaient MySQL et PostgreSQL, faute de serveur local.
Le 2026-10-30, avec des serveurs MySQL et PostgreSQL démarrés et le correctif `_loaded` d'`anch-conf`, 56 tests sur 59 passent ; les 3 échecs restants sont expliqués par S6.
Piste : option CMake pour désactiver ces tests, ou conteneurs de test.

### S3. Pool de connexions — renvoi
`sqlConnectionFactory` s'appuie sur `anch::ResourcePool` : il hérite des constats U6 à U8 de [../anch-utils/ANALYSE.md](../anch-utils/ANALYSE.md).

### S4. Build — **corrigé le 2026-10-29**
- Les tests `SQLConnectionFactory*` et `SqlStmtWC-*` utilisaient `${EXECUTABLE_OUTPUT_PATH}anch-...` (barre oblique manquante) ; ils référencent maintenant les cibles.
- `LD_LIBRARY_PATH=./lib` remplacé par le vrai répertoire des bibliothèques (`$<TARGET_FILE_DIR:anch-sql>`), et le test de la factory dépend des pilotes activés.
- MySQL est trouvé via pkg-config (`libmariadb` ou `mysqlclient`), PostgreSQL et SQLite via les modules fournis par CMake.
- `SQLConnectionFactorySQLite` échouait à cause du bug `_loaded` d'`anch-conf` ; il passe depuis le correctif du 2026-10-30.

### S5. Pilote introuvable : appel d'un pointeur de fonction indéterminé — lecture
`src/sql/sqlSharedLibraries.cpp:58` : si `dlopen` échoue, l'entrée du pilote est quand même enregistrée (`_libraries[db] = lib`) avant de lever l'exception. Les champs `make_shared_connection` et `create` de `SQLSharedLibrary` (`inc/sql/sqlSharedLibraries.hpp:39-51`) ne sont pas initialisés. Un appel ultérieur à `registerSQLLibrary` pour ce pilote ne fait plus rien (l'entrée existe), et `makeConnection()` / `makeSharedConnection()` appellent alors une adresse indéterminée.
De même, le résultat de `dlsym` (ligne 63-64) n'est pas vérifié : un pilote sans `create_connection` provoque l'appel d'un pointeur nul.
Ce point relève en partie de la gestion d'erreurs, mais il transforme une erreur de configuration (pilote absent) en plantage.
Piste : n'enregistrer l'entrée qu'après un `dlopen` et des `dlsym` réussis ; initialiser les pointeurs à `nullptr`.

### S6. Tests non isolés : résultats dépendants de l'ordre et des exécutions précédentes — vérifié — **corrigé le 2026-10-31**
Les tests MySQL, PostgreSQL et SQLite partagent l'état de la base : `*-update` suppose que `*-insert` a été exécuté avant, et `*-transactions` compte des lignes laissées par les exécutions précédentes. `anch.db` n'est copié qu'à la configuration CMake : son contenu s'accumule d'une exécution de `ctest` à l'autre.
Reproduction (2026-10-30) : avec `ctest -j`, `SQLite3-insert`, `SQLite3-update`, `SQLite3-update-stmt` et `SQLite3-transactions` échouent ; en séquentiel, il reste `PostgreSQL-update` (« Number of affected rows should be 1 instead of 0 ») et `SQLite3-transactions` (« Found 3 persons instead of 1 »), qui échouent aussi lancés seuls car ils dépendent des lignes laissées par les exécutions précédentes. `SQLite3-update` passe quand il est lancé seul.
Piste : fixtures CTest (`FIXTURES_SETUP` / `FIXTURES_REQUIRED`) qui recréent la base avant chaque série (`mysql_init.sql`, `postgresql_init.sql`, copie de `anch.db`), et `RESOURCE_LOCK` par base pour empêcher l'exécution parallèle.
État avant correction (2026-10-31) : 55 lignes dans `T_Test` sous MySQL (dont 51 « Update ») et 51 sous PostgreSQL, accumulées par les exécutions précédentes.
Correction (chaque cas indépendant, plutôt qu'une série ordonnée) :
- `testSQLite3.cpp`, `testMySQL.cpp`, `testPostgreSQL.cpp` : fonction `resetData`, enregistrée avec `beforeTest`, qui ne garde que les 4 lignes du script d'initialisation (`DELETE FROM T_Test WHERE id > 4`) ; les tests ne modifient jamais ces lignes (ils ciblent « Insert », « Update », « Roll », « Com », « Batch … »).
- Préconditions locales : `update`, `update-stmt` et `delete` insèrent eux-mêmes la ligne qu'ils modifient ou suppriment (`insertTestPerson`), au lieu de dépendre du test précédent.
- `06-delete` réactivé pour MySQL et PostgreSQL (il était en commentaire) ; `SQLite3-delete`, `MySQL-delete` et `PostgreSQL-delete` enregistrés dans CMake (le cas SQLite était l'oubli restant de T6 dans `anch-ut/ANALYSE.md`).
- `RESOURCE_LOCK` par base (`anch_mysql`, `anch_pgsql`, `anch_sqlite`) sur tous les tests d'une base, y compris `SQLConnectionFactory*` qui y écrit aussi : jamais deux tests de la même base en parallèle ; les bases différentes et les autres tests restent parallèles.
- Vérifié : 64 tests `sql` passent avec `ctest -j12` (3 exécutions de suite) et en séquentiel ; les 12 tests d'écriture (`update`, `delete`, `update-stmt`, `transactions` des trois bases) passent lancés seuls ; les trois exécutables passent avec tous leurs cas dans un seul processus ; après les tests, chaque base contient de nouveau ses 4 lignes initiales.
- Limite : la remise à zéro suppose que les lignes 1 à 4 du script d'initialisation sont intactes (elles le sont : aucun test ne les modifie).

### S7. Conversion des colonnes : valeurs négatives et `uint8_t` — renvoi
`src/sql/resultSet.cpp:99-186` et `postgresql/src/sql/postgresqlConnection.cpp:202` utilisent `anch::convert` : une colonne négative lue dans un type non signé est acceptée modulo 2^n, sans erreur (constat U9 de [../anch-utils/ANALYSE.md](../anch-utils/ANALYSE.md)).

### S8. Lecture des colonnes `DATE`, `TIME` et `TIMESTAMP` : date courante au lieu de la valeur — vérifié — **corrigé le 2026-11-03**
`ResultSet::get<anch::sql::Date>` (et `Time`, `Timestamp`, `src/sql/resultSet.cpp`) faisait `out = new anch::sql::Date(date);` : `out` est une référence, pas un pointeur. Le pointeur était converti en `bool` et passé au constructeur implicite `Date(bool init = true)` : la valeur lue était remplacée par la date courante, et l'objet alloué n'était jamais libéré. De plus, l'analyse partait d'une `Date()` (maintenant) et gardait l'heure courante pour une colonne `DATE` (D8 d'`anch-date`).
Reproduction (2026-11-03, même construction sur une classe dérivée de `Date`) : `2001-01-01` lu comme `2026-11-03`.
Correction : `out = anch::sql::Date(date);` (idem `Time`, `Timestamp`) ; constructeurs `bool` de `Date`, `Time` et `Timestamp` rendus `explicit` ; documentation « You have to delete result » retirée.
Non couvert par un test : les bases de test n'ont pas de colonne date.
Remarque : les constructeurs de `anch::sql::Date` et `anch::sql::Time` remettent à zéro des champs (heure, ou date) sans recalculer le timestamp : deux `anch::sql::Date` du même jour à des heures différentes ne sont pas égales. À revoir avec ce module.

## À approfondir
- [ ] Gestion des erreurs (connu).
- [ ] Fermeture des pilotes (`dlclose` dans le destructeur du singleton) alors que des connexions issues de ces pilotes peuvent encore exister dans les pools.
- [ ] Batch (`anch-batch.todo` : schéma par défaut).
