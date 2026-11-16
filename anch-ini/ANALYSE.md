# Analyse — anch-ini

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

Hors périmètre : en construction pour la configuration en cascade par profil. Le module n'a pas été analysé en détail.

## Observations

- 18 tests passent (2026-11-14).
- Conversion numérique correcte (vérifié à la lecture le 2026-10-30) : `toNumber` (`src/ini/section.cpp:101-114`) utilise `std::from_chars` et contrôle les trois cas d'erreur (`invalid_argument`, `result_out_of_range`, caractères restants). C'est le modèle à suivre pour `anch-cli` (CL1) et `anch-utils` (U9, U10).
- `anch-ini` n'était pas inclus dans le `CMakeLists.txt` racine alors que `anch-conf` en dépend (B5) : corrigé par la migration CMake du 2026-10-29.

## Constats (2026-11-14)

Reproduits avec un fichier de test lu par `anch::ini::merge` (sous ASan / UBSan).

### I1. Une ligne commentée contenant `=` devient une valeur — vérifié — **corrigé le 2026-11-14**
`KEYVAL_REGEX` (`src/ini/parser.cpp`) commence par `^[#; \t]*` : les caractères de commentaire en début de ligne sont ignorés, puis la ligne est lue comme `clé=valeur`.
- `; old=value` définit la clé `old` : désactiver une valeur en la commentant ne fonctionne pas.
- `# see http://host/page?a=b for details` définit la clé `see http://host/page?a` avec la valeur `b for details`.

Avec la cascade, une valeur commentée dans un fichier de profil écrase donc celle du fichier principal.
Correction : `parseLine` ignore la ligne dès que son premier caractère non blanc est `#` ou `;` (et les lignes vides), et `KEYVAL_REGEX` n'accepte plus de caractères de commentaire avant la clé. Test `parser-comment-keyval`, qui échoue avant correction.

### I2. `clé=` (valeur vide) est ignorée — vérifié — **corrigé le 2026-11-14**
La valeur est capturée par `(.+)` : une ligne sans valeur n'est pas reconnue, la clé n'existe pas (au lieu d'une chaîne vide). Impossible de vider une valeur dans un fichier de profil.
Correction : la valeur peut être vide (`(.*)`) : `clé=`, `clé=` suivi d'espaces ou d'un commentaire donnent une chaîne vide. Test `parser-empty-value`, qui échoue avant correction. 773/773 tests sous ASan / UBSan.

### I3. Espaces avant `=` dans la clé — vérifié (déjà noté dans `anch-conf`) — **corrigé le 2026-11-16**
`key = value` donne la clé `"key "` : `[^=]+` est gourmand, le `( |\t)*` suivant ne capture jamais rien.
Correction : la clé se termine par un caractère qui n'est ni un espace ni une tabulation (`([^=]*[^= \t])`) ; les espaces internes sont gardés (`inner space = v` donne la clé `inner space`). Test `parser-key-spaces` (dont `[TATA] tata = tutu` de `test.ini`, lu jusque-là sous la clé `"tata "`).

### I4. `getValue<bool>` : tout ce qui n'est pas `true` vaut `false` — vérifié — **corrigé le 2026-11-16**
`True`, `1`, `yes` ou une faute de frappe donnent `false` sans erreur, alors que les nombres invalides lèvent `BAD_CAST`.
Correction : `true` et `false` sans distinction de casse ; toute autre valeur (dont `1`, `yes`, une valeur vide) lève `ParserError` `BAD_CAST`, comme un nombre invalide. La documentation de `getValue` indiquait `std::bad_cast` : corrigée (`ParserError`). Concerne anch-log (`color`, `thread-safe`, `low-priority`) qui lit ses booléens par `getValue<bool>`. Tests `parser-bool-values`, `parser-bool-invalid`.

### I5. Fonctions et constantes dans l'espace de noms global — vérifié (`nm`) — **corrigé le 2026-11-14** (X1)
`parseValue`, `parseSection`, `parseLine`, `addIndent`, `printSection` sont exportées par `libanch-ini.so` hors de tout namespace. Voir le point transverse dans [../ANALYSE.md](../ANALYSE.md).

À savoir (choix à documenter) : `#` et `;` terminent une valeur où qu'ils soient (`url=http://host/a;b` donne `http://host/a`), sauf précédés de `\`.

## À approfondir
- [ ] Analyse complète une fois la cascade terminée.
- [ ] Comportement sur les sections et clés dupliquées, les valeurs multilignes, les commentaires en fin de ligne.
