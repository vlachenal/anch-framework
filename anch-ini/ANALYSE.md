# Analyse — anch-ini

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

Hors périmètre : en construction pour la configuration en cascade par profil. Le module n'a pas été analysé en détail.

## Observations

- 16 tests passent.
- Conversion numérique correcte (vérifié à la lecture le 2026-10-30) : `toNumber` (`src/ini/section.cpp:101-114`) utilise `std::from_chars` et contrôle les trois cas d'erreur (`invalid_argument`, `result_out_of_range`, caractères restants). C'est le modèle à suivre pour `anch-cli` (CL1) et `anch-utils` (U9, U10).
- `anch-ini` n'était pas inclus dans le `CMakeLists.txt` racine alors que `anch-conf` en dépend (B5) : corrigé par la migration CMake du 2026-10-29.

## À approfondir
- [ ] Analyse complète une fois la cascade terminée.
- [ ] Comportement sur les sections et clés dupliquées, les valeurs multilignes, les commentaires en fin de ligne.
