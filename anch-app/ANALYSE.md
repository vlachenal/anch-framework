# Analyse — anch-app

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

Module débuté (`app.cpp`, `args.cpp`), non suivi par git pour l'instant. Premier passage le 2026-10-30 (lecture) : il compile avec `-DANCH_BUILD_APP=ON`.

## Observations

- Pas de tests.
- Inclus dans le `CMakeLists.txt` racine depuis la migration CMake, mais désactivé par défaut (`ANCH_BUILD_APP=OFF`).
- Feuille de route dans `anch-app.todo` : options par défaut `-v`, `-c`, `-d` dans `anch-cli` ; nom de base et dossier de configuration par défaut dans `anch-conf` ; niveaux de log selon le nombre de `-v`.

## Constats

### A1. `after()` appelé depuis le destructeur : la version dérivée n'est jamais exécutée — lecture
`src/app/app.cpp` : `~Application()` appelle `after()`, déclarée `virtual` (`inc/app/app.hpp:133`). Dans un destructeur, la partie dérivée est déjà détruite : c'est toujours `Application::after()` (vide) qui s'exécute, jamais la redéfinition de la classe fille.
Piste : appeler `after()` à la fin de `run()`, après `execute()` (éventuellement dans un bloc qui garantit l'appel en cas d'exception).

### A2. Premier `-v` : `DEBUG` au lieu d'`INFO` — lecture
`src/app/args.cpp` (`Arguments::verbose`) : si aucun niveau n'est défini, le niveau est d'abord mis à `INFO`, puis le `switch` le fait passer à `DEBUG`. Le premier `-v` donne donc `DEBUG`, alors que `anch-app.todo` prévoit `INFO`, puis `DEBUG`, puis `TRACE`. `quiet()` a le même décalage (premier `-q` : `WARN`), qui correspond peut-être au niveau par défaut d'`anch-log` (`WARN`) : à décider.

### A3. Arguments déclarés mais non pris en compte — lecture
`Application::arg()` / `args()` ignorent leur paramètre, et `run()` supprime le gestionnaire d'arguments sans analyser `argc` / `argv`. Cohérent avec un module en cours d'écriture.

## À approfondir
- [ ] Analyse une fois l'implémentation avancée.
