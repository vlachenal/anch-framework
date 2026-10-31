# Analyse — anch-date

Analyse du 2026-10-27, complétée le 2026-10-30. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : `src/date/date.cpp`, `inc/date/impl/date.hpp`, `dateFormatter.cpp`, formateurs `fmt/` (heures, marqueur, année, millisecondes).

## Constats

### D1. `localtime` non sûr entre librairies — lecture (constat précisé le 2026-10-30)
`src/date/date.cpp:41, 50, 68` : `localtime()` renvoie un pointeur vers un tampon statique partagé. Dans `anch-date`, les appels sont sérialisés par `Date::_mutex`, mais ce verrou ne protège pas contre les autres appels à `localtime` du processus, par exemple `anch-log` (`fmt/dateFormatter.cpp:45`).
Piste : `localtime_r`, ou `std::chrono::zoned_time` (C++20, disponible en GCC 13+) ; le mutex devient alors inutile.

### D2. `mktime` lit un `tm_isdst` non initialisé — lecture
`computeTimestamp()` (`src/date/date.cpp:106`) déclare `std::tm tmpTm;` sans initialisation, et `computeTm()` (`inc/date/impl/date.hpp`) ne renseigne ni `tm_isdst`, ni `tm_wday`, ni `tm_yday`. `mktime` lit donc une valeur indéterminée de `tm_isdst` : selon son contenu, le timestamp calculé est décalé d'une heure (ou correct, par hasard).
Piste : `std::tm tmpTm{};` et `tmpTm.tm_isdst = -1;` pour laisser la libc déterminer l'heure d'été.

### D3. Heure d'été ajoutée deux fois — vérifié
`src/date/date.cpp:89` (`initialize`) : si `tm_isdst > 0`, une heure est ajoutée à `tm_hour`. Or `localtime` renvoie déjà l'heure locale, heure d'été comprise.
Reproduction (2026-10-30) : `Date(1751364000)` (2025-07-01 10:00 UTC) avec `TZ=Europe/Paris` est formatée `13:00:00`, alors que `localtime_r` donne 12 h. Entre 23 h et minuit en été, l'heure vaut même 24.
Impact : `anch-log` utilise `anch-date` pour le motif `$D{...}` : les horodatages des logs ont une heure d'avance en été. Les tests passent car ils ne vérifient pas l'heure en période d'heure d'été.
Piste : supprimer la correction `tm_isdst` dans `initialize`.

### D4. Millisecondes fausses à partir d'un `timespec` — vérifié
`src/date/date.cpp:70-72` : `_milliseconds = tv_nsec % 1000000` (au lieu de `/ 1000000`), puis conversion en `uint16_t` qui tronque. Les microsecondes et nanosecondes en dépendent.
Reproduction : `tv_nsec = 123456789` donne `63573` millisecondes (456 789 tronqué sur 16 bits) au lieu de 123.
Piste : `ms = tv_nsec / 1000000`, `us = (tv_nsec / 1000) % 1000`, `ns = tv_nsec % 1000`.

### D5. `%y` affiche l'année sur 4 chiffres — vérifié
`src/date/fmt/year2dFormatter.cpp:43` : `std::setw(2)` n'est qu'une largeur minimale, l'année complète est écrite. `%y` donne `2025` au lieu de `25`.
Piste : `getYear(date) % 100`.

### D6. `%h` (heure sur 12 h) affiche `00` à midi et à minuit — vérifié
`src/date/fmt/hour12Formatter.cpp:45` : 12 h devient 0 et 0 h reste 0 ; la convention est `12` dans les deux cas (`12 a.m.`, `12 p.m.`). Reproduction : midi donne `00 p.m.`, minuit `00 a.m.`.
À noter : `%h` est l'abréviation du mois dans `strftime` ; l'équivalent `strftime` de ce formateur est `%I`. À documenter pour éviter la confusion.

### D7. `high_resolution_clock` converti avec `system_clock::to_time_t` — lecture
`src/date/date.cpp:34` : la norme n'impose pas que `high_resolution_clock` soit `system_clock` (c'est le cas avec libstdc++, pas forcément ailleurs). `std::chrono::system_clock::now()` est le choix portable.

## Points positifs
- 5 tests passent ; la documentation Doxygen a été initialisée récemment.

## À approfondir
- [ ] Tests en heure d'été (`TZ=Europe/Paris`, date de juillet), à minuit et à midi, avec un `timespec`.
- [ ] Évaluer le remplacement par `std::chrono` (calendrier et fuseaux C++20), qui réglerait D1 à D4.
- [ ] Analyser les formateurs restants (jours, mois, secondes, constantes).
