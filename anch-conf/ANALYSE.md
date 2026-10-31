# Analyse — anch-conf

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

Hors périmètre : la configuration en cascade par profil est en construction (voir `cascade.todo`). Le module n'a pas été analysé en détail.

## Observations

- **`_loaded` jamais mis à `true`** — constaté le 2026-10-29, **corrigé provisoirement le 2026-10-30** : `load()` met désormais `_loaded = true` (`src/conf/configuration.cpp:205`), ce qui débloque `Configuration::inst()` (le test `SQLConnectionFactorySQLite` d'`anch-sql` passe de nouveau).
  Limite de ce correctif provisoire : l'affectation est dans la branche `else` de `placeholders.collect()`. Quand la configuration contient des placeholders (par exemple `test-placeholders.ini`), `resolve()` est appelé mais `_loaded` reste à `false` : `inst()` lève toujours `NOT_LOADED`, et un second appel à `load()` recharge tout. À reprendre lors de la finalisation des résolveurs.

- Résolveurs et placeholders en cours : le test `testPlaceHolders.cpp` vérifie pour l'instant que `${env=USER}` reste non résolu (l'assertion sur la valeur résolue est commentée).
- `src/conf/configuration.cpp:165-167` : `std::getenv("ANCH_PROFILES")` est appelé deux fois alors que le résultat du premier appel est déjà dans `env`.
- `Configuration::loader()` renvoie une instance `static` : les appels concurrents à `name()` / `profiles()` / `load()` ne sont pas protégés (à garder en tête si le chargement peut se faire depuis plusieurs threads).
- 13 tests passent.

## À approfondir
- [ ] Mettre `_loaded = true` aussi après `placeholders.resolve()` (ou en fin de `load()`).
- [ ] Analyse complète une fois la cascade terminée (includes, profils par défaut, `ANCH_PROFILES`).
- [ ] Détection de cycles dans les placeholders (`${a}` qui renvoie vers `${b}` qui renvoie vers `${a}`).
- [ ] Détection de cycles dans les `includes`.
