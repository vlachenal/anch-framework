# Analyse — anch-cli

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

Périmètre : lecture de `argv` dans `src/cli/args.cpp`, fonctions de liaison de `src/cli/utils.cpp`, activation du formatage (complété le 2026-10-30).

## Observations

- Les options à valeur vérifient la présence d'un argument suivant (`hasMoreArg`) avant `argv[++i]` : pas de lecture hors de `argv` relevée.
- 27 tests passent.
- Les tests `anch-cli-istream-i-test` et `anch-cli-istream--input-test` lisaient `Makefile` dans le répertoire de build, qui n'existe qu'avec le générateur Makefile. Ils lisent désormais `test/CMakeLists.txt` (corrigé le 2026-10-29).

## Constats

### CL1. Valeurs numériques hors limites ou vides ignorées silencieusement — vérifié
`src/cli/utils.cpp:56-59` (`setNum`, utilisé par tous les `bindNum`) : seul `res.ptr != last` est testé après `std::from_chars`. Or, en cas de dépassement, `from_chars` place `ptr` à la fin des chiffres et signale l'erreur uniquement dans `res.ec` ; pour une chaîne vide, `ptr` vaut déjà `last`. Dans les deux cas, aucune exception n'est levée et la variable n'est pas modifiée.
Reproduction (2026-10-30) : avec `uint16_t port = 42`, `bindNum(port)("70000")` et `bindNum(port)("")` laissent `port` à 42 sans erreur ; `"-1"` est bien refusé.
Piste : tester aussi `res.ec != std::errc()`.

### CL2. Fichiers de sortie non vérifiés — lecture
`setOS` (ligne 188) et `setOFS` (ligne 198) créent un `std::ofstream` sans vérifier qu'il est ouvert : une option `-o /chemin/interdit` est acceptée, et les écritures échouent ensuite silencieusement. `setIFS` vérifie bien l'existence du fichier en entrée.

### CL3. Formatage ANSI même hors terminal — lecture
Le formatage n'est désactivé que par `ANCH_CLI_NO_FMT=1` (`manageEnvNoFormat`). Les codes ANSI sont donc écrits aussi quand la sortie est redirigée vers un fichier ou un tube (y compris les logs d'`anch-log` via `termWriter`).
Piste : désactiver automatiquement si `!isatty(fileno(stdout))`, et respecter la convention `NO_COLOR`.

## À approfondir
- [ ] Test de non-régression pour CL1 (valeur hors limites, valeur vide).
- [ ] Analyser `app.hpp` et l'affichage de l'aide.
- [ ] Options par défaut prévues dans `anch-app/anch-app.todo` (`-v`, `-c`, `-d`).
