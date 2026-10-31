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

### S6. Tests non isolés : résultats dépendants de l'ordre et des exécutions précédentes — vérifié
Les tests MySQL, PostgreSQL et SQLite partagent l'état de la base : `*-update` suppose que `*-insert` a été exécuté avant, et `*-transactions` compte des lignes laissées par les exécutions précédentes. `anch.db` n'est copié qu'à la configuration CMake : son contenu s'accumule d'une exécution de `ctest` à l'autre.
Reproduction (2026-10-30) : avec `ctest -j`, `SQLite3-insert`, `SQLite3-update`, `SQLite3-update-stmt` et `SQLite3-transactions` échouent ; en séquentiel, il reste `PostgreSQL-update` (« Number of affected rows should be 1 instead of 0 ») et `SQLite3-transactions` (« Found 3 persons instead of 1 »), qui échouent aussi lancés seuls car ils dépendent des lignes laissées par les exécutions précédentes. `SQLite3-update` passe quand il est lancé seul.
Piste : fixtures CTest (`FIXTURES_SETUP` / `FIXTURES_REQUIRED`) qui recréent la base avant chaque série (`mysql_init.sql`, `postgresql_init.sql`, copie de `anch.db`), et `RESOURCE_LOCK` par base pour empêcher l'exécution parallèle.

### S7. Conversion des colonnes : valeurs négatives et `uint8_t` — renvoi
`src/sql/resultSet.cpp:99-186` et `postgresql/src/sql/postgresqlConnection.cpp:202` utilisent `anch::convert` : une colonne négative lue dans un type non signé est acceptée modulo 2^n, sans erreur (constat U9 de [../anch-utils/ANALYSE.md](../anch-utils/ANALYSE.md)).

## À approfondir
- [ ] Gestion des erreurs (connu).
- [ ] Fermeture des pilotes (`dlclose` dans le destructeur du singleton) alors que des connexions issues de ces pilotes peuvent encore exister dans les pools.
- [ ] Batch (`anch-batch.todo` : schéma par défaut).
