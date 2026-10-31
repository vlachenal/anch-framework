# Analyse — anch-uuid

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : `src/uuid.cpp`, `inc/uuid.hpp`, `inc/impl/uuid.hpp` (complété le 2026-10-30).

## Constats

### UU1. `parse` ne valide pas l'entrée — vérifié
`UUID::parse` découpe par positions fixes (`substr`) sans vérifier la longueur (36), les tirets ni les caractères hexadécimaux. Une chaîne courte lève `std::out_of_range` et une chaîne mal formée `std::invalid_argument` (via `stoul`), sans message métier. Certaines entrées invalides sont même acceptées (par exemple `"1234zzzz-..."`, `stoul` s'arrêtant au premier caractère non hexadécimal).
Reproduction (2026-10-30) : `UUID("1234zzzz-e89b-42d3-a456-426614174000")` est accepté et vaut `00001234-e89b-42d3-a456-426614174000`.
Piste : vérifier la longueur, la position des tirets et les caractères hexadécimaux avant conversion (ou `std::from_chars` en contrôlant `ptr`).

### UU2. Registre des fournisseurs non protégé — lecture
`registerProvider` écrit dans `_providers` sans `_mutex`, alors que `generateUUID` lit la map. Un enregistrement concurrent d'une génération est une course.

### UU3. `std::random_device` utilisé comme moteur — lecture
`getRandomEngine()` renvoie un `std::random_device`. Sous Linux c'est correct (`getrandom`), mais plus lent qu'un moteur pseudo-aléatoire initialisé une fois. La variable `std::random_device rand;` dans `registerRandomUUID` n'est pas utilisée. Les distributions `static` sont partagées entre threads.

## Points positifs
- UUID aléatoires conformes (vérifié le 2026-10-30 sur 10 000 tirages) : version `4` à chaque fois, variant RFC (`8`, `9`, `a`, `b`) uniformément réparti, longueur 36.
- Aller-retour `parse` / `toString` correct sur un UUID valide.
- 4 tests passent.

## À approfondir
- [ ] Tests de `parse` sur des entrées invalides.
- [ ] Tester les UUID de version 3 et 5 (fournisseurs enregistrés par `anch-crypto`) par rapport aux vecteurs de la RFC 9562.
