# Analyse — anch-log

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : writers (`writer`, `fileWriter`, `lowPriorityWriter`, `threadSafeWriter`, `termWriter`, `writerRegistry`), `logger`, `loggerFactory`, `loggerConfiguration`, `mdc`, `fmt/messageFormatter`, `fmt/dateFormatter`.
Non analysés en détail : les autres formateurs de `fmt/`.
Complété le 2026-10-30 (L1 et L2 vérifiés, L8 à L11).

## Constats

### L1. `LowPriorityWriter` n'écrit jamais et plante à la destruction — vérifié
- `startTreatment()` (`src/log/lowPriorityWriter.cpp`) n'est appelé nulle part. Le thread de traitement n'est jamais lancé : `write()` empile les messages indéfiniment (croissance mémoire) et rien n'est écrit.
- Le destructeur (ligne 69) fait `_thread->join()` alors que `_thread` vaut `NULL` : plantage.
- Il suffit de configurer `low-priority` pour un writer de fichier (`src/log/writerRegistry.cpp:41`).
- `_running` est un `bool` simple lu et écrit par deux threads (course).
- `process()` sonde toutes les 50 ms au lieu d'utiliser une variable de condition ; les messages restants à l'arrêt ne sont pas vidés.
Reproduction (2026-10-30) : construire un `LowPriorityWriter` à partir d'une section `path=...`, appeler `write()`, le détruire : erreur de segmentation.
Piste : lancer le thread dans le constructeur (ou à la création par le registre), rendre `_running` atomique, vider la file avant `join()`.

### L2. Lecture de `max-size` incorrecte — vérifié
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

### L3. Noms des archives faux dans `rotateFiles` — lecture
`src/log/fileWriter.cpp:132-153` : `ostr` n'est vidé que dans le chemin « renommage ». Après une branche `continue` (fichier absent ou suppression), l'itération suivante concatène : `log.3.2` au lieu de `log.2`.

### L4. Toutes les archives supprimées par défaut ? — lecture, à confirmer
`_maxIdx` vaut `-1` si `max-rotate-files` est absent. La condition `i >= _maxIdx` (ligne 140) est alors toujours vraie : toutes les archives existantes sont supprimées à chaque rotation. À vérifier si c'est la sémantique voulue (« pas d'archive ») ou si -1 devait signifier « illimité ».

### L5. `std::localtime` non sûr entre threads — lecture
`src/log/fmt/dateFormatter.cpp:45` : `std::localtime` renvoie un pointeur vers un tampon statique partagé. Plusieurs threads qui journalisent en même temps peuvent obtenir des dates mélangées.
Piste : `localtime_r`, ou `std::chrono::zoned_time` / `std::format`.

### L6. Gestion manuelle des flux — lecture
`FileWriter` alloue `std::ofstream` avec `new`/`delete`, et l'ouverture n'est pas vérifiée (`_output` peut être en échec sans erreur). `std::unique_ptr` et un test de `is_open()` simplifieraient.

### L7. Tests non parallélisables — **corrigé le 2026-10-29**
Les tests du logger partagent les mêmes fichiers (« has to be launch one by one »). Ils ont désormais la propriété `RESOURCE_LOCK anch-logger-files` : `ctest -j` les exécute l'un après l'autre.

### L8. Motif par défaut `%m` au lieu de `$m` : le message n'est jamais écrit — vérifié
`src/log/writer.cpp:38` : un writer sans `pattern` utilise le motif `"%m"`, alors que la syntaxe du formateur est `$m` (`fmt/messageFormatter.cpp:47`, et `"$d{...} - $m"` dans `loggerFactory.cpp:142`). `%m` est traité comme du texte constant : chaque ligne contient littéralement `%m`.
Reproduction : `MessageFormatter("%m").formatMessage("cat", Level::INFO, "hello")` renvoie `"%m"` ; avec `"$m"`, `"hello"`.
Piste : `_formatter.parserPattern("$m")`.

### L9. `MDC` : conditions inversées dans `add`, `remove`, `reset` et `copy` — vérifié
`src/log/mdc.cpp:44-80` : la documentation indique que ces méthodes s'appliquent à toutes les clés « sauf l'identifiant du thread et les constantes », mais le test `MDC_CONST.find(key) == MDC_CONST.end()` est inversé : seules les clés **constantes** sont traitées, toutes les autres sont ignorées.
Reproduction : `MDC.add("user", "alice")` n'ajoute rien ; après `put("a")`, `put("b")`, `put("c")`, `reset()` laisse les 4 entrées.
De plus :
- `add` est documenté « si la clé n'existe pas » mais appelle `put`, qui écrase ;
- `reset` (ligne 63) fait `_context.erase(iter)` puis `++iter` sur l'itérateur invalidé (comportement indéfini). Non déclenché aujourd'hui à cause de l'inversion, il le sera dès qu'elle sera corrigée.
Piste : `MDC_CONST.contains(key)` pour exclure les constantes ; `iter = _context.erase(iter)` dans `reset`.

### L10. `LoggerFactory::getLogger` n'est pas protégé entre threads — lecture
`src/log/loggerFactory.cpp:95-110` : `getLogger` (statique) lit et insère dans `_loggers` (`std::map`) sans aucun verrou. Deux threads qui demandent pour la première fois un logger en même temps modifient la map simultanément (comportement indéfini).
Piste : `std::mutex` (ou `std::shared_mutex`) autour de la recherche et de l'insertion.

### L11. Liste de writers d'un logger : pas de suppression des espaces — lecture
`parseWriters` (ligne 226) découpe sur `,` sans supprimer les espaces : `writers=console, file` cherche `" file"`, introuvable (message sur la sortie standard, writer ignoré). Une valeur vide (`a,,b`) arrête le découpage : `b` est cherché sous le nom `,b`.

## Points positifs
- Registre de writers extensible (`registerWriter`).
- 6 tests passent.

## À approfondir
- [ ] Tests unitaires de `max-size` (`"100"`, `"1G"`, `"10M"`).
- [ ] Test de rotation avec 3+ archives existantes.
- [ ] Tests de `MDC` (`add`, `remove`, `reset`, `copy`) et d'un writer sans `pattern`.
- [ ] Analyser les autres formateurs de `fmt/`.
