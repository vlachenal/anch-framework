# Analyse — anch-ut

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre : `main.cpp` (options de ligne de commande), `unit.hpp`, runners (`defaultRunner`, `socketServerRunner`, `socketClientRunner`), vérification des symboles définis dans les headers publics (complété le 2026-10-30) ; le 2026-10-31 : `assert.hpp`, `impl/assert.hpp`, `unit.cpp`, `defaultRunner.cpp`, tests propres, intégration CTest de tout le dépôt et performances.
Vérifications du 2026-10-31 : exécutable de test piégé (`probe`) lié à `libanch-unit`, comparaison des cas enregistrés (`-l`) avec ceux lancés par CTest (`ctest --show-only=json-v1`).
Non analysé : plugin GCC (`anch-ut-gcc-attr-plugin`).

## Constats

### T1. Spécialisation `equals<long double>` sans `inline` — vérifié — **corrigé le 2026-10-29**
`inc/ut/impl/assert.hpp:168-169` : une spécialisation explicite complète de fonction template n'est pas implicitement `inline`. `nm` montre un symbole fort (`T`) dans chaque fichier qui inclut `ut/assert.hpp` : un exécutable de test composé de deux `.cpp` qui incluent ce header ne se lie pas.
Piste : `template<> inline bool equals(...)`.

### T2. Une seule suite par exécutable — lecture (conception)
`anch-ut` fournit `main()`, et chaque exécutable définit un seul `anch::ut::setup`. Il faut donc un exécutable par fichier de test, et une ligne `add_test` par cas dans CMake. `--gen-ctest` existe dans les options : il pourrait générer ces lignes automatiquement, mais il n'est pas implémenté (voir T4) (`AnchTest.cmake` / `anch_add_tests` est encore vide ; les tests sont désormais déclarés dans `<module>/test/CMakeLists.txt`).

### T3. `-r` / `--runner` provoque une erreur de segmentation — vérifié
`src/ut/main.cpp:126` : un runner n'est créé que si `--runner` est **absent** (`DefaultRunner`) ; les autres runners sont un `\todo`. Avec `--runner`, `runner` reste un `std::shared_ptr` nul et `runner->startup()` le déréférence.
Reproduction (2026-10-30) : `anch-uuid-test -r tcp://localhost:54321` : erreur de segmentation (code 139).
Piste : tant que les runners socket ne sont pas implémentés, refuser l'option avec un message d'erreur.

### T4. Options `--start`, `--stop` et `--gen-ctest` sans effet — vérifié
Ces options sont analysées (`opts.start`, `opts.shutdown`, `opts.genCTEST`) mais jamais utilisées. `--gen-ctest` lance simplement tous les tests.
Les runners socket sont des squelettes : `SocketServerRunner` (constructeur et `handle()` vides, `src/ut/socketServerRunner.cpp`) ; `SocketClientRunner` n'a qu'un header, sans implémentation.

### T5. Un test qui appelle `exit()` termine la suite avec succès — vérifié — **corrigé le 2026-10-31**
`DefaultRunner::test` n'intercepte que les exceptions. Si un test (ou le code testé) appelle `std::exit(0)` (ou `exit` avec n'importe quel code nul), le processus s'arrête aussitôt : les tests suivants ne sont pas exécutés et le code de sortie est `0`.
Reproduction (2026-10-31) : `probe -t exit-in-test -t z-registered-first` : `z-registered-first` ne s'exécute pas, code de sortie `0` ; lancé sans `-t`, tous les tests classés après `exit-in-test` sont ignorés et la suite « passe ».
Avec CTest (un processus par cas), seul le cas concerné est faussement réussi ; avec `-t all` ou sans `-t`, toute la fin de la suite l'est.
Piste : pendant l'exécution d'un test, un gestionnaire `std::atexit` qui affiche le test en cours et force un code d'échec (`std::_Exit(EXIT_FAILURE)`) ; à défaut, exécuter chaque test dans un processus fils (`fork`), ce qui couvrirait aussi les plantages (T10).
Correction (`src/ut/defaultRunner.cpp`) :
- `DefaultRunner` enregistre une fois un gestionnaire avec `std::atexit` et `std::at_quick_exit`.
- Chaque étape est marquée pendant son exécution (`StepGuard`) : initialisation de la suite, chaque test (fonctions `beforeTest` et `afterTest` comprises), désinitialisation. Un `exit(0)` dans la désinitialisation (appelée après les tests) faisait lui aussi passer une suite en échec.
- Si `exit()` ou `quick_exit()` est appelé pendant une étape, le gestionnaire affiche « exit() has been called during <étape>: unit tests are failed », vide les tampons de sortie (`fflush`) et termine avec `EXIT_FAILURE` (`std::_Exit`). Hors étape (fin normale de `main`), rien ne change.
- Non couvert : `std::_Exit`, `abort` et les plantages (code de sortie non nul de toute façon), ni la poursuite des tests restants après un `exit()` (il faudrait un processus par test).
- Tests (`anch-ut/test`) : cas `exit-in-test`, et `exit(0)` dans l'initialisation et la désinitialisation (variables `ANCH_UT_EXIT_IN_INIT` / `ANCH_UT_EXIT_IN_UNINIT`), chacun vérifié deux fois : code de sortie en échec (`WILL_FAIL`) et message attendu (`PASS_REGULAR_EXPRESSION`).
- Vérifié : avec l'ancienne bibliothèque, `exit-in-test` et l'`exit(0)` de la désinitialisation sortent avec `0` ; avec la nouvelle, avec `1` et le message. Les 450 tests du dépôt passent ; `anch-json/test/testReader.cpp`, qui appelle `exit(1)` sur une erreur de lecture, reste un échec, désormais avec le nom du test.

### T6. 13 cas de test enregistrés ne sont jamais lancés par CTest — vérifié — **corrigé le 2026-10-31**
Chaque cas doit être déclaré deux fois : `.add(...)` dans le code et `add_test(... -t <nom>)` dans CMake (T2). Un oubli côté CMake est silencieux.
Reproduction (2026-10-31), cas listés par `-l` mais absents de CTest :
- `anch-fluent-test` : `collect-map` ;
- `anch-flux-test` : `push-async-10-ok`, `push-async-100-ok`, `push-async-10000-ok`, `push-sync-10-ok`, `push-sync-100-ok`, `push-sync-1000-ok`, `push-sync-10000-ok` ;
- `anch-json-lexer-test` : `lexer-bool-ko` ; `anch-json-mapper-test` : `json-deser-discard128-col-buf2` ;
- `anch-select-builder-test` : `inner_join_no_where`, `inner_join_without_valid_clause` ;
- `anch-sqlite3-test` : `delete`.
Lancés à la main : 12 passent ; `anch-sqlite3-test -t delete` échoue seul (« Number of affected rows should be 1 instead of 0 ») car il dépend de la ligne créée par `update` (S6 dans `anch-sql/ANALYSE.md`).
L'inverse est détecté : un nom inconnu passé à `-t` fait échouer l'exécutable.
Piste : générer les `add_test` depuis la liste des cas (`--gen-ctest` + `anch_add_tests`, T4), ou ajouter par exécutable un test CTest qui compare `-l` aux cas déclarés.
Historique : aucune version des fichiers CMake ne contenait ces cas (`git log -S`) ; ils n'ont jamais été déclarés.
Correction : `add_test` ajoutés à côté des cas voisins, avec la convention de nommage de chaque fichier : `anch-fluent-collect-map`, `anch-flux-push-{async,sync}-*-ok-test` (`anch-utils/test`), `lexer-bool-ko`, `json-deser-discard128-col-buf2` (`anch-json/test`), `InnerJoin`, `InnerJoinWithoutValidClause` (`anch-sql/test`). Les 12 passent ; la comparaison `-l` / CTest ne signale plus que `delete`.
Ajouté avec S6 (`anch-sql/ANALYSE.md`, même jour) : `anch-sqlite3-test -t delete`, une fois les tests SQL isolés. Avant cela : Il attend exactement une ligne « Update Test » : enregistré après `update-stmt` (qui en crée une seconde), il échoue dès la première exécution (vérifié, 2 passes séquentielles) ; placé juste après `update`, il échouerait à partir de la deuxième exécution, `anch.db` accumulant les lignes (S6). À traiter avec S6 (tests isolés), ou à enregistrer en sachant qu'il échouera.

### T7. `anch::ut::assert` inutilisable avec `<cassert>` — vérifié — **corrigé le 2026-10-31**
`assert` est aussi la macro de `<cassert>`. Si `<cassert>` est inclus avant `ut/assert.hpp`, la déclaration du header elle-même ne compile plus ; s'il est inclus après, chaque appel `anch::ut::assert(...)` est développé par la macro et ne compile plus (vérifié dans les deux ordres, 2026-10-31). Un header testé qui inclut `<cassert>` suffit.
Piste : renommer (par exemple `check` ou `assertThat`, en gardant `assert` comme alias obsolète), ou au minimum déclarer et appeler la fonction entre parenthèses (`(anch::ut::assert)(...)`) pour empêcher le développement de la macro.
Correction (nom choisi : `assertThat`) :
- `anch::ut::assertThat(bool, std::string_view, const T&...)` remplace `assert` (même signature, même comportement).
- `assert` reste un alias marqué `[[deprecated]]`, déclaré et défini entre parenthèses (`void (assert)(...)`) : le header compile même si `<cassert>` est inclus avant ; un appel `anch::ut::assert(...)` reste développé par la macro si `<cassert>` est inclus (il faut passer à `assertThat`).
- Les 198 appels du dépôt (18 fichiers de test) sont renommés, les lignes de continuation des 22 appels sur plusieurs lignes réalignées ; `CLAUDE.md` et `anch-ut.dox` sont mis à jour. La documentation HTML générée (`docs/`) n'est pas régénérée.
- Test : `anch-ut/test/testUnit.cpp` inclut `<cassert>` avant et après les headers `ut/`, et utilise la macro `assert` et `assertThat` dans le même test.
- Vérifié : `assertThat` compile avec `<cassert>` inclus avant ou après `ut/assert.hpp` ; l'ancien `assert` produit l'avertissement d'obsolescence (une erreur avec `-Werror`) ; build complet sans avertissement et 450 tests passent.

### T8. Message d'assertion vérifié seulement à l'échec, et masqué par l'erreur de format — vérifié
`assert(bool, std::string_view, const T&...)` passe par `std::vformat` : une erreur dans le message (accolades et arguments qui ne correspondent pas) n'est détectée qu'au moment où l'assertion échoue, et l'exception `std::format_error` remplace alors l'échec.
Reproduction (2026-10-31) : `assert(false, "value {} and {}", 1)` affiche « execution failed: format error: invalid arg-id in format string » ; le message et la nature de l'échec (assertion) sont perdus.
Piste : `std::format_string<const T&...>` comme type du message : contrôle à la compilation, et `std::format` à l'échec.

### T9. Pas d'emplacement dans les échecs — vérifié
Les échecs affichent « Unit test X execution failed on assertion: <message> », sans fichier ni ligne : avec plusieurs assertions de même message (boucles, fonctions d'aide), il faut deviner laquelle a échoué.
Piste : `std::source_location` (C++20) capturé par défaut ; pour la version variadique, le passer dans une petite structure construite à partir du message (`struct Message { std::format_string<...> fmt; std::source_location loc = std::source_location::current(); }`).

### T10. Une assertion qui échoue dans un autre thread arrête le processus — vérifié
`AssertException` est levée dans le thread qui appelle `assert` ; hors du thread principal, rien ne l'intercepte : `std::terminate`.
Reproduction (2026-10-31) : un test qui appelle `assert(false, …)` dans un `std::thread` : « Abandon (core dumped) », code 134, sans message d'échec.
Exemple dans le dépôt : `uuid-provider-concurrency` (`anch-uuid/test/testUuid.cpp`) vérifie la version dans des threads : un échec ferait planter l'exécutable au lieu d'être signalé.
Piste : documenter (vérifier après `join`, comme `uuid-v1-v6-unique-threads`) ; fournir une aide qui capture l'exception (`std::exception_ptr`) et la relève dans le thread du test.

### T11. Exception pendant `setup` : arrêt brutal — vérifié
`main` appelle `anch::ut::setup(tests)` hors de tout `try` : un nom de test enregistré deux fois, ou `all`, lève `std::invalid_argument` non interceptée.
Reproduction (2026-10-31) : nom en double : « Abandon (core dumped) », code 134, sans message.
Piste : intercepter dans `main`, afficher le message, sortir avec `EXIT_FAILURE`.

### T12. `uninitialize` appelé même quand `initialize` a échoué — vérifié
`main` appelle toujours `runner->shutdown()`. Si l'initialisation a levé une exception, la fonction de désinitialisation s'exécute sur un état partiellement initialisé (connexion jamais ouverte, fichier absent…).
Reproduction (2026-10-31) : `initialize` qui lève une exception : « Unit tests initialization failed », puis la fonction `uninitialize` est appelée (code de sortie `1`, correct).
Piste : n'appeler `shutdown` que si `startup` a réussi, ou le documenter.

### T13. Tests exécutés dans l'ordre alphabétique, pas dans l'ordre d'enregistrement — vérifié
Les cas sont stockés dans une `std::map` et les noms demandés dans un `std::set` : `-t all` (ou sans `-t`) les exécute par ordre alphabétique. Reproduction : `a-registered-second` passe avant `z-registered-first`.
C'est peu visible avec CTest (un processus par cas) mais trompeur pour les suites dont les cas dépendent les uns des autres (S6 d'`anch-sql`) ou pour lire une sortie `-t all`.
Piste : conserver l'ordre d'enregistrement (`std::vector` + index), et exécuter les `-t` dans l'ordre de la ligne de commande.

### T14. `equalsPtr` écrit sur `std::cout` au lieu du flux demandé — vérifié
`equalsPtr(field, expected, value, out)` écrit le cas « l'un des deux est nul » sur `std::cout` (le paramètre `out` est ignoré), avec un double espace (« p is not null and  is null »). Vérifié : flux passé en paramètre vide, message sur la sortie standard.
Piste : écrire sur `out`.

### T15. `UnitTests::setTests` déclarée mais jamais définie — vérifié
`setTests` est déclarée dans `ut/unit.hpp`, sans définition (`nm -D libanch-unit.so` : aucun symbole) : un appel compile mais ne se lie pas.
Piste : la supprimer ou la définir.

### T16. Codes couleur ANSI hors terminal — vérifié
Les noms de tests et les échecs sont colorés même quand la sortie n'est pas un terminal (CTest, redirection) : les journaux contiennent `^[[34m … ^[[0m^[[K`. Voir aussi `anch-cli` (`Formatter`).
Piste : désactiver les couleurs si `!isatty(STDOUT_FILENO)` ou si `NO_COLOR` est défini.

### T17. Performances — vérifié
- Coût fixe d'un exécutable de test : ~1,6 ms par processus (`-l`), ~2,2 ms pour un cas simple. Avec les 449 `add_test` du dépôt, moins d'une seconde au total : la conception « un processus par cas » coûte peu, sauf si l'initialisation de la suite (`initialize`) est lourde, puisqu'elle est refaite pour chaque cas.
- `equals` sur les conteneurs (`std::vector`, `std::list`, `std::set`) construit un `std::ostringstream` et une chaîne par élément, même quand tout est égal : 94 ms pour comparer deux vecteurs égaux de 1 000 000 d'entiers, alors que `operator==` est quasi instantané.
  Piste : comparer d'abord avec `operator==` (si disponible) et ne construire le nom de l'élément qu'en cas de différence.
- `assertTrue` / `assertFalse` / `fail` prennent le message en `const std::string&` : un message de plus de 15 caractères est alloué à chaque appel, même réussi (~9 ns au lieu de ~2 ns). Négligeable sauf dans des boucles serrées ; `assert` (`std::string_view`) n'a pas ce coût.
- Les arguments de `assert` sont évalués à chaque appel, même quand l'assertion réussit (par exemple `uuid.toString()` dans les boucles des tests `anch-uuid`) : c'est inhérent à une fonction (pas de macro), mais à savoir pour les tests en boucle.

### T18. Les tests d'`anch-ut` ne vérifient que le code de sortie — lecture
`test/testUnit.cpp` exécute un test vide, une exception, un `assertFalse` et un `assert` formaté ; les tests CTest ne vérifient que le code de sortie (`WILL_FAIL`). Aucun ne vérifie les messages, l'ordre d'exécution, les codes de retour ni les comportements de T5 à T14.
Piste : tests de bout en bout qui lancent un exécutable de test et vérifient sa sortie (`PASS_REGULAR_EXPRESSION` / `FAIL_REGULAR_EXPRESSION` de CTest).

## Points positifs
- API fluide (`name`, `description`, `initialize`, `beforeTest`, `afterTest`, `add`), options `-l` et `-t`.
- 6 tests passent.
- Isolation par processus (avec CTest) : un plantage ou une fuite d'état n'affecte que le cas concerné ; un nom de test inconnu passé à `-t` fait échouer l'exécutable.
- `assert` ne formate le message qu'en cas d'échec.

## À approfondir
- [ ] Terminer `anch_add_tests` dans `AnchTest.cmake` en s'appuyant sur `--gen-ctest`.
- [ ] Implémenter `--gen-ctest`, ou retirer les options non implémentées de l'aide.
- [ ] Plugin GCC (`anch-ut-gcc-attr-plugin`) : toujours non analysé.
