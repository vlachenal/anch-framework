# Analyse — anch-json

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : `stringLexer.cpp`, `stringMapper.cpp`, `readerContext.cpp`, `integerLexer.cpp`, mappers numériques, `unknownLexer.cpp`, `mappingOptions.hpp`.
Non analysés en détail : mappers génériques, `factory`, `writerContext`, `utils/jsonFormat.cpp`.
Complété le 2026-10-30 (J1 à J3 vérifiés, J6 à J8).

## Constats

### J1. Chaînes non échappées à l'écriture — vérifié — **corrigé le 2026-10-31**
`src/json/stringMapper.cpp:45` : la valeur est écrite telle quelle entre guillemets. Une chaîne contenant `"`, `\` ou un caractère de contrôle (saut de ligne, tabulation, < 0x20) produit du JSON invalide.
Exemple : `a"b` est sérialisé en `"a"b"`.
Reproduction (2026-10-30) : un champ contenant `a"b\c`, un saut de ligne, puis `d` est écrit tel quel ; la relecture de cette sortie échoue (« Unexpected character 'b' »).
Piste : échapper `"`, `\`, `\b \f \n \r \t`, et les autres caractères < 0x20 en `\u00XX`.
Correction :
- `WriterContext::writeString` (guillemets compris) et `writeEscaped` (contenu seul) échappent `"`, `\`, `\b \f \n \r \t` et les autres caractères < 0x20 en `\u00XX`. Les autres caractères (UTF-8 compris) et `/` sont écrits tels quels. Copie par blocs entre les caractères à échapper (table de 256 booléens).
- Utilisé pour les valeurs `std::string` et `std::string_view`, et pour les noms de champs (`writeField`), donc aussi pour les clés des `std::map`.
- `writeRawField` écrit un nom de champ sans l'échapper : `utils/jsonFormat.cpp` l'utilise, car ses noms de champs contiennent des codes couleur ANSI (caractère ESC, < 0x20). Le formateur échappe lui-même le texte avant d'ajouter les couleurs (noms de champs et valeurs).
- Tests : `json-ser-escape` (sortie exacte et aller-retour) et `json-ser-escape-map-key`. Vérifié aussi : sortie de `anch-json-formatter` valide (relue par Python), sortie de `data_set.json` inchangée, pas de ralentissement mesuré (~395 µs).

### J2. Séquences d'échappement non décodées à la lecture — vérifié — **corrigé le 2026-10-31**
`src/json/stringLexer.cpp:29` (`lexStringContent`) : après `\`, le caractère suivant est ajouté tel quel.
- `\n` devient `n`, `\t` devient `t`.
- `\u00e9` devient `u00e9` (pas de décodage Unicode ni de conversion UTF-8, pas de paires de substitution).
- `\"` et `\\` fonctionnent par chance.
Reproduction (2026-10-30) : `{"s":"x\ny\tz"}` donne `xnytz` ; `{"s":"caf\u00e9"}` donne `cafu00e9`.
Piste : table de décodage, plus le décodage `\uXXXX` vers UTF-8 (avec gestion des paires `\uD8xx\uDCxx`).
Correction (avec P6) : `lexStringContent` décode `\" \\ \/ \b \f \n \r \t` et `\uXXXX` vers UTF-8, paires de substitution comprises. Une séquence inconnue (`\x`), un chiffre hexadécimal invalide ou une substitution isolée lèvent `INVALID_FORMAT` (avant : `\x` donnait `x`). Tests : `lexer-str-escapes`, `lexer-str-unicode`, `lexer-str-surrogate`, `lexer-str-escape-ko`.
- Les chaînes ignorées (champs inconnus, `lexUnknownStr`) ne sont pas décodées, seulement parcourues.

### J3. Chaîne non terminée acceptée au niveau racine — vérifié (constat précisé le 2026-10-30) — **corrigé le 2026-10-31**
Si l'entrée se termine avant le `"` fermant, `next()` renvoie `'\0'` (buffer complété par des zéros dans `refillBuffer`). Ce `'\0'` est ajouté à la chaîne, puis la boucle s'arrête sans erreur.
- Dans un objet (`{"s":"abc`), l'erreur est quand même détectée ensuite par l'analyse de l'objet, avec un message peu clair (caractère `'\0'` inattendu).
- Au niveau racine (`deserialize<std::string>` sur `"abc`), la chaîne est acceptée et vaut `abc` suivi d'un octet nul (taille 4).
Piste : lever `INVALID_FORMAT` dans `lexStringContent` quand `'\0'` est atteint.
Correction (avec P6) : `lexStringContent` lève `INVALID_FORMAT` quand la fin des données arrive avant le `"` fermant. Test : `lexer-str-unterminated-ko`. `lexUnknownStr` (champs ignorés) s'arrête toujours sans erreur, l'erreur venant ensuite de l'analyse de l'objet.

### J4. `'\0'` dans les données — lecture
`'\0'` sert de marqueur de fin d'entrée : un octet nul légitime dans le flux (invalide en JSON hors échappement, de toute façon) est confondu avec la fin.

### J5. Code mort — lecture — **corrigé le 2026-10-31**
`stringLexer.cpp` contient une version commentée `lexStrContent` avec un `std::cout` de debug. À supprimer ou à finir (elle semble viser l'optimisation par copie de blocs).
Correction : supprimée lors de la réécriture du lexer de chaînes (P6).

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

### J8. Récursion non bornée : débordement de pile — vérifié — **corrigé le 2026-10-31**
La lecture des tableaux et objets est récursive (`unknownLexer.cpp:99-106` pour les champs ignorés), sans limite de profondeur. Avec `deserialize_ignore_unknown_field = true`, un champ inconnu contenant 200 000 tableaux imbriqués (`[[[...]]]`, 400 Ko) provoque une erreur de segmentation. `max_payload` limite la taille totale (depuis J10) mais pas la profondeur.
Piste : option `max_depth` dans `MappingOptions`, avec un compteur de profondeur dans `ReaderContext`.
Reproduction (2026-10-31) : `anch-json-formatter` sur 200 000 tableaux imbriqués s'arrête sur une erreur de segmentation (code 139).
Correction :
- Option `MappingOptions::max_depth` (défaut `512`, `0` = illimité) : profondeur maximale d'imbrication des objets et tableaux, la racine étant le niveau 1. Au-delà, `POTENTIAL_OVERFLOW`.
- Compteur `ReaderContext::depth`, `enterLevel()` / `leaveLevel()` et garde RAII `DepthGuard`, utilisée à chaque entrée dans un objet ou un tableau : `lexObject`, `lexArray`, `lexMap`, `lexUnknownArray`, `lexUnknownObject`, `Reader::parseObject`, `Reader::parseArray`.
- Le défaut n'est pas « illimité », pour que l'entrée d'origine ne plante plus par défaut. Avec une pile de thread réduite, il peut être nécessaire de baisser `max_depth`.
- Tests : `json-deser-max-depth-unknown` (200 000 tableaux dans un champ ignoré : erreur au lieu du plantage), `json-deser-max-depth-unknown-limit` (limite exacte, champ ignoré), `json-deser-max-depth-mapped` (type récursif mappé : limite exacte, dépassement, `0` illimité).
- Vérifié aussi : `anch-json-formatter` accepte 512 niveaux, signale une erreur à 513 et à 200 000 (plus de plantage). Pas de coût mesurable sur le benchmark.
- `lexMap` n'a pas de test dédié (même garde que `lexArray`).

### J9. Dépassements non détectés : `POTENTIAL_OVERFLOW` n'est jamais levé — vérifié — **corrigé le 2026-10-30**
`src/json/integerLexer.cpp` (`lexInteger`, `lexUInteger`) et `src/json/numberLexer.cpp` (`lexNumber`) ne testent `res.ec` que si `res.ptr != last`. Or, en cas de dépassement, `std::from_chars` place `ptr` à la fin des chiffres (donc `ptr == last`) et signale l'erreur uniquement dans `res.ec` : la fonction renvoie `true` sans modifier la valeur.
Reproduction (2026-10-30) : `99999999999999999999` (entier) et `1e999` (flottant) sont acceptés sans erreur, la variable garde sa valeur précédente (dans les mappers, une variable locale non initialisée).
Correction : dans `lexInteger`, `lexUInteger` et `lexNumber`, `res.ec == result_out_of_range` est testé en premier (`POTENTIAL_OVERFLOW`), puis toute autre erreur ou tout caractère restant (`INVALID_FORMAT`), comme `toNumber` d'`anch-ini`.
- Tests ajoutés : `lexer-uint-overflow` (2^64), `lexer-int-overflow` (2^63), `lexer-int-underflow` (-2^63 - 1), `lexer-double-overflow` (`1e999`), et les valeurs limites acceptées `lexer-uint-max` (2^64 - 1) et `lexer-int-min` (-2^63). 72 tests `json` et `cutils` passent.
- Comportement à connaître : avec libstdc++, un flottant trop petit pour un `double` (`1e-400`) est aussi signalé `result_out_of_range`, donc refusé avec `POTENTIAL_OVERFLOW` (avant : accepté sans modifier la valeur). Les dénormaux représentables (`4.9e-324`) sont acceptés. Si l'on préfère arrondir ces valeurs à 0, il faut traiter ce cas à part.
- J7 (troncature vers les types 32 / 16 bits et `float` dans les mappers) n'est pas concerné par ce correctif : il reste à faire.

### J15. Données après la valeur racine acceptées — vérifié (2026-11-14)
`{"s":"a"} x`, `{"s":"a"}}` et `{"s":"a"}{"s":"b"}` sont acceptés : la lecture s'arrête à la fin de la première valeur sans regarder la suite. Côté serveur (`anch-rest`), un corps suivi de n'importe quoi est donc valide.
Piste : après la valeur racine, n'accepter que des espaces jusqu'à la fin du flux (option si la lecture de plusieurs documents sur un même flux est voulue).

### J16. Virgule finale acceptée dans un objet mappé — vérifié (2026-11-14)
`{"s":"a",}` est accepté, alors que `[1,]` et `{"a":1,}` dans une `std::map` sont refusés.

### J17. Caractères de contrôle bruts acceptés dans les chaînes — vérifié (2026-11-14)
Une tabulation, un saut de ligne ou `0x1b` non échappés dans une chaîne sont acceptés (la RFC 8259 impose l'échappement des caractères < 0x20). Sans gravité : ils sont échappés correctement à la réécriture.

### J18. UTF-8 invalide accepté et réécrit tel quel — vérifié (2026-11-14)
`"\xff\xfe"` ou une séquence tronquée (`"\xc3"`) sont acceptés puis écrits sans changement : la sortie n'est plus de l'UTF-8 valide. Les contraintes `minLength` / `maxLength` comptent alors des points de code sur une séquence invalide.
Piste : option de validation UTF-8 à la lecture (coût à mesurer), au moins côté serveur.

### J19. `deserialize_max_field_char` n'est jamais appliqué — vérifié (2026-11-14)
L'option (défaut `512`, commentaire `\todo keep it ?`) n'est lue nulle part : une chaîne de 2 000 000 de caractères est acceptée avec la valeur par défaut. Comme `max_payload` vaut `0` (illimité) par défaut, et que le codec `Json` d'`anch-rest` copie ces valeurs par défaut, la taille d'un corps JSON n'est bornée que par le serveur HTTP (`client_max_body_size` de nginx).
Piste : supprimer l'option (une contrainte `maxLength` par champ et `max_payload` couvrent le besoin) ou l'appliquer ; donner une valeur par défaut non nulle à `max_payload` côté serveur dans `anch::rest::Json::options(Side::SERVER)`.

### Fuzz du 2026-11-14 — aucun défaut
2,7 millions de lectures de documents mutés (structure avec chaînes, entiers, flottants, `optional`, `vector`, `set`, `list`, `map`, objets imbriqués, UUID), sous ASan / UBSan, avec et sans champs ignorés, avec `buffer_size` de 1, 2, 3, 5, 8, 17, 64 et 1024 : aucun plantage, aucun blocage, et le résultat (succès ou erreur, valeur relue) est identique quelle que soit la taille du tampon.

## Performances

Mesures : `anch-json-bench` sur `test/data_set.json` (1001 personnes, 311 Ko compacts), preset `release` (avec `ANCH_HARDENING` et `ANCH_NATIVE_ARCH`), moyenne de 10 000 itérations, 3 exécutions.

### P1. Table des lecteurs reconstruite pour chaque objet lu — **corrigé le 2026-10-31**
`ObjectMapper<T>::deserializeValue` construisait, pour chaque objet JSON, une `std::map<std::string, LexFunc>` (copie de chaque clé, `std::bind_front` dans une `std::function`, un nœud par champ) et un `std::set<std::string>` des champs trouvés, jamais relu.
Correction :
- `lexObject` devient un template (`inc/json/impl/lexer.hpp`) qui appelle un gestionnaire `void(const std::string& field, ReaderContext&)` pour chaque champ.
- `deserializeValue` cherche directement le champ dans `_readers` (construit une seule fois par type) et appelle le lecteur avec l'objet en argument. Le `std::set` est supprimé, ainsi que l'alias `LexFunc` (devenu inutile).
- Résultat : lecture de 2565-2630 µs à 1646-1667 µs (**-36 %**) ; callgrind : -34 % d'instructions sur la lecture. 71 tests `json` passent.
- L'écriture affichait 817 µs avant et ~970 µs après, alors que son code n'a pas changé : callgrind compte exactement le même nombre d'instructions des deux côtés. L'écart venait de l'état du tas ou du cache dans la boucle du benchmark (la sérialisation suit la lecture de l'itération précédente) : il disparaît après P2/P3 (~817 µs).

### P2. `ReaderContext::next()` non inline — **corrigé le 2026-10-31**
`next()`, appelée pour chaque caractère lu, était définie dans `readerContext.cpp`. Elle est maintenant `inline` dans `impl/readerContext.hpp`, avec le remplissage du buffer marqué `[[unlikely]]`.

### P3. `std::function` sur le chemin critique de la lecture — **corrigé le 2026-10-31**
- `discard()` et `parseUnknown()` passaient par des `std::function` (`_discardFunc`, `_unknownFunc`), supprimées : un test sur l'option suffit. `discard()` a un chemin rapide inline (caractère suivant non blanc) ; sinon elle délègue à `lexDiscard` ou `discardFail` comme avant.
- `lexArray` et `lexMap` deviennent des templates (`impl/lexer.hpp`) : les lambdas de `GenericMapper` ne sont plus emballées dans des `std::function`. Les alias `AddItem` / `PushItem` restent utilisables.
- Résultat (P1 à P3) : lecture de ~2580 µs à ~945 µs (**-63 %**, soit 2,7 fois plus rapide) ; écriture inchangée (~817 µs). 71 tests `json` passent.
- Restent pour l'`ObjectMapper` : `_readers` et `_writers` en `std::function` (un appel indirect par champ).

### P4. Écriture caractère par caractère via `std::ostream` — **corrigé le 2026-10-31**
Chaque `put()` / `write()` sur `std::ostream` construit une `sentry` ; les nombres passaient par `operator<<` (locale, facettes).
Correction :
- `WriterContext::write(char)`, `write(std::string_view)` et `writeNumber<T>()` (`inc/json/impl/writerContext.hpp`) écrivent directement dans le `std::streambuf` du flux ; en cas d'échec, `badbit` est positionné sur le flux. `output` reste public : du code qui écrit encore dans `output` (comme `utils/jsonFormat.cpp`) garde l'ordre des écritures, puisque tout passe par le même `streambuf`.
- Les nombres sont écrits avec `std::to_chars`. Pour les flottants, c'est l'écriture la plus courte relisible sans perte : `1234567.89` était écrit `1.23457e+06` (6 chiffres significatifs, perte de précision), il est maintenant écrit `1234567.89`. La précision configurée sur le flux (`precision()`) n'est plus prise en compte.
- Vérifié : sortie identique octet par octet à l'ancienne version sur `data_set.json` ; 74 tests `json` passent.
- Résultat : écriture de ~818 µs à ~440 µs (**-46 %**).
- `anch::UUID::toString()` allouait à chaque écriture : corrigé le 2026-10-31 (`UUID::toChars`, voir UU4 dans `anch-uuid/ANALYSE.md`) ; écriture de 100 000 UUID de ~26,5 ms à ~3,7 ms.

### P5. Indentation par `std::function`, copie du résultat, appels hors ligne — **corrigé le 2026-10-31**
- `_indent` (une `std::function` appelée à chaque champ, même sans indentation) est remplacée par un test sur `prettify_nbs`.
- `serialize()` vers `std::string` (`impl/jsonSerialize.hpp`) renvoie `std::move(out).str()` au lieu de copier la chaîne de l'`ostringstream` ; le `WriterContext` est détruit avant.
- `next()`, `writeField()`, `beginObject()`, `endObject()`, `beginArray()` et `endArray()` sont `inline` dans `impl/writerContext.hpp` (seule `indent()` reste dans le `.cpp`). Ces symboles ne sont plus exportés par `libanch-json.so` : un binaire compilé avec les anciens en-têtes doit être recompilé.
- Mesures séparées : la suppression de `_indent` seule exécute moins d'instructions (callgrind) mais était ~5 % plus lente (placement du code) ; la copie évitée seule gagne 1-2 %. Avec le passage en `inline`, l'ensemble gagne 5-6 %.
- Résultat : écriture vers un flux de ~420 µs à ~399 µs ; vers une `std::string` de ~426 µs à ~400 µs. Sorties compacte et indentée (`prettify_nbs = 2`) identiques octet par octet à la version d'origine sur `data_set.json` ; 74 tests `json` passent.
- Aucun test ne couvre l'indentation : à ajouter.

### P6. Chaînes copiées caractère par caractère, puis recopiées — **corrigé le 2026-10-31**
`lexStringContent` ajoutait chaque caractère à `pbuf` (`pbuf += current`), puis `lexString` et `lexField` copiaient `pbuf` dans la valeur.
Correction (`src/json/stringLexer.cpp`) :
- Les caractères sont copiés par blocs jusqu'au prochain `"`, `\` ou `'\0'` (table de 256 booléens). `buffer[buffer_size]` vaut toujours `'\0'` et sert de sentinelle : pas de test de fin de buffer dans la boucle.
- Nouvelle surcharge `lexStringContent(std::string& value, ReaderContext&)` : `lexString` et `lexField` écrivent directement dans la valeur. L'ancienne `lexStringContent(ReaderContext&)` (utilisée par `Reader`) écrit dans `pbuf`.
- En cas d'erreur, la valeur peut être partiellement remplie (avant : inchangée).
- Test `lexer-str-buffer` : chaîne avec échappements, `\u` et paire de substitution, buffers de 1 à 12 octets.
- Résultat : lecture de ~925 µs à ~710 µs (**-23 %**).

### P7. Nombres copiés caractère par caractère — **corrigé le 2026-10-31**
`lexInteger`, `lexUInteger` et `lexNumber` copiaient chaque chiffre dans `pbuf` avant `std::from_chars`.
Correction :
- `scanNumber` (`inc/json/numberScanner.hpp`, implémentation dans `inc/json/impl/numberScanner.hpp`) repère les caractères du nombre directement dans le buffer ; `std::from_chars` lit le buffer sans copie. `pbuf` ne sert que si le nombre est coupé en fin de buffer.
- Validation stricte selon RFC 8259 sur les caractères repérés : `-? (0 | [1-9][0-9]*) (.[0-9]+)? ([eE][+-]?[0-9]+)?` (entiers : sans fraction ni exposant). Changements : `.5` et `-.5` étaient acceptés comme flottants, ils sont refusés ; `1-2` est refusé dès le lexer (avant : `1` était lu et l'erreur venait du caractère suivant).
- `parseNumber` et `parseDecimal` restent (utilisées par `Reader` et `integerLexerExtended.cpp`).
- Tests : `lexer-number-buffer` (entier signé, non signé, flottants, buffers de 1 à 12 octets et décalages), `lexer-double-format-ko`, `lexer-int-format-ko`.
- Résultat : 200 000 doubles de 13,6 ms à 10,1 ms (**-26 %**) ; 200 000 entiers 64 bits de 13,5 ms à 12,1 ms (-10 %). Peu d'effet sur `data_set.json`, qui contient peu de nombres.
- `parseUnknownNumber` (champs ignorés) avait ses propres règles : voir J11.

### J10. `max_payload` n'était jamais appliqué — vérifié — **corrigé le 2026-10-31**
`ReaderContext` préparait `_checkSize`, mais rien ne l'appelait : une entrée de taille quelconque était acceptée, quelle que soit l'option.
Correction :
- `ReaderContext::checkPayloadSize()` est appelée après la lecture initiale et après chaque `refillBuffer`, et lève `POTENTIAL_OVERFLOW` au-delà de `max_payload` (`0` = illimité). `_checkSize` est supprimé.
- Le constructeur de `ReaderContext` n'est plus `noexcept` ; il libère son buffer avant de propager l'erreur (le destructeur n'est pas appelé).
- La documentation de `max_payload` dans `mappingOptions.hpp` (copiée de `deserialize_max_field_char`) est corrigée.
- La taille comptée est celle lue dans le flux, par blocs de `buffer_size` : des données placées après le JSON dans le même flux sont comptées si elles sont lues dans le même bloc.
- Tests ajoutés (`testJsonMapper.cpp`) : `json-deser-max-payload-ko` (dépassement à la lecture initiale), `json-deser-max-payload-refill-ko` (dépassement au remplissage du buffer), `json-deser-max-payload-limit` (taille exactement égale acceptée, buffers de 3 et 1024 octets).

### J11. Nombres des champs ignorés : exposants refusés, nombres invalides acceptés — vérifié — **corrigé le 2026-10-31**
`parseUnknownNumber` (`unknownLexer.cpp`, utilisé avec `deserialize_ignore_unknown_field = true`) ne reconnaissait que les chiffres et le `.`.
Reproduction (2026-10-31), champ `u` ignoré dans `{"u":…,"s":"a"}` :
- refusés alors que valides : `1e5`, `-1.5E-3`, `2.5e+10`, `[1e2,3]` (« Unexpected character 'e' ») ;
- acceptés alors qu'invalides : `1.`, `-`, `01`.
Correction : `parseUnknownNumber` utilise `scanNumber` et `isJsonNumber`, comme `lexNumber` (déplacées dans `inc/json/numberScanner.hpp` avec `isNumberChar`). Les nombres ignorés sont validés mais pas convertis.
Tests : `json-deser-unknown-number` (10 nombres valides, exposants compris) et `json-deser-unknown-number-ko` (9 nombres invalides).

### J12. UUID invalide : `std::invalid_argument` au lieu de `MappingError` — **corrigé le 2026-10-31**
`PrimitiveMapper<anch::UUID>::deserializeValue` (`uuidMapper.cpp`) laissait passer l'exception de `UUID::parse` : un document contenant un UUID invalide levait `std::invalid_argument`, que le code appelant ne s'attend pas à recevoir de `anch-json`.
Correction : l'exception est convertie en `MappingError` (`INVALID_FORMAT`).
- Message : nouveau constructeur `MappingError(ErrorCode, const ReaderContext&, std::string_view message)`, qui remplace le message générique du code d'erreur (l'extrait de l'entrée autour de l'erreur est toujours ajouté). Le mapper UUID lui passe le message de `UUID::parse`, par exemple « Invalid UUID '1234zzzz-…': unexpected character at position 4 near '614174000"' » (avant : « Unexpected character '"' found at 9 … »). Le test `json-deser-uuid-ko` vérifie que le message vient de `UUID::parse`. Les autres erreurs gardent leurs messages génériques.
- Tests (seulement avec `ANCH_UUID`) : `json-deser-uuid` (UUID valide, majuscules acceptées) et `json-deser-uuid-ko` (`1234zzzz-…`, trop court, vide, texte quelconque).

### J13. Extrait « near » des messages d'erreur tronqué — vérifié — **corrigé le 2026-10-31**
`computeErrorMessage` (`mappingError.cpp`) devait afficher 10 caractères avant et après l'erreur :
- la partie « après » ne s'affichait jamais : condition de boucle `i < 11` avec `i` partant de la position de l'erreur, et `ReaderContext::bufferSize` jamais renseigné (toujours `0`) ;
- les `'\0'` de fin de données étaient écrits tels quels dans le message (message coupé à l'affichage) ;
- `INVALID_FORMAT` en fin de données affichait « Unexpected character '\0' » ; avec une position `0`, `buffer[-1]` était lu.
Correction :
- `bufferSize` vaut `options.buffer_size`. Effet de bord voulu : le `memset` du buffer après une erreur dans `Reader::parse`, sans effet jusqu'ici, nettoie maintenant le buffer.
- Extrait : jusqu'à 10 caractères avant (caractère en erreur compris) et 10 après, sans les `'\0'`, arrêt à la fin des données.
- `INVALID_FORMAT` en fin de données : « Unexpected end of data ».
- Exemple : « Unexpected character 'x' found at 9 near '"s":"abc"x"012345678' » (avant : « … near '"s":"abc"x' »).
- La position « found at » reste relative au début de l'extrait, pas à l'entrée complète (la position absolue n'est pas connue du contexte).
- Tests : `json-error-message` (message exact, avec plus et moins de 10 caractères après l'erreur) et `json-error-message-end` (fin de données).

## Points positifs
- Lecture en flux avec buffer configurable (`MappingOptions::buffer_size`).
- 48 tests passent, plus un benchmark (`benchJson.cpp`).

## À approfondir
- [x] Tests aller-retour avec `"`, `\`, `\n` et caractères de contrôle (`json-ser-escape`) ; lecture de `\u00e9`, `\uD83D\uDE00` (J2).
- [x] Test de non-régression pour la profondeur (J8).
- [ ] Analyser les exposants et `-0` dans le lexer des flottants.

### J14. Objets ignorés de plus d'un champ refusés — vérifié — **corrigé le 2026-11-10**
`lexUnknownObject` (`unknownLexer.cpp`) consommait un caractère de trop après chaque `,` : le `discard()` de fin de boucle avalait le `"` du champ suivant, que le `discard()` du début de boucle attendait. Avec `deserialize_ignore_unknown_field`, tout objet ignoré d'au moins deux champs faisait échouer la lecture (« Unexpected character 'b' found … »), y compris dans un tableau ignoré. Les tests existants n'ignoraient que des objets vides ou d'un seul champ.
Trouvé en lisant des documents OpenAPI avec `anch-oas` (champs `paths`, `example`, … ignorés).
Correction : suppression du `discard()` en trop. Test `json-deser-unknown-object` (objet vide, un champ, deux champs, objets imbriqués avec espaces, tableau d'objets), qui échoue avant correction.

