# Analyse — anch-json

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : `stringLexer.cpp`, `stringMapper.cpp`, `readerContext.cpp`, `integerLexer.cpp`, mappers numériques, `unknownLexer.cpp`, `mappingOptions.hpp`.
Non analysés en détail : mappers génériques, `factory`, `writerContext`, `utils/jsonFormat.cpp`.
Complété le 2026-10-30 (J1 à J3 vérifiés, J6 à J8).

## Constats

### J1. Chaînes non échappées à l'écriture — vérifié
`src/json/stringMapper.cpp:45` : la valeur est écrite telle quelle entre guillemets. Une chaîne contenant `"`, `\` ou un caractère de contrôle (saut de ligne, tabulation, < 0x20) produit du JSON invalide.
Exemple : `a"b` est sérialisé en `"a"b"`.
Reproduction (2026-10-30) : un champ contenant `a"b\c`, un saut de ligne, puis `d` est écrit tel quel ; la relecture de cette sortie échoue (« Unexpected character 'b' »).
Piste : échapper `"`, `\`, `\b \f \n \r \t`, et les autres caractères < 0x20 en `\u00XX`.

### J2. Séquences d'échappement non décodées à la lecture — vérifié
`src/json/stringLexer.cpp:29` (`lexStringContent`) : après `\`, le caractère suivant est ajouté tel quel.
- `\n` devient `n`, `\t` devient `t`.
- `\u00e9` devient `u00e9` (pas de décodage Unicode ni de conversion UTF-8, pas de paires de substitution).
- `\"` et `\\` fonctionnent par chance.
Reproduction (2026-10-30) : `{"s":"x\ny\tz"}` donne `xnytz` ; `{"s":"caf\u00e9"}` donne `cafu00e9`.
Piste : table de décodage, plus le décodage `\uXXXX` vers UTF-8 (avec gestion des paires `\uD8xx\uDCxx`).

### J3. Chaîne non terminée acceptée au niveau racine — vérifié (constat précisé le 2026-10-30)
Si l'entrée se termine avant le `"` fermant, `next()` renvoie `'\0'` (buffer complété par des zéros dans `refillBuffer`). Ce `'\0'` est ajouté à la chaîne, puis la boucle s'arrête sans erreur.
- Dans un objet (`{"s":"abc`), l'erreur est quand même détectée ensuite par l'analyse de l'objet, avec un message peu clair (caractère `'\0'` inattendu).
- Au niveau racine (`deserialize<std::string>` sur `"abc`), la chaîne est acceptée et vaut `abc` suivi d'un octet nul (taille 4).
Piste : lever `INVALID_FORMAT` dans `lexStringContent` quand `'\0'` est atteint.

### J4. `'\0'` dans les données — lecture
`'\0'` sert de marqueur de fin d'entrée : un octet nul légitime dans le flux (invalide en JSON hors échappement, de toute façon) est confondu avec la fin.

### J5. Code mort — lecture
`stringLexer.cpp` contient une version commentée `lexStrContent` avec un `std::cout` de debug. À supprimer ou à finir (elle semble viser l'optimisation par copie de blocs).

### J6. La valeur `0` est refusée pour les entiers et les flottants — vérifié — **corrigé le 2026-10-30**
`src/json/integerLexer.cpp:60` (`lexInteger`) et `:98` (`lexUInteger`) : le cas `'0'` lève `INVALID_FORMAT`, pour interdire les zéros non significatifs (`0123`). Mais cela refuse aussi la valeur `0` elle-même, qui est valide (RFC 8259 : `int = zero / ( digit1-9 *DIGIT )`). Le lexer des flottants a le même problème pour `0` (mais accepte `0.5`).
Conséquence : **la librairie ne relit pas sa propre sortie** dès qu'un champ numérique vaut zéro.
Reproduction (2026-10-30) : `serialize` d'une structure aux entiers nuls produit `{"s":"a","n":0,"u":0}`, et sa relecture lève « Unexpected character '0' » ; `{"d":0}` pour un `double` lève aussi une erreur.
Les tests `lexer-uint-0-ko` / `lexer-int-0-ko` vérifient seulement le refus de `0123456789`.
Correction :
- `integerLexer.cpp` : `0` est traité comme les autres chiffres, puis `checkLeadingZero()` refuse les zéros non significatifs une fois l'entier lu. `0` et `-0` sont acceptés ; `0123` et `-0123` sont refusés (`-0123` était accepté avant).
- `numberLexer.cpp` : un `0` initial peut être suivi de `.`, d'un exposant (`0e2`) ou de la fin du nombre ; seul un chiffre est refusé.
- Tests ajoutés (`testLexer.cpp`) : `lexer-uint-0`, `lexer-int-0`, `lexer-int-minus-0`, `lexer-int-minus-0-ko`, `lexer-double-0`, `lexer-double-0-exp`, `lexer-double-0-ko`.
- Vérifié : l'aller-retour `{"s":"a","n":0,"u":0}` et `{"d":0}` fonctionnent ; 66 tests `json` et `cutils` passent.
- `integerLexerExtended.cpp` (compilé seulement avec `ANCH_JSON_C_INT_PARSER`) n'a pas été modifié : ce code ne compile plus (il référence `val` et `anch::json::Context`, qui n'existent pas).

### J7. Entiers et flottants tronqués silencieusement — vérifié — **corrigé le 2026-10-30**
Les mappers 32 et 16 bits (`int32Mapper.cpp:53`, `int16Mapper.cpp:53`, `uint32Mapper.cpp:53`, `uint16Mapper.cpp:53`) lisent un entier 64 bits puis le convertissent avec `static_cast`, sans contrôle de plage. Même chose pour `floatMapper.cpp:50` (`double` vers `float`).
Reproduction : `{"n":99999999999}` dans un `int32_t` donne `1215752191`, sans erreur.
Cause : le contrôle de plage existait déjà, mais il testait `value` (la variable de destination, qui contient encore l'ancienne valeur) au lieu de `val` (la valeur lue). Il ne pouvait donc jamais échouer.
Correction : `val` dans les conditions des quatre mappers (ligne 50 de `int16Mapper.cpp`, `int32Mapper.cpp`, `uint16Mapper.cpp`, `uint32Mapper.cpp`).
- Tests ajoutés (`testJsonMapper.cpp`, structure `TestRange`) : `json-deser-int16-overflow`, `json-deser-int16-underflow`, `json-deser-int32-overflow`, `json-deser-int32-underflow`, `json-deser-uint16-overflow`, `json-deser-uint32-overflow` (refus avec `POTENTIAL_OVERFLOW`) et `json-deser-range-limits` (bornes acceptées). Vérifié : avec l'ancien `int32Mapper.cpp`, les tests `int32` échouent ; 79 tests `json` et `cutils` passent avec la correction.
- `floatMapper.cpp` (`double` vers `float`) : `1e300` dans un `float` donnait `inf` sans erreur. Une valeur dont la magnitude dépasse `std::numeric_limits<float>::max()` lève maintenant `POTENTIAL_OVERFLOW`. Les valeurs trop petites (`1e-50`) restent acceptées et arrondies vers 0, comme toute perte de précision d'un `float`. Tests ajoutés : `json-deser-float-overflow`, `json-deser-float-negative-overflow`, `json-deser-float-limits` ; 82 tests `json` et `cutils` passent.

### J8. Récursion non bornée : débordement de pile — vérifié
La lecture des tableaux et objets est récursive (`unknownLexer.cpp:99-106` pour les champs ignorés), sans limite de profondeur. Avec `deserialize_ignore_unknown_field = true`, un champ inconnu contenant 200 000 tableaux imbriqués (`[[[...]]]`, 400 Ko) provoque une erreur de segmentation. `max_payload` limite la taille totale mais pas la profondeur.
Piste : option `max_depth` dans `MappingOptions`, avec un compteur de profondeur dans `ReaderContext`.

### J9. Dépassements non détectés : `POTENTIAL_OVERFLOW` n'est jamais levé — vérifié — **corrigé le 2026-10-30**
`src/json/integerLexer.cpp` (`lexInteger`, `lexUInteger`) et `src/json/numberLexer.cpp` (`lexNumber`) ne testent `res.ec` que si `res.ptr != last`. Or, en cas de dépassement, `std::from_chars` place `ptr` à la fin des chiffres (donc `ptr == last`) et signale l'erreur uniquement dans `res.ec` : la fonction renvoie `true` sans modifier la valeur.
Reproduction (2026-10-30) : `99999999999999999999` (entier) et `1e999` (flottant) sont acceptés sans erreur, la variable garde sa valeur précédente (dans les mappers, une variable locale non initialisée).
Correction : dans `lexInteger`, `lexUInteger` et `lexNumber`, `res.ec == result_out_of_range` est testé en premier (`POTENTIAL_OVERFLOW`), puis toute autre erreur ou tout caractère restant (`INVALID_FORMAT`), comme `toNumber` d'`anch-ini`.
- Tests ajoutés : `lexer-uint-overflow` (2^64), `lexer-int-overflow` (2^63), `lexer-int-underflow` (-2^63 - 1), `lexer-double-overflow` (`1e999`), et les valeurs limites acceptées `lexer-uint-max` (2^64 - 1) et `lexer-int-min` (-2^63). 72 tests `json` et `cutils` passent.
- Comportement à connaître : avec libstdc++, un flottant trop petit pour un `double` (`1e-400`) est aussi signalé `result_out_of_range`, donc refusé avec `POTENTIAL_OVERFLOW` (avant : accepté sans modifier la valeur). Les dénormaux représentables (`4.9e-324`) sont acceptés. Si l'on préfère arrondir ces valeurs à 0, il faut traiter ce cas à part.
- J7 (troncature vers les types 32 / 16 bits et `float` dans les mappers) n'est pas concerné par ce correctif : il reste à faire.

## Points positifs
- Lecture en flux avec buffer configurable (`MappingOptions::buffer_size`).
- 48 tests passent, plus un benchmark (`benchJson.cpp`).

## À approfondir
- [ ] Tests aller-retour avec `"`, `\`, `\n`, `\u00e9`, `\uD83D\uDE00`.
- [ ] Test de non-régression pour la profondeur (J8).
- [ ] Analyser les exposants et `-0` dans le lexer des flottants.
