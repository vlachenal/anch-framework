# Analyse — anch-conf

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

Hors périmètre : la configuration en cascade par profil est en construction (voir `cascade.todo`). Le module n'a pas été analysé en détail.

## État

Cascade et placeholders terminés le 2026-11-12 (voir [cascade.todo](cascade.todo)) : 26 tests, propres sous ASan / UBSan.

## Constats corrigés le 2026-11-12

- **`splitTrim` bouclait à l'infini dès que la liste contenait deux éléments** (`while(std::size_t pos = str.find(...) != npos)` : `pos` valait le résultat de la comparaison) : `ANCH_PROFILES=dev,ut` ou deux includes bloquaient `load()`. Le découpage s'arrêtait aussi au premier espace d'un élément.
- `Configuration::section("a.b.c")` : même défaut de découpage.
- Les profils étaient chargés avant la lecture des includes : les fichiers `<include>-<profil>` n'étaient jamais lus ; ils le sont maintenant, après les includes.
- Clé `default-profiles` (décrite dans `cascade.todo`) prise en compte, `default-profile` gardée.
- Include introuvable signalé avec le nom du fichier principal ; double `getenv("ANCH_PROFILES")`.
- `ConfError` préfixait `UNKNOWN:` tous les messages sauf `NOT_READABLE` et `PARSING_ERROR`.
- **Placeholders jamais résolus** (`resolve()` vide) et **`_loaded` resté à `false`** quand la configuration en contenait. Résolution réécrite : événementielle (file synchrone propre au chargement, pas d'`EventBus` asynchrone et singleton), watchdog de profondeur, cycles et références absentes en erreur, valeurs résolues littérales. Les traces `std::cout` sont supprimées.

## Reste

- `Configuration::loader()` renvoie une instance `static` : les appels concurrents pendant le chargement ne sont pas protégés.
- ~~anch-ini : les espaces avant `=` font partie de la clé (`cle = valeur` donne la clé `"cle "`).~~ Corrigé le 2026-11-16 (I3).
- Résolveur `arg` protégé mais non enregistré ; placeholders imbriqués non supportés (erreur explicite).
