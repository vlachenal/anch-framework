# Analyse — anch-date

Analyse du 2026-10-27, complétée le 2026-10-30 ; révision complète et corrections le 2026-11-03. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : `src/date/date.cpp`, `inc/date/impl/date.hpp`, `dateFormatter.cpp`, formateurs `fmt/` (heures, marqueur, année, millisecondes) ; le 2026-11-03 : tout le module, ses utilisateurs (`anch-log`, `anch-sql`, `anch-json`) et ce que la bibliothèque standard rend obsolète.
Vérifications du 2026-11-03 : programme de test (`probe`) lié à `libanch-date`, avec `TZ=Europe/Paris`.

## Révision du 2026-11-03 : ce que la bibliothèque standard remplace

`anch-date` est l'une des premières librairies du framework (C++11). Réécrite sur C++20/23, en gardant l'API publique utilisée par `anch-log` et `anch-sql` (`Date`, `DateFormatter`, motifs, extension par `IDatePartFormatter`) :
- **`Date`** : `std::chrono` (`sys_time<nanoseconds>`, `floor`, `time_point_cast`) pour le timestamp et les sous-secondes ; `localtime_r` (POSIX) pour l'heure locale, sans mutex. Les fuseaux de `std::chrono` (`current_zone`, `zoned_time`) ne sont pas utilisés : ils ne sont pas disponibles avec GCC 13, minimum du projet. Tous les constructeurs passent par une seule fonction `initialize`.
- **Comparaisons** : les 6 opérateurs écrits à la main sont remplacés par `operator<=>` et `operator==` (C++20) ; `after`, `before` et `equals` sont conservés.
- **Conversions** : `time_t`, `timespec` et `tm` sont calculés depuis le timestamp (plus de `mktime` sur les champs) ; ajout du constructeur et de la conversion `std::chrono::system_clock::time_point`.
- **`DateFormatter`** : l'analyse du motif par `std::regex` est remplacée par une boucle ; les parties sont des `std::unique_ptr` (plus de `delete` manuel) ; les valeurs sont lues par `std::from_chars` (au lieu de `std::istringstream`) et écrites par `std::format_to` (au lieu de `setfill` / `setw`) ; la validité du jour est vérifiée par `std::chrono::year_month_day::ok()`.
- **Formateurs de champ** : les 11 classes (`Year4DFormatter`, `Year2DFormatter`, `MonthFormatter`, `DayFormatter`, `Hour24Formatter`, `Hour12Formatter`, `MinuteFormatter`, `SecondFormatter`, `MillisecondFormatter`, `MarkerFormatter`, `ConstantFormatter`), soit 22 fichiers quasiment identiques, deviennent 3 classes internes de `dateFormatter.cpp` (champ numérique décrit par une table, marqueur, constante). Seule l'interface d'extension `IDatePartFormatter` reste publique. Le fichier `date-formatter-namespace.dox` documentait un namespace `anch::date::formatter` qui n'existait pas : supprimé.

Changements d'API :
- constructeurs de `Date` et opérateurs de conversion `explicit` (voir D13) ;
- `Date(false)` vaut l'epoch (le timestamp n'était pas initialisé, D12) ;
- `DateFormatter::parse(const std::string&)` renvoie une `Date` par valeur (au lieu d'un `Date*` à libérer) ;
- `DateFormatter` n'est plus copiable (D11), il est déplaçable ; son constructeur n'est plus `noexcept` ;
- suppression des en-têtes des formateurs de champ, de `Date::_mutex` et de `Date::computeTm` ;
- analyse plus stricte (D8, D10, D14).
Aucun autre module n'utilisait les éléments supprimés ; `anch-log` et `anch-sql` compilent sans changement (hors correction S8 d'`anch-sql`).

## Constats

### D1. `localtime` non sûr entre librairies — lecture (constat précisé le 2026-10-30) — **corrigé le 2026-11-03**
`src/date/date.cpp:41, 50, 68` : `localtime()` renvoie un pointeur vers un tampon statique partagé. Dans `anch-date`, les appels sont sérialisés par `Date::_mutex`, mais ce verrou ne protège pas contre les autres appels à `localtime` du processus, par exemple `anch-log` (`fmt/dateFormatter.cpp:45`).
Piste : `localtime_r`, ou `std::chrono::zoned_time` (C++20, disponible en GCC 13+) ; le mutex devient alors inutile.
Correction : `localtime_r` ; le mutex est supprimé.

### D2. `mktime` lit un `tm_isdst` non initialisé — lecture — **corrigé le 2026-11-03**
`computeTimestamp()` (`src/date/date.cpp:106`) déclare `std::tm tmpTm;` sans initialisation, et `computeTm()` (`inc/date/impl/date.hpp`) ne renseigne ni `tm_isdst`, ni `tm_wday`, ni `tm_yday`. `mktime` lit donc une valeur indéterminée de `tm_isdst` : selon son contenu, le timestamp calculé est décalé d'une heure (ou correct, par hasard).
Piste : `std::tm tmpTm{};` et `tmpTm.tm_isdst = -1;` pour laisser la libc déterminer l'heure d'été.
Correction : `computeTimestamp` part d'un `std::tm{}` avec `tm_isdst = -1`. Test : `date-conversions` (`Date(const std::tm*)` en été).

### D3. Heure d'été ajoutée deux fois — vérifié — **corrigé le 2026-11-03**
`src/date/date.cpp:89` (`initialize`) : si `tm_isdst > 0`, une heure est ajoutée à `tm_hour`. Or `localtime` renvoie déjà l'heure locale, heure d'été comprise.
Reproduction (2026-10-30) : `Date(1751364000)` (2025-07-01 10:00 UTC) avec `TZ=Europe/Paris` est formatée `13:00:00`, alors que `localtime_r` donne 12 h. Entre 23 h et minuit en été, l'heure vaut même 24.
Impact : `anch-log` utilise `anch-date` pour le motif `$D{...}` : les horodatages des logs ont une heure d'avance en été. Les tests passent car ils ne vérifient pas l'heure en période d'heure d'été.
Piste : supprimer la correction `tm_isdst` dans `initialize`.
Correction : correction supprimée. Test : `date-dst` (`TZ=Europe/Paris` : 12:00 en été, 11:00 en hiver, minuit en été).

### D4. Millisecondes fausses à partir d'un `timespec` — vérifié — **corrigé le 2026-11-03**
`src/date/date.cpp:70-72` : `_milliseconds = tv_nsec % 1000000` (au lieu de `/ 1000000`), puis conversion en `uint16_t` qui tronque. Les microsecondes et nanosecondes en dépendent.
Reproduction : `tv_nsec = 123456789` donne `63573` millisecondes (456 789 tronqué sur 16 bits) au lieu de 123.
Piste : `ms = tv_nsec / 1000000`, `us = (tv_nsec / 1000) % 1000`, `ns = tv_nsec % 1000`.
Correction : sous-secondes calculées une seule fois depuis le timestamp (`time - floor<seconds>(time)`). Test : `date-timespec` (`.123.456.789`, aller-retour `timespec`, `time_t` et `time_point`).

### D5. `%y` affiche l'année sur 4 chiffres — vérifié — **corrigé le 2026-11-03**
`src/date/fmt/year2dFormatter.cpp:43` : `std::setw(2)` n'est qu'une largeur minimale, l'année complète est écrite. `%y` donne `2025` au lieu de `25`.
Piste : `getYear(date) % 100`.
Correction : année modulo 100 ; à l'analyse, `00`-`68` donnent 2000-2068 et `69`-`99` 1969-1999, comme `strptime` (au lieu de 1900 + valeur). Test : `date-year-2-digits`.

### D6. `%h` (heure sur 12 h) affiche `00` à midi et à minuit — vérifié — **corrigé le 2026-11-03**
`src/date/fmt/hour12Formatter.cpp:45` : 12 h devient 0 et 0 h reste 0 ; la convention est `12` dans les deux cas (`12 a.m.`, `12 p.m.`). Reproduction : midi donne `00 p.m.`, minuit `00 a.m.`.
À noter : `%h` est l'abréviation du mois dans `strftime` ; l'équivalent `strftime` de ce formateur est `%I`. À documenter pour éviter la confusion.
Correction : `12` à midi et à minuit ; l'analyse accepte `01`-`12` (`12 a.m.` = minuit) et le marqueur n'ajoute 12 h que si l'heure est inférieure à 12. Documenté dans `DateFormatter` (`%h` = `%I` de `strptime`). Test : `date-hour-12` (formatage et aller-retour).

### D7. `high_resolution_clock` converti avec `system_clock::to_time_t` — lecture — **corrigé le 2026-11-03**
`src/date/date.cpp:34` : la norme n'impose pas que `high_resolution_clock` soit `system_clock` (c'est le cas avec libstdc++, pas forcément ailleurs). `std::chrono::system_clock::now()` est le choix portable.
Correction : `std::chrono::system_clock::now()`.

### D8. Analyse partielle : les champs absents du motif gardent la valeur de la date cible — vérifié — **corrigé le 2026-11-03**
`parse` ne modifiait que les champs du motif : `DateFormatter("%Y-%m-%d").parse("2012-10-23", date)` sur une `Date()` donnait `2012-10-23 20:32:12.397` (heure courante). `anch-sql` analyse ainsi les colonnes `DATE` et `TIME`.
Correction : les champs sont remis à 1970-01-01 00:00:00.000 (heure locale) avant l'analyse. Test : `date-parse-reset`.

### D9. Millisecondes ignorées dans le timestamp d'une date analysée — vérifié — **corrigé le 2026-11-03**
`computeTimestamp` ne prenait que la partie `mktime` (secondes) : `10:02:59.001` et `10:02:59.999` étaient égales et ordonnées à égalité.
Correction : sous-secondes ajoutées au timestamp. Test : `date-parse-milliseconds`.

### D10. Valeurs invalides acceptées à l'analyse — vérifié — **corrigé le 2026-11-03**
`std::istringstream` acceptait `" 5"` et `"1a"` ; le mois `00` donnait `0 - 1` sur un `uint16_t` (65535) ; aucune vérification du jour selon le mois (`2025-02-30` accepté, puis normalisé par `mktime` en `2025-03-02`).
Correction : `std::from_chars` sur le champ entier (chiffres uniquement), bornes de chaque champ, `year_month_day::ok()`. Test : `date-parse-invalid` (11 valeurs refusées, 29 février d'une année bissextile accepté).

### D11. `DateFormatter` copiable : double libération — lecture — **corrigé le 2026-11-03**
Le destructeur libérait les parties (pointeurs bruts) et le constructeur de copie implicite copiait les pointeurs : copier un `DateFormatter` provoquait une double libération.
Correction : `std::unique_ptr` ; copie interdite, déplacement autorisé. Test : `date-formatter-ownership` (`static_assert` et déplacement).

### D12. `Date(false)` : timestamp non initialisé — lecture — **corrigé le 2026-11-03**
`_timestamp` n'avait pas de valeur : comparer une `Date(false)` était indéterminé.
Correction : `Date(false)` vaut l'epoch (timestamp et champs cohérents). Test : `date-conversions`.

### D13. Constructeurs implicites : un pointeur devient `Date(true)` — vérifié — **corrigé le 2026-11-03**
`Date(bool init = true)` permettait des conversions implicites, y compris depuis un pointeur (conversion pointeur → `bool`). C'est ce qui rend silencieux le bug S8 d'`anch-sql` (`out = new anch::sql::Date(date)` donne la date courante). Les opérateurs de conversion vers `time_t`, `tm` et `timespec` étaient aussi implicites (`date + 1` compilait).
Correction : constructeurs et conversions `explicit` (aucun appel implicite dans le dépôt). Les types d'`anch-sql`, qui dérivent de `Date`, ont aussi leur constructeur `bool` rendu `explicit`.

### D14. Parties constantes non vérifiées à l'analyse — lecture — **corrigé le 2026-11-03**
`ConstantFormatter::setValue` renvoyait toujours `true` : `2025/01/01` était accepté avec le motif `%Y-%m-%d`.
Correction : la partie constante doit correspondre exactement. Test : `date-parse-invalid`.

### D15. Formatage : état du flux modifié et vidage systématique — lecture — **corrigé le 2026-11-03**
Les formateurs faisaient `output << setfill('0')` : le caractère de remplissage restait modifié sur le flux de l'appelant. `format(..., std::ostream&)` appelait `flush()` à chaque date (chaque ligne de log d'`anch-log`).
Correction : `std::format_to` (n'altère pas le flux), plus de `flush`.

### D16. Motif `%` inconnu ou final — lecture — comportement conservé
Un motif inconnu (`%Q`) ou un `%` final sont écrits tels quels ; `%%` n'est pas un échappement (écrit `%%`). Avant, la regex arrêtait l'analyse du motif au `%` final et ignorait la suite. Test : `date-unknown-pattern`.

### D17. `DateFormatter::parse` : dépassement signé hors de 1678-2261 — vérifié (fuzzing, UBSan) — **corrigé le 2026-11-16**
Trouvé par la cible `anch-fuzz-date` : `1013-11-16T20:41:20.123` est accepté par le motif `%Y-%m-%dT%H:%M:%S.%s`, puis `computeTimestamp` convertit le `time_t` de `mktime` en nanosecondes sur 64 bits avec dépassement (`-30172361281 * 1000000000`, comportement indéfini). L'analyseur RFC 3339 bornait déjà les secondes, pas `DateFormatter`.
Correction : `fromLocalTime` vérifie que les secondes tiennent en nanosecondes sur 64 bits et lève `std::invalid_argument` (« Date out of range (years 1678 to 2261) »), documenté dans `DateFormatter::parse` et `computeTimestamp`. Test `date-parse-invalid` (1013, 9999, 0000 refusés ; 1700 et 2261 acceptés).

## Points positifs
- 5 tests passaient ; 15 depuis le 2026-11-03 (10 tests ajoutés, lancés avec `TZ=Europe/Paris`).
- Extension par `IDatePartFormatter` / `registerFormatterPart` : conservée (le registre est désormais protégé par un mutex).

## À approfondir
- [x] Tests en heure d'été (`TZ=Europe/Paris`, date de juillet), à minuit et à midi, avec un `timespec`.
- [x] Évaluer le remplacement par `std::chrono` : fait pour le calendrier et les durées ; les fuseaux (`zoned_time`) demanderaient GCC 14 au minimum.
- [x] Analyser les formateurs restants (jours, mois, secondes, constantes).
- [ ] Heure locale inexistante ou ambiguë au changement d'heure (`2025-03-30 02:30` à Paris) : normalisée par `mktime` (03:30), sans erreur.
- [ ] Avec GCC 14 comme minimum : `std::chrono::zoned_time` remplacerait `localtime_r` / `mktime`, et `std::format` / `std::chrono::parse` une partie de `DateFormatter`.
