# Analyse — anch-ut

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre : `main.cpp` (options de ligne de commande), `unit.hpp`, runners (`defaultRunner`, `socketServerRunner`, `socketClientRunner`), vérification des symboles définis dans les headers publics (complété le 2026-10-30).
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

## Points positifs
- API fluide (`name`, `description`, `initialize`, `beforeTest`, `afterTest`, `add`), options `-l` et `-t`.
- 6 tests passent.

## À approfondir
- [ ] Terminer `anch_add_tests` dans `AnchTest.cmake` en s'appuyant sur `--gen-ctest`.
- [ ] Implémenter `--gen-ctest`, ou retirer les options non implémentées de l'aide.
