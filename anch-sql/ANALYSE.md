# Analyse — anch-sql

Analyse du 2026-10-27 ; requêtes préparées et erreurs fatales le 2026-11-03. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

Hors périmètre jusqu'au 2026-11-03 : la gestion des erreurs, que vous saviez manquante (traitée pour les erreurs fatales et le pool, S9 à S16).
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

### S5. Pilote introuvable : appel d'un pointeur de fonction indéterminé — lecture — **corrigé le 2026-11-03**
`src/sql/sqlSharedLibraries.cpp:58` : si `dlopen` échoue, l'entrée du pilote est quand même enregistrée (`_libraries[db] = lib`) avant de lever l'exception. Les champs `make_shared_connection` et `create` de `SQLSharedLibrary` (`inc/sql/sqlSharedLibraries.hpp:39-51`) ne sont pas initialisés. Un appel ultérieur à `registerSQLLibrary` pour ce pilote ne fait plus rien (l'entrée existe), et `makeConnection()` / `makeSharedConnection()` appellent alors une adresse indéterminée.
De même, le résultat de `dlsym` (ligne 63-64) n'est pas vérifié : un pilote sans `create_connection` provoque l'appel d'un pointeur nul.
Ce point relève en partie de la gestion d'erreurs, mais il transforme une erreur de configuration (pilote absent) en plantage.
Piste : n'enregistrer l'entrée qu'après un `dlopen` et des `dlsym` réussis ; initialiser les pointeurs à `nullptr`.
Correction : l'entrée n'est enregistrée qu'après `dlopen` et les deux `dlsym` réussis (sinon `dlclose` et `SqlException`), un nouvel essai est donc possible ; pointeurs initialisés à `NULL`.

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


### S9. Découpage des requêtes préparées : la plupart des `?` ne sont pas trouvés — vérifié — **corrigé le 2026-11-03**
`PreparedStatement::getPlaceholders` ne comptait un `?` qu'après un `=` ou dans des parenthèses, et considérait l'antislash comme un échappement dans toutes les bases.
Reproduction (2026-11-03, programme lié à `libanch-sql`) : 0 marqueur pour `a <> ?`, `a > ? AND b < ?`, `a LIKE ?`, `a BETWEEN ? AND ?`, `LIMIT ?`, soit justement ce que génère le constructeur de requêtes (`clauses.cpp`) : chaque requête de ce type échouait au premier `set` (« Index 1 is upper than number of wildcards 0 »). À l'inverse, les `?` dans un commentaire (`-- b = ?`) ou un identifiant (`"a=?"`) étaient comptés ; `'x\'` (littéral complet en PostgreSQL et SQLite) masquait la suite de la requête.
Conséquences par pilote : nombre de paramètres faux (refus de `set`, ou `MYSQL_BIND` alloués pour `n` marqueurs mais remplis avec toutes les valeurs : débordement de tas si un indice `0` était lié), réécriture `$n` incomplète pour PostgreSQL (`?` envoyé tel quel : erreur de syntaxe), paramètres non liés envoyés comme valeurs non initialisées (PostgreSQL) ou vides.
Correction :
- Le nombre de paramètres vient de la base : `mysql_stmt_param_count` (MySQL), `sqlite3_bind_parameter_count` (SQLite), `PQdescribePrepared` + `PQnparams` (PostgreSQL, qui accepte aussi des `$n` écrits à la main). Plus aucune analyse pour MySQL et SQLite.
- PostgreSQL seul a besoin de remplacer `?` par `$n` : nouvel analyseur lexical `anch::sql::findPlaceholders` / `numberPlaceholders` (`sql/placeholders.hpp`), paramétré par les règles du dialecte (`SqlSyntax::POSTGRESQL`, `MYSQL`, `SQLITE`, `STANDARD`) : chaînes (`''` doublé, antislash pour MySQL et `E'...'`), identifiants (`"..."`, `` `...` ``, `[...]`), commentaires (`--`, `/* */` imbriqués en PostgreSQL, `#` et `-- ` de MySQL), chaînes `$tag$...$tag$` (sans confondre `$1`). Limite documentée : l'opérateur `?` de `jsonb` n'est pas utilisable dans une requête préparée.
- `PreparedStatement` : valeurs dans un `std::vector` indexé (indice `0` et indices supérieurs au nombre de paramètres refusés), tous les paramètres doivent être liés avant l'exécution, `NULL` pris en charge (`setNull`, pointeur nul, `std::optional` vide), `clearParameters`, `getParameterCount`.
- Tests : `anch-placeholders-test` (7 cas dont les 17 anciens), `stmt-operators` et `stmt-null` sur les trois bases.

### S10. Erreurs fatales jamais prises en compte : connexions mortes remises dans le pool — vérifié — **corrigé le 2026-11-03**
`Connection::setValid` n'était jamais appelé et `SqlException::isOK` jamais lu : une connexion perdue restait « valide » et le pool la redonnait indéfiniment.
- MySQL : le code comparait la valeur de retour de `mysql_query` / `mysql_stmt_execute` (`1`) à `CR_SERVER_GONE_ERROR`, `CR_SERVER_LOST` au lieu de `mysql_errno` : aucune erreur n'était jamais fatale.
- PostgreSQL : à l'inverse, `PGRES_FATAL_ERROR` (toute erreur SQL, une faute de syntaxe par exemple) invalidait la connexion dans certaines fonctions.
Correction :
- Remontée centrale : toute `SqlException` levée par la connexion, ses requêtes préparées (`executeQuery` / `executeUpdate` deviennent des méthodes non virtuelles, les pilotes implémentent `doExecuteQuery` / `doExecuteUpdate`) et ses jeux de résultats (`next` appelle `fetchNext` des pilotes) passe par `Connection::guard` : l'erreur est notée (transaction à annuler) et, si elle est fatale, la connexion est invalidée.
- Détection par pilote : MySQL (`mysql::isFatal` : 2002, 2003, 2006, 2008, 2013, 2014, 2055, 1053, 1927, 4031 via `mysql_errno` / `mysql_stmt_errno`), PostgreSQL (fatale si `PQstatus` n'est plus `CONNECTION_OK` ; `isValid` interroge aussi `PQstatus`), SQLite (`IOERR`, `CORRUPT`, `NOTADB`, `CANTOPEN`, `NOMEM`).
- Tests : connexion tuée côté serveur (`pg_terminate_backend`, `KILL`) invalidée ; erreur SQL non fatale qui laisse la connexion valide (trois bases) ; tests sans base (`anch-sql-connection-test`, 8 cas).

### S11. Connexion rendue au pool sans fin de transaction — vérifié — **corrigé le 2026-11-03**
`Connection::release()` (documentée « used for connections' pool ») n'était appelée que par les destructeurs des pilotes : une transaction ouverte restait ouverte pour l'emprunteur suivant.
Correction : `anch::ResourcePool` appelle `release()` sur les ressources qui la définissent (concept `Releasable`, `anch-utils`) à leur retour, avant la validation. `release()` annule la transaction si une erreur a été signalée, la valide sinon (comportement existant conservé, voir S16) ; si elle ne peut être ni validée ni annulée, la connexion est invalidée (état inconnu).
Tests : `sql-pool-mysql`, `sql-pool-postgresql`, `sql-pool-sqlite3` (annulation après erreur, validation sinon, connexion tuée remplacée) ; `pool-*` sans base.

### S12. Erreurs de lecture des résultats ignorées — lecture — **corrigé le 2026-11-03**
- MySQL : `mysql_fetch_row` et `mysql_stmt_fetch` renvoyant une erreur (ou `MYSQL_DATA_TRUNCATED`) étaient traités comme la fin des résultats.
- PostgreSQL : un résultat en erreur (`PQgetResult` d'une requête invalide) était lu comme « 0 ligne » : la requête semblait réussir.
- SQLite : une erreur de `sqlite3_step` était lue comme la fin des résultats ; dans `executeUpdate` d'une requête préparée, une violation de contrainte était ignorée (et le résultat de `sqlite3_bind_text` jamais vérifié).
Correction : erreurs levées (avec leur statut fatal), résultats PostgreSQL restants consommés.

### S13. MySQL, requête préparée : `getValue(idx)` inversé et tampons démesurés — lecture — **corrigé le 2026-11-03**
`MySQLPreparedStatementResultSet::getValue(std::size_t)` renvoyait la valeur quand la colonne était `NULL` et rien sinon. Les tampons de colonnes étaient alloués à `field->length + 1` : 4 Go pour une colonne `LONGTEXT`.
Correction : tampons bornés à 4096 octets, valeurs plus longues relues à leur taille réelle (`mysql_stmt_fetch_column`), `getValue` corrigé.

### S14. Valeurs binaires tronquées au premier `'\0'` — lecture — **corrigé le 2026-11-03**
`MySQLResultSet` (`out = _row[idx]`) et `SQLite3ResultSet` (`std::string(data)`) ignoraient la longueur des valeurs. Correction : `mysql_fetch_lengths`, `sqlite3_column_bytes`.

### S15. PostgreSQL : nom des requêtes préparées — lecture — **corrigé le 2026-11-03**
Le nom utilisait `std::addressof(dbCon)`, l'adresse du paramètre (sur la pile) et non de la connexion. Remplacé par `anch_stmt_<compteur>`.

### S16. Validation implicite à la libération — lecture — **à décider**
`release()` valide la transaction en cours si aucune `SqlException` n'a été signalée. Si le code appelant échoue pour une autre raison (exception applicative entre deux requêtes) et rend la connexion au pool, ou détruit la connexion, le travail partiel est validé. Les pools usuels (HikariCP par exemple) annulent toute transaction non validée explicitement.
Piste : annuler systématiquement dans `release()` (la validation devenant toujours explicite avec `commit()`).

### S17. Cache des requêtes préparées non borné — lecture — **corrigé le 2026-11-14**
`Connection::prepareStatement` gardait chaque texte SQL distinct jusqu'à la fermeture de la connexion (`std::map` sans éviction ; côté PostgreSQL, requêtes jamais désallouées avant la fin de la session). Les requêtes construites avec des valeurs littérales (listes `IN` du builder, valeurs concaténées) faisaient croître la mémoire du client et du serveur sans limite ; avec MySQL, `max_prepared_stmt_count` (16 382 par défaut, pour tout le serveur) finit par faire échouer les préparations de toutes les connexions.
Correction : cache LRU borné par connexion (`statement-cache-size`, 256 par défaut, `Connection::setStatementCacheSize`). La requête la moins récemment utilisée est retirée et désallouée côté serveur (`PreparedStatement::deallocate` : `PQclosePrepared` pour PostgreSQL, accepté dans une transaction en erreur, `DEALLOCATE` avec une libpq antérieure à 17 ; destructeur pour MySQL et SQLite). Une requête dont un `ResultSet` est vivant n'est pas retirée (le cache peut alors dépasser temporairement sa taille) : requêtes imbriquées SQLite et `ResultSet` MySQL / SQLite qui pointent sur la requête. La référence renvoyée par `prepareStatement` reste valide tant que la requête n'est pas retirée (au moins `statement-cache-size - 1` autres préparations).
Tests : `statement-cache-lru`, `statement-cache-busy`, `statement-cache-size` (sans base) ; `statement-cache` pour les trois pilotes, `16-statement-cache-deallocate` (`pg_prepared_statements`, éviction dans une transaction en erreur) ; `sql-factory-statement-cache-size` (configuration). Sous ASan / UBSan également.
Restent (non traités) : une erreur `cached plan must not change result type` (PostgreSQL, SQLSTATE `0A000`, après un changement de schéma) laisse la requête en erreur dans le cache jusqu'à son éviction ou la fermeture de la connexion ; ré-exécuter une requête dont un `ResultSet` est encore ouvert (même texte SQL dans un `queryMapRow` imbriqué) réinitialise le curseur extérieur avec SQLite.

## À approfondir
- [x] Gestion des erreurs fatales et pool (S10, S11) ; reste : messages et codes d'erreur structurés (`SqlException` n'a qu'un message et un booléen).
- [ ] Validation à l'emprunt : une connexion restée longtemps dans le pool peut avoir été fermée par le serveur (`wait_timeout` MySQL) ; elle n'est invalidée qu'à sa première utilisation, qui échoue.
- [ ] Remarque de S8 : `anch::sql::Date` / `Time` modifient des champs sans recalculer le timestamp.
- [ ] Fermeture des pilotes (`dlclose` dans le destructeur du singleton) alors que des connexions issues de ces pilotes peuvent encore exister dans les pools.
- [ ] Batch (`anch-batch.todo` : schéma par défaut).
