# Analyse — anch-log

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : writers (`writer`, `fileWriter`, `lowPriorityWriter`, `threadSafeWriter`, `termWriter`, `writerRegistry`), `logger`, `loggerFactory`, `loggerConfiguration`, `mdc`, `fmt/messageFormatter`, `fmt/dateFormatter`.
Non analysés en détail : les autres formateurs de `fmt/`.
Complété le 2026-10-30 (L1 et L2 vérifiés, L8 à L11).

## Constats

### L1. `LowPriorityWriter` n'écrit jamais et plante à la destruction — vérifié — **corrigé le 2026-11-02**
- `startTreatment()` (`src/log/lowPriorityWriter.cpp`) n'est appelé nulle part. Le thread de traitement n'est jamais lancé : `write()` empile les messages indéfiniment (croissance mémoire) et rien n'est écrit.
- Le destructeur (ligne 69) fait `_thread->join()` alors que `_thread` vaut `NULL` : plantage.
- Il suffit de configurer `low-priority` pour un writer de fichier (`src/log/writerRegistry.cpp:41`).
- `_running` est un `bool` simple lu et écrit par deux threads (course).
- `process()` sonde toutes les 50 ms au lieu d'utiliser une variable de condition ; les messages restants à l'arrêt ne sont pas vidés.
Reproduction (2026-10-30) : construire un `LowPriorityWriter` à partir d'une section `path=...`, appeler `write()`, le détruire : erreur de segmentation.
Piste : lancer le thread dans le constructeur (ou à la création par le registre), rendre `_running` atomique, vider la file avant `join()`.

### L2. Lecture de `max-size` incorrecte — vérifié — **corrigé le 2026-11-02**
`src/log/fileWriter.cpp:86` : `items.length()` renvoie la longueur de la correspondance entière, pas le nombre de groupes.
- `"1G"` (longueur 2) : facteur 1, donc 1 octet : rotation à chaque ligne.
- `"100"` (longueur 3) : `SIZES[""]` insère 0 dans la map, donc `_maxSize = 0` : rotation désactivée.
- `T` est accepté par la regex (ligne 33) mais absent de `SIZES` : 0.
- 4G ou plus déborde un `uint32_t` (`1024*1024*1024 * num`).
Reproduction (2026-10-30, 30 lignes de 11 octets, `max-rotate-files=3`) :
- `max-size=1K` et `max-size=1G` : une rotation à chaque ligne (archives de 3 octets) ;
- `max-size=100` : aucune rotation (fichier de 330 octets) ;
- `max-size=99` : rotation correcte (archives de 99 octets).
Piste : tester `items[2].matched`, passer à `std::uint64_t`, ajouter `T` ou le retirer de la regex.

### L3. Noms des archives faux dans `rotateFiles` — lecture — **corrigé le 2026-11-02**
`src/log/fileWriter.cpp:132-153` : `ostr` n'est vidé que dans le chemin « renommage ». Après une branche `continue` (fichier absent ou suppression), l'itération suivante concatène : `log.3.2` au lieu de `log.2`.

### L4. Toutes les archives supprimées par défaut ? — lecture, à confirmer — **corrigé le 2026-11-02** (sémantique choisie : N archives, 1 par défaut)
`_maxIdx` vaut `-1` si `max-rotate-files` est absent. La condition `i >= _maxIdx` (ligne 140) est alors toujours vraie : toutes les archives existantes sont supprimées à chaque rotation. À vérifier si c'est la sémantique voulue (« pas d'archive ») ou si -1 devait signifier « illimité ».

### L5. `std::localtime` non sûr entre threads — lecture — **corrigé le 2026-11-02**
`src/log/fmt/dateFormatter.cpp:45` : `std::localtime` renvoie un pointeur vers un tampon statique partagé. Plusieurs threads qui journalisent en même temps peuvent obtenir des dates mélangées.
Piste : `localtime_r`, ou `std::chrono::zoned_time` / `std::format`.

### L6. Gestion manuelle des flux — lecture — **corrigé le 2026-11-02**
`FileWriter` alloue `std::ofstream` avec `new`/`delete`, et l'ouverture n'est pas vérifiée (`_output` peut être en échec sans erreur). `std::unique_ptr` et un test de `is_open()` simplifieraient.

### L7. Tests non parallélisables — **corrigé le 2026-10-29**
Les tests du logger partagent les mêmes fichiers (« has to be launch one by one »). Ils ont désormais la propriété `RESOURCE_LOCK anch-logger-files` : `ctest -j` les exécute l'un après l'autre.

### L8. Motif par défaut `%m` au lieu de `$m` : le message n'est jamais écrit — vérifié — **corrigé le 2026-11-02**
`src/log/writer.cpp:38` : un writer sans `pattern` utilise le motif `"%m"`, alors que la syntaxe du formateur est `$m` (`fmt/messageFormatter.cpp:47`, et `"$d{...} - $m"` dans `loggerFactory.cpp:142`). `%m` est traité comme du texte constant : chaque ligne contient littéralement `%m`.
Reproduction : `MessageFormatter("%m").formatMessage("cat", Level::INFO, "hello")` renvoie `"%m"` ; avec `"$m"`, `"hello"`.
Piste : `_formatter.parserPattern("$m")`.

### L9. `MDC` : conditions inversées dans `add`, `remove`, `reset` et `copy` — vérifié — **corrigé le 2026-11-02**
`src/log/mdc.cpp:44-80` : la documentation indique que ces méthodes s'appliquent à toutes les clés « sauf l'identifiant du thread et les constantes », mais le test `MDC_CONST.find(key) == MDC_CONST.end()` est inversé : seules les clés **constantes** sont traitées, toutes les autres sont ignorées.
Reproduction : `MDC.add("user", "alice")` n'ajoute rien ; après `put("a")`, `put("b")`, `put("c")`, `reset()` laisse les 4 entrées.
De plus :
- `add` est documenté « si la clé n'existe pas » mais appelle `put`, qui écrase ;
- `reset` (ligne 63) fait `_context.erase(iter)` puis `++iter` sur l'itérateur invalidé (comportement indéfini). Non déclenché aujourd'hui à cause de l'inversion, il le sera dès qu'elle sera corrigée.
Piste : `MDC_CONST.contains(key)` pour exclure les constantes ; `iter = _context.erase(iter)` dans `reset`.

### L10. `LoggerFactory::getLogger` n'est pas protégé entre threads — lecture — **corrigé le 2026-11-02** (reproduit : erreur de segmentation avec 8 threads)
`src/log/loggerFactory.cpp:95-110` : `getLogger` (statique) lit et insère dans `_loggers` (`std::map`) sans aucun verrou. Deux threads qui demandent pour la première fois un logger en même temps modifient la map simultanément (comportement indéfini).
Piste : `std::mutex` (ou `std::shared_mutex`) autour de la recherche et de l'insertion.

### L11. Liste de writers d'un logger : pas de suppression des espaces — lecture — **corrigé le 2026-11-02**
`parseWriters` (ligne 226) découpe sur `,` sans supprimer les espaces : `writers=console, file` cherche `" file"`, introuvable (message sur la sortie standard, writer ignoré). Une valeur vide (`a,,b`) arrête le découpage : `b` est cherché sous le nom `,b`.

### L12. Writers d'un logger inversés — vérifié — **corrigé le 2026-11-02**
`initializeLoggers` : un logger qui déclare `writers=...` recevait **tous** les writers, et un logger sans `writers` (ou dont aucun writer n'existe) n'en recevait **aucun** : la condition `loggerWriters.empty()` était inversée.
Correction : writers déclarés s'ils existent, sinon tous les writers (comme le logger par défaut).

### L13. `LowPriorityWriter` : messages sans fin de ligne — lecture — **corrigé le 2026-11-02**
`process()` écrivait `*_output << msg` sans `'\n'` : tous les messages sur une seule ligne (masqué par L1).

### L14. Une erreur d'écriture arrête l'application — lecture — **corrigé le 2026-11-02**
`Logger::log` est `noexcept`, mais `rotateFiles` utilisait les versions de `std::filesystem::rename` / `remove` qui lèvent `filesystem_error` (droits, fichier supprimé...) : `std::terminate`. De même pour toute exception d'un writer (allocation, writer personnalisé).
Correction : rotation avec `std::error_code` (en cas d'échec, le fichier courant est conservé et la rotation retentée plus tard) ; `Logger::log` isole chaque writer (`try` / `catch`).

### L15. Fichiers jamais vidés avant la sortie — lecture — **corrigé le 2026-11-02**
`FileWriter` ne faisait pas de `flush` : les derniers messages, ceux qui expliquent un plantage, étaient perdus en cas d'arrêt brutal. `FileWriter` vide le tampon après chaque message, `LowPriorityWriter` après chaque lot.

### L16. Formateurs — lecture — **corrigé le 2026-11-02**
- documentation de `MessageFormatter` en `%c`, `%m`... alors que la syntaxe est `$c`, `$m` (origine probable de L8) ; `$C{clé}` (MDC) non documenté ;
- un `$` qui ne commence pas un motif (`prix $5`) arrêtait l'analyse du motif : la suite était ignorée sans erreur. Il est maintenant écrit tel quel ;
- `MessageFormatter` possède ses formateurs (pointeurs bruts) mais était copiable : double libération. Copie supprimée ;
- `MDCFormatter` copiait tout le contexte à chaque message ;
- `DateFormatter` : `std::localtime` (L5) remplacé par `localtime_r`, et le tampon de `strftime` n'était pas défini quand le format est trop long.

## Corrections du 2026-11-02 : détails
- `FileWriter` : `max-size` lu avec `std::from_chars` en `uint64_t`, unités `K`, `M`, `G`, `T` (puissances de 1024), dépassement et unités invalides refusés (L2) ; `max-rotate-files=N` garde N archives (`.0` la plus récente), 1 par défaut, 0 = fichier tronqué (L3, L4) ; flux `std::ofstream` par valeur, ouverture vérifiée (L6) ; un chemin sans répertoire (`test.log`) est accepté.
- `LowPriorityWriter` : thread démarré par le constructeur (`std::jthread`), variable de condition au lieu d'une attente active, messages en file écrits avant la destruction (L1).
- `ThreadSafeWriter` : `std::lock_guard` (une exception laissait le mutex verrouillé).
- `MDC` : `add`, `remove`, `reset` et `copy` excluent l'identifiant du thread et les constantes ; `add` n'écrase pas ; `reset` n'utilise plus d'itérateur invalidé (L9).
- `LoggerFactory` : mutex dans `getLogger` (L10) ; liste de writers sans espaces ni entrées vides (L11) ; L12.
- Tests : nouvel exécutable `anch-log-writers-test` (10 cas : motif par défaut, formateurs, `max-size`, rotation, archives, `LowPriorityWriter` multi-thread, `MDC`) et 2 cas de `LoggerFactory` (writers par logger, `getLogger` concurrent). Sur l'ancien code, 11 de ces 12 nouveaux cas échouent ou plantent (seul `mdc-formatter` passe). Sans signalement sous TSan / ASan.

## Points positifs
- Registre de writers extensible (`registerWriter`).
- 18 tests (2026-11-02).

## À approfondir
- [x] Tests unitaires de `max-size` (`"100"`, `"1G"`, `"10M"`) (2026-11-02).
- [x] Test de rotation avec 3+ archives existantes (2026-11-02).
- [x] Tests de `MDC` (`add`, `remove`, `reset`, `copy`) et d'un writer sans `pattern` (2026-11-02).
- [x] Analyser les autres formateurs de `fmt/` (2026-11-02).
