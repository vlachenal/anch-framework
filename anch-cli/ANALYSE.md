# Analyse — anch-cli

Analyse du 2026-10-27, complétée et corrigée le 2026-11-03. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

Périmètre : lecture de `argv` dans `src/cli/args.cpp`, fonctions de liaison de `src/cli/utils.cpp`, activation du formatage (complété le 2026-10-30) ; le 2026-11-03 : tout `args.cpp` (enregistrement, analyse, aide, bannière), `formatter.cpp`, `app.hpp`, `arg.hpp` et les tests.
Vérifications du 2026-11-03 : programme de test (`probe`) lié à `libanch-cli`, compilation d'un appel depuis un autre namespace.

## Observations

- Les options à valeur vérifient la présence d'un argument suivant (`hasMoreArg`) avant `argv[++i]` : pas de lecture hors de `argv` relevée.
- 27 tests passaient ; 34 depuis le 2026-11-03 (7 tests anch-ut ajoutés dans `anch-cli-utils-test`).
- Les tests `anch-cli-istream-i-test` et `anch-cli-istream--input-test` lisaient `Makefile` dans le répertoire de build, qui n'existe qu'avec le générateur Makefile. Ils lisent désormais `test/CMakeLists.txt` (corrigé le 2026-10-29).

## Constats

### CL1. Valeurs numériques hors limites ou vides ignorées silencieusement — vérifié — **corrigé le 2026-11-03**
`src/cli/utils.cpp:56-59` (`setNum`, utilisé par tous les `bindNum`) : seul `res.ptr != last` est testé après `std::from_chars`. Or, en cas de dépassement, `from_chars` place `ptr` à la fin des chiffres et signale l'erreur uniquement dans `res.ec` ; pour une chaîne vide, `ptr` vaut déjà `last`. Dans les deux cas, aucune exception n'est levée et la variable n'est pas modifiée.
Reproduction (2026-10-30) : avec `uint16_t port = 42`, `bindNum(port)("70000")` et `bindNum(port)("")` laissent `port` à 42 sans erreur ; `"-1"` est bien refusé.
Piste : tester aussi `res.ec != std::errc()`.
Correction : `setNum` analyse dans une variable locale, refuse `result_out_of_range` (« Value out of range: '70000' ») et toute autre erreur ou chaîne vide (« Invalid value: '' »), et ne modifie la destination qu'en cas de succès. `ArgHandler::handle` affiche le message et sort avec le code `1`. Test : `anch-cli-utils-numbers` (`uint16_t`, `int16_t`, `double`).

### CL2. Fichiers de sortie non vérifiés — vérifié — **corrigé le 2026-11-03**
`setOS` (ligne 188) et `setOFS` (ligne 198) créent un `std::ofstream` sans vérifier qu'il est ouvert : une option `-o /chemin/interdit` est acceptée, et les écritures échouent ensuite silencieusement. `setIFS` vérifie bien l'existence du fichier en entrée, mais pas qu'il a pu être ouvert (droits).
Correction : `setOS` et `setOFS` passent par `openOFS`, qui lève `std::invalid_argument` (« File … can not be opened for writing ») si le fichier n'est pas ouvert ; `setIFS` vérifie aussi l'ouverture (« File … can not be read »). Test : `anch-cli-utils-output-file` (répertoire inexistant, avec et sans mode).

### CL3. Formatage ANSI même hors terminal — vérifié — **corrigé le 2026-11-03**
Le formatage n'est désactivé que par `ANCH_CLI_NO_FMT=1` (`manageEnvNoFormat`). Les codes ANSI sont donc écrits aussi quand la sortie est redirigée vers un fichier ou un tube (y compris les logs d'`anch-log` via `termWriter`).
Piste : désactiver automatiquement si `!isatty(fileno(stdout))`, et respecter la convention `NO_COLOR`.
Correction (`src/cli/formatter.cpp`) : la valeur initiale de `Formatter::DISABLED` est calculée au chargement de la bibliothèque : `true` si la sortie standard n'est pas un terminal, si `NO_COLOR` est défini et non vide, ou si `ANCH_CLI_NO_FMT=1`. Cela couvre aussi les programmes qui n'utilisent pas `ArgHandler` (`termWriter` d'`anch-log`). `--no-term-fmt` et l'affectation de `DISABLED` restent possibles (une application peut forcer les couleurs avec `DISABLED = false`). Le correctif équivalent d'`anch-ut` (T16) est retiré de `anch-ut/src/ut/main.cpp`, devenu redondant.
Limite : seule la sortie standard est testée ; l'aide écrite sur `std::cerr` suit l'état de `stdout`. Test : `anch-cli-utils-formatter-no-terminal`.

### CL4. `operator<<(std::ostream&, const Formatter&)` introuvable depuis un namespace qui déclare son propre `operator<<` — vérifié — **corrigé le 2026-11-03**
L'opérateur était déclaré dans l'espace de noms global : la recherche dépendante des arguments (ADL) ne regarde que `std` et `anch::cli`, et la recherche ordinaire s'arrête au premier `operator<<` trouvé dans les namespaces englobants. Reproduction : `namespace foo { struct X {}; std::ostream& operator<<(std::ostream&, const X&); void f() { std::cout << anch::cli::RESET; } }` ne compile pas (« no match for operator<< »).
Correction : opérateur déclaré et défini dans `anch::cli`. Test : `anch-cli-utils-formatter-no-terminal` (appel depuis un namespace `other` qui définit son `operator<<`).

### CL5. Une option `--no-term-fmt` déclarée par l'application devient un argument positionnel obligatoire — vérifié — **corrigé le 2026-11-03**
`build()` testait `_lopts.contains("no-term-format")` (nom jamais enregistré) puis, si `no-term-fmt` existait déjà, enregistrait une option sans `sopt` ni `lopt` : elle devenait positionnelle et obligatoire. Reproduction : application qui déclare `--no-term-fmt`, lancée sans argument : « Missing arguments: ARG », code `2`. Même effet au second appel de `handle()`.
Correction : l'option n'est enregistrée que si `--no-term-fmt` n'est pas déjà déclarée. Test : `anch-cli-utils-user-no-term-fmt`.

### CL6. Un positionnel multiple qui n'est pas le dernier n'est jamais rempli — vérifié — **corrigé le 2026-11-03**
Le message de `build()` (« Multiple positional must be defined at last position ») n'était émis que pour un **second** positionnel multiple ; un positionnel multiple suivi d'un positionnel simple était accepté, puis `parse` lui donnait une seule valeur et refusait les suivantes. Reproduction : `[M+ (multi), LAST]` avec `a b c` : « Too many positional arguments ».
Correction : vérifié à l'enregistrement (`arg()`, donc dès le constructeur) : un positionnel ajouté après un positionnel multiple lève `std::invalid_argument`. La vérification de `build()` est retirée. Test : `anch-cli-utils-multi-positional-last`.

### CL7. Options sans valeur répétées : `-v -v` accepté, `--verbose --verbose` refusé — vérifié — **corrigé le 2026-11-03**
La forme courte ne refusait la répétition que pour les options à valeur ; la forme longue la refusait pour toutes.
Correction (choix : comportement de la forme courte, qui permet un compteur de verbosité `-v -v`) : seules les options à valeur non `multi` répétées sont refusées, sous les deux formes. Tests : `anch-cli-utils-repeated-flag`, `anch-cli-utils-repeated-value`.

### CL8. Codes ANSI dans les messages d'exception — vérifié — **corrigé le 2026-11-03**
« Argument … has already been found » utilisait `formatArg` (soulignement) : le texte de l'exception contenait des codes ANSI, quel que soit l'endroit où il est affiché ou journalisé.
Correction : `plainArg` (`-p|--plop`) pour les messages. Test : `anch-cli-utils-repeated-value` vérifie le message exact.

### CL9. Bannière illisible ignorée silencieusement — lecture — **corrigé le 2026-11-03**
`printBanner` comptait sur une exception (`catch(...)`) que `std::ifstream` ne lève pas : un fichier absent n'affichait rien. Correction : « Can not read <chemin> » si le fichier n'est pas ouvert ; un fichier vide n'affiche rien (le `failbit` ne concerne que le flux d'entrée).

### CL10. Débordement de tas d'un octet dans `testCli.cpp` — lecture — **corrigé le 2026-11-03**
`main` copiait chaque argument dans `new char[strlen(argv[i])]` puis `strcpy` : le `'\0'` final était écrit hors du tampon (et `args` n'était pas terminé par `NULL`). Correction : les pointeurs de `argv` sont réutilisés dans un `std::vector<char*>` terminé par `NULL`.

### CL11. `--` bascule l'analyse des options au lieu de la terminer — lecture — **corrigé le 2026-11-21**
`parse` inverse `ignoreOpt` à chaque `--` (« trigger ignore option status ») : un second `--` réactive les options et n'est jamais transmis. La convention POSIX (`getopt`) termine l'analyse des options au premier `--` ; les `--` suivants sont des arguments positionnels. Impossible donc de passer `--` comme valeur positionnelle.
Piste : terminer l'analyse au premier `--`, si la bascule n'est pas voulue.
Correction (décidée le 2026-11-21 : convention POSIX) : seul le premier `--` est consommé, les arguments suivants (`--` compris) sont positionnels. Documenté dans `anch-cli.dox`. Test `anch-cli-utils-end-of-options` (`a -- -v -- --verbose` : option non lue, 4 positionnels).

### CL12. Formes `--option=valeur` et `-pvaleur` non prises en charge — lecture — conforme (choix)
`--plop=x` donne « Unknown argument --plop=x » ; `-px` est lu comme les options `-p` et `-x`. Les deux formes sont courantes (`getopt_long`).
Confirmé le 2026-11-03 : non prises en charge par choix ; à réexaminer éventuellement plus tard.

### CL13. Option `pipe` absente : lecture de `std::cin` même sur un terminal — lecture — conforme (voulu)
`check()` appelle le gestionnaire `pipe` avec `std::cin` dès que l'option n'est pas fournie ; si l'entrée standard est un terminal, le programme attend une saisie.
Confirmé le 2026-11-03 : comportement voulu, comme `cat` (lecture de l'entrée standard quand aucun fichier n'est donné). Documenté dans `Arg::pipe` (`inc/cli/arg.hpp`).
Remarque liée : les arguments positionnels sont toujours rendus obligatoires (`arg()` force `mandatory`) : pas de positionnel optionnel.

## À approfondir
- [x] Test de non-régression pour CL1 (valeur hors limites, valeur vide) : `anch-cli-utils-numbers`.
- [x] Analyser `app.hpp` et l'affichage de l'aide (2026-11-03 : `app.hpp` est une simple structure ; voir CL5, CL8, CL9).
- [ ] Options par défaut prévues dans `anch-app/anch-app.todo` (`-v`, `-c`, `-d`).
- [x] Décider CL11 (`--`) : convention POSIX (2026-11-21) ; CL12 (`--opt=val`, choix pouvant être réexaminé) et CL13 (`pipe` sur un terminal) sont voulus.
- [ ] `testCli.cpp` ne vérifie que des codes de sortie (avec son propre `main`) : à migrer vers anch-ut.
