# Analyse — anch-oas

Analyse du 2026-11-14. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

## État

48 tests, propres sous ASan / UBSan. O1 à O3 corrigés le 2026-11-19.

## Vérification par exécution

Document OpenAPI volontairement piégeux passé à `anch-oas-gen --server --client`, puis code généré compilé avec les options du projet (`-Wall -Wextra -Werror -Wconversion -Wsign-conversion`) : noms de propriétés et d'opérations qui sont des mots-clés ou des macros (`class`, `int`, `EOF`, `errno`, `NULL`), identifiants réservés (`__x`, `_X`), noms invalides (`a-b`, `a b`, `1x`, `é`, nom vide), noms qui ne diffèrent que par la casse ou la ponctuation (`x`, `X`, `x `), schémas nommés `std`, `int`, `A.B` / `A_B`, `Dup` / `dup`, modèles récursifs, références circulaires entre alias, valeurs d'énumération en collision (`a`, `A`, `a b`, `a-b`, chaîne vide, doublon), `*/` et guillemets dans les descriptions et les motifs, bornes hors plage.
Résultat : tout est généré et compile ; les cas non gérés sont signalés par un avertissement.

## Constats mineurs

### O1. `minimum` / `maximum` hors de la plage de `double` — vérifié — **corrigé le 2026-11-19**
`"minimum": 1e400` sur un `number` est recopié tel quel : le code généré ne compile pas avec `-Werror` (« la constante en virgule flottante déborde de la plage de double »). Les bornes hors plage des entiers sont, elles, signalées.

### O2. Contraintes incohérentes non signalées — vérifié — **corrigé le 2026-11-19**
`minLength` supérieur à `maxLength`, `minItems` supérieur à `maxItems` : générés sans avertissement (aucune valeur ne peut alors être valide). Une propriété listée dans `required` mais absente de `properties` est ignorée sans avertissement.

### O3. `??/` dans une description — vérifié — **corrigé le 2026-11-19**
Recopié dans un commentaire : avertissement `-Wtrigraphs`, donc erreur avec `-Werror`. Anecdotique.

Corrections (2026-11-19) :
- O1 : bornes, `multipleOf` et valeurs d'énumération qui ne s'écrivent pas en littéral C++ (hors de la plage de `double`, ou entiers hors de `int64_t` / `uint64_t` et de `double`) ignorés avec un avertissement ;
- O2 : avertissement (erreur avec `--strict`) quand `minLength` > `maxLength`, `minItems` > `maxItems`, `minProperties` > `maxProperties`, `minimum` > `maximum` (ou égaux avec une borne exclusive), et pour une propriété `required` absente de `properties` ; les contraintes sont générées telles quelles ;
- O3 : `??` écrit `? ?` dans les commentaires et `?\?` dans les chaînes ;
- tests `gen-constraints-inconsistent` et `gen-trigraphs` ; code généré depuis un document piégé compilé avec `-Werror -Wtrigraphs`.
