# Analyse — anch-utils

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : tous les headers (`threadPool`, `resourcePool`, `singleton`, `lessPtrCompare`, `convert`, `optional`, `flux`, `fluent`, `stringUtils`, `endianness`, `processor`, `collectors`) et leurs `impl/`.
Complété le 2026-10-30 (U9 à U16).

## Constats

### U1. `ThreadPool` : définitions multiples à l'édition de liens — vérifié — **corrigé le 2026-10-29** (`inline` ajouté)
`inc/impl/threadPool.hpp` : le constructeur, le destructeur, `start`, `stop` et `execute` sont définis dans le header sans `inline` (seuls `launchTreatment` et `afterExecute` le sont).
Dès que deux `.cpp` incluent `threadPool.hpp`, `ld` échoue avec « définitions multiples de anch::ThreadPool::stop() », etc.
Impact : tous les headers de modes de chiffrement de `anch-crypto` (`cbc`, `ctr`, `ecb`... via `bcModOp.hpp`) en héritent.
Reproduction : deux fichiers `a.cpp` / `b.cpp` qui incluent `threadPool.hpp` et instancient `anch::ThreadPool`, liés ensemble.
Piste : ajouter `inline` à ces définitions.

### U2. `ThreadPool` : threads détachés qui survivent au pool — lecture — **corrigé le 2026-11-02**
`inc/impl/threadPool.hpp:128` : `launchTreatment` crée un `std::thread` puis le détache. Ce thread garde `this`.
Le destructeur vide la file mais n'attend pas les tâches en cours. Si le pool est détruit pendant une tâche, `execute` accède à un objet détruit : `afterExecute()`, puis `std::lock_guard` sur `_mutex`.
Même après `awaitTermination`, `execute` reprend `_mutex` après la notification : le pool peut déjà être détruit à ce moment-là.
Piste : garder les `std::thread` (ou `std::jthread`) et les joindre dans le destructeur, ou utiliser de vrais workers permanents.

### U3. `ThreadPool::awaitTermination` peut rester bloqué — lecture — **corrigé le 2026-11-02**
- `afterExecute` (ligne 133) modifie `_available` et appelle `notify_all` sans tenir `_mutex`, alors que l'attente (lignes 88, 93, 96) vérifie sa condition sous `_mutex`. Une notification peut se perdre entre la vérification et la mise en sommeil.
- Après expiration du délai (ligne 88), la file est vidée puis la fonction attend à nouveau sans limite (ligne 93) : le délai ne borne donc pas l'attente.
Piste : modifier `_available` et notifier sous `_mutex` ; décider de la sémantique voulue après expiration du délai.

### U4. `ThreadPool` : pas un vrai pool — lecture (conception) — **corrigé le 2026-11-02**
Un thread système est créé par tâche. Le nombre de threads simultanés est borné, mais ils ne sont pas réutilisés.

### U5. `ThreadPool::stop` attend en boucle active — lecture — **corrigé le 2026-11-02**
`stop()` tient `_mutex` et dort par tranches de 50 ms jusqu'au retour de toutes les tâches. Pas d'interblocage, car `_available` est incrémenté avant la prise du mutex dans `execute`, mais c'est fragile.

Correction de U2 à U5 (2026-11-02) : `ThreadPool` réécrit avec un nombre fixe de threads permanents (`std::jthread`) qui prennent les tâches dans la file ; un seul mutex et deux variables de condition (tâches, fin des tâches), plus d'attente active. Le destructeur abandonne les tâches en file et joint les threads (les tâches en cours sont attendues). `awaitTermination` : le délai borne l'attente des tâches en file ; les tâches en cours, non interruptibles, sont toujours attendues. `add` renvoie `false` si la tâche est refusée (fin en cours) et accepte des fonctions et arguments non copiables. Tests réécrits avec assertions (9 cas), sans signalement sous TSan / ASan.
`anch-crypto` n'inclut plus `threadPool.hpp` (il ne s'en servait pas) : `ThreadPool` n'est plus utilisé hors de ses tests.

### U6. `ResourcePool::borrowResource` : course sur `_availables` — vérifié (TSan, erreur de segmentation) — **corrigé le 2026-11-02**
`inc/impl/resourcePool.hpp:133` : l'attente se fait avec `_waitex`, mais la condition lit `_availables`, protégé par `_mutex`.
Après le réveil, `_mutex` est repris puis `_availables.front()` est appelé sans revérifier : un autre thread a pu prendre la ressource entre-temps, d'où un `front()` sur liste vide (comportement indéfini).
Piste : un seul mutex pour l'état et la variable de condition.
Correction : un seul mutex ; la condition d'attente (« ressource disponible ou place libre ») est revérifiée sous ce mutex.
Autres défauts corrigés en même temps :
- **réveil perdu** : `returnResource` notifiait sans tenir le mutex de l'attente, d'où des `TimeoutException` alors qu'une ressource était libre ;
- **création sous verrou** : `make_ptr` (ouverture d'une connexion SQL) était appelé en tenant `_mutex`, y compris depuis `invalidateResource` (donc depuis un destructeur). La création se fait maintenant hors verrou, après réservation d'une place ; une ressource invalide n'est plus recréée immédiatement mais à la demande ;
- **`TimeoutException` héritait de `std::exception` en privé** : `catch(const std::exception&)` ne l'attrapait pas ;
- `setTimeout` écrivait `_timeout` sans verrou pendant que `borrowResource` le lisait ;
- `maxSize == 0` donnait un pool inutilisable (toujours en délai dépassé) : lève maintenant `std::invalid_argument`.

### U7. `PoolableResource` est copiable — lecture — **corrigé le 2026-11-02** (déplaçable, non copiable)
`inc/resourcePool.hpp` : la copie implicite est disponible et le destructeur rend la ressource au pool. Une copie entraîne une double restitution.
Piste : `= delete` sur la copie, constructeur de déplacement qui vide la source.

### U8. `PoolableResource` peut survivre au pool — lecture — **corrigé le 2026-11-02** (état partagé `ResourcePoolState`, la ressource est détruite à sa libération si le pool n'existe plus)
Il garde une référence `_pool`. Si le pool est détruit avant, le destructeur de la ressource utilise une référence invalide.

### U9. `convert` accepte les nombres négatifs pour les types non signés — vérifié — **corrigé le 2026-11-02**
`inc/convert.hpp:44` : `conv >> num` sur un type non signé accepte le signe `-` et renvoie la valeur modulo 2^n, sans erreur.
Exemple : `anch::convert<uint32_t>("-1")` renvoie `4294967295`. `convert` est utilisé par `anch-sql` (`resultSet.cpp`, conversion des colonnes ; `postgresqlConnection.cpp`) et `anch-rest` (`endpointFunction.cpp`, paramètres de route) : une valeur négative lue dans une colonne ou une URL passe donc silencieusement. `anch-ini` utilise déjà `std::from_chars` (`src/ini/section.cpp:104`).
Piste : `std::from_chars` (C++17), qui refuse le signe pour les non signés et signale les dépassements (`std::errc::result_out_of_range`).

### U10. `convert<uint8_t>` / `convert<int8_t>` lèvent toujours une exception — vérifié — **corrigé le 2026-11-02**
`uint8_t` est un `unsigned char` : le flux lit un caractère et non un nombre. Pour `"7"`, il lit `'7'` (55) puis, la fin du flux n'étant pas encore atteinte (`eof()` faux), `std::bad_cast` est levée. Aucune conversion vers ces types ne peut réussir.
Piste : même solution que U9 (`std::from_chars` traite `uint8_t` comme un entier).

Correction de U9 et U10 : `convert` utilise `std::from_chars` et prend un `std::string_view`. Elle continue de lever `std::bad_cast` (les appelants de `anch-sql` et `anch-rest` l'attrapent).
Autres effets :
- la conversion ne dépend plus de la locale globale (le `stringstream` l'utilisait) ;
- environ 23× plus rapide (5 ns au lieu de 120 ns par `int64_t`, sans construction de `stringstream`), ce qui compte pour la lecture des colonnes de `anch-sql` ;
- `inf`, `nan`, `Infinity` et `NaN` (sans tenir compte de la casse) sont acceptés pour les flottants ;
- le signe `+` est maintenant refusé ;
- le type est contraint (`anch::Convertible` : entiers sauf `bool`, flottants).

### U11. `Optional` hérite de `std::optional` en privé — vérifié
`inc/optional.hpp:56` : `class Optional: std::optional<T>` (héritage privé par défaut pour `class`). `has_value()`, `value()`, `operator*`, `operator bool` sont inaccessibles depuis l'extérieur : `anch::Optional<int> o(3); o.has_value();` ne compile pas. Seuls `ifValue`, `ifValueOrElse` et `map` sont utilisables.
Autres points :
- les constructeurs « par déplacement » (lignes 206, 218) et le constructeur générique (ligne 237) copient au lieu de déplacer (`std::optional<T>(other)` sans `std::move` / `std::forward`), y compris celui marqué `noexcept` : si la copie de `T` lève, `std::terminate` ;
- les arguments `in_place` (lignes 224, 231) ne sont pas transmis avec `std::forward` ;
- C++23 fournit `std::optional::transform`, `and_then` et `or_else`, qui couvrent `map` et `ifValueOrElse`.
Piste : `class Optional: public std::optional<T>`, ou remplacer la classe par les fonctions libres `applyIfValue` / `applyIfValueOrElse` + les méthodes C++23.

### U12. `Fluent::forEach` modifie des copies — vérifié — **corrigé le 2026-11-02**
`inc/impl/fluent.hpp:230` : `for(auto val : _values)` copie chaque élément ; l'action `std::function<void(T&)>` reçoit la copie. Exemple : `forEach([](int& i){ i *= 10; })` sur `{1,2,3}` laisse le conteneur à `{1,2,3}`.
Même boucle par copie dans `allMatch`, `anyMatch` et `noneMatch` (lignes 126, 161, 196) : coût de copie pour les types lourds. Les filtres sont aussi copiés à chaque élément (`for(auto filter : filters)`).
Piste : `for(auto& val : _values)` et `for(const auto& filter : filters)`.

### U13. `Fluent` copié : les filtres `skip` / `limit` pointent vers l'original — vérifié (ASan) — **corrigé le 2026-11-02**
`skip()` (ligne 81) et `limit()` (ligne 93) ajoutent des lambdas qui capturent `this`. Le constructeur de copie et celui de déplacement copient `_filters` : les lambdas de la copie lisent et modifient les compteurs de l'objet d'origine. Si l'original est détruit, c'est un accès à de la mémoire libérée (ASan : `heap-use-after-free` dans la lambda de `skip`).
Piste : ne pas stocker `skip` / `limit` comme filtres, mais les appliquer dans les boucles à partir des compteurs de l'instance courante.

Correction de U12 et U13 : les opérations (`filter`, `skip`, `limit`) sont stockées comme des étapes sans référence à l'objet ; les compteurs de `skip` / `limit` sont locaux à chaque parcours (une opération finale peut être appelée plusieurs fois, plusieurs `skip` / `limit` sont possibles). Un seul parcours (`traverse`) remplace les quatre boucles dupliquées (`goto` supprimés) ; les éléments sont parcourus par référence. Un `Fluent` construit sur un conteneur temporaire (ou renvoyé par `map`) possède ses valeurs (`std::shared_ptr`) : le constructeur de déplacement ne copie plus le conteneur. Nouveaux tests, sans signalement sous ASan.

### U14. `Flux` : appels répétés et messages d'erreur — lecture — **corrigé le 2026-11-02**
- `ready()` (lignes 53, 56) appelle `set_value` sur les promesses du finaliseur et du gestionnaire d'erreur sans mettre `_fin` / `_err` à `true` : un second appel à `ready()`, ou `setFinalizer()` après `ready()`, lève `std::future_error` (« promise already satisfied ») au lieu du message prévu.
- `setErrorHandler` (ligne 106) lève « Finalizer function is already set » (copier-coller du message de `setFinalizer`).
- `push()` (lignes 63-64) attend indéfiniment tant que `ready()` n'a pas été appelé ou que le gestionnaire d'erreur n'est pas défini : à documenter.
- `_con`, `_fin` et `_err` sont des `bool` lus et écrits par des threads différents (les futures synchronisent les fonctions, pas ces drapeaux).

Correction : `ready()` positionne les drapeaux (appel multiple sans effet, `setFinalizer` / `setErrorHandler` après `ready()` lèvent « already set »), message de `setErrorHandler` corrigé, drapeaux et promesses protégés par un mutex. Nouveaux tests.

### U15. `endianness` et `processor` : remplaçables par le standard — lecture (conception) — **corrigé le 2026-11-02** pour `endianness`
- `isBigEndian()` / `isLittleEndian()` : `std::endian::native` (C++20) donne la même information à la compilation.
- `byteSwap()` : `std::byteswap` (C++23).
- `processor.hpp` (définition de `ANCH64` / `ANCH32` selon `ULONG_MAX`) n'est utilisé nulle part dans le framework.

Correction de `endianness` : `std::byteswap` et `std::endian` ; `isBigEndian` / `isLittleEndian` sont `constexpr`, `byteSwap` est limité aux entiers ; ajout de `<cstdint>` et des premiers tests. Ce changement a révélé un bug dans `anch-crypto` (voir C3). `processor.hpp` est conservé (usage prévu).

### U16. `stringUtils`, `collectors` — lecture
`LessPtrCompare` (2026-11-02) : comparaison faite avec `std::less<const T*>` (le `<` entre pointeurs d'objets différents n'est pas spécifié) ; il équivaut au comparateur par défaut de `std::set<T*>`, à préférer dans `anch-events`.
Pas de défaut relevé. `toLower` / `toUpper` convertissent correctement en `unsigned char` avant `std::tolower` / `std::toupper`, mais ne gèrent que l'ASCII (pas d'UTF-8) : à documenter.

### U17. `Singleton` : schéma recopié à la main dans chaque classe, ordre de destruction non documenté — lecture — **corrigé le 2026-11-02**
Chaque classe dérivée devait déclarer `friend`, un constructeur et un destructeur privés, et supprimer la copie. Résultat incohérent : `LoggerFactory`, `CPU` et `EventBus` restaient copiables (copie bloquée seulement par le destructeur privé), tous les destructeurs étaient `virtual` sans raison.
L'ordre de destruction (inverse de la *fin* des constructions) n'était pas documenté : les dépendances entre singletons sont implicites.
Correction : `Singleton` supprime la copie et le déplacement (hérité par toutes les classes dérivées), a un constructeur et un destructeur `protected` non virtuels, vérifie que `T` hérite de `Singleton<T>`, et documente le schéma et la règle de dépendance (appeler `getInstance` du singleton dont on dépend dans le constructeur).
Tests : instance unique sous concurrence (20 threads synchronisés par `std::latch`, joints), nouvel essai après une exception du constructeur, ordre de destruction (le test échoue si la dépendance est inversée).
À corriger dans les librairies concernées :
- `anch-sql` : `SqlConnectionFactory` dépend de `SQLSharedLibraries` (les connexions des pools viennent des pilotes fermés par `dlclose`) ; l'ordre est correct seulement parce que le constructeur appelle `registerMySQL()` & co. À rendre explicite ;
- `anch-events` / `anch-conf` : le constructeur d'`EventBus` est public (voulu : `anch-conf` crée son propre bus) mais son destructeur est privé, donc le bus créé par `Placeholders` ne peut pas être détruit (fuite) ;
- `anch-device` : `CPU` laisse ses drapeaux non initialisés si la feuille 1 de `cpuid` n'existe pas ;
- toutes : `virtual` sur les destructeurs et suppressions de copie devenus inutiles.

## Points positifs
- `Singleton` repose sur une variable `static` locale : initialisation sûre entre threads.
- 43 tests passent.

## À approfondir
- [ ] Ajouter un test liant deux TU qui incluent `threadPool.hpp`.
- [x] Tests de charge multi-thread sur `ResourcePool` (sous TSan) : `res-pool-concurrent` (2026-11-02).
- [x] Tests de `convert` sur les bornes (`"-1"` en non signé, dépassements, `uint8_t`) (2026-11-02).
- [x] Test de `Fluent::forEach` modifiant les éléments, et de la copie d'un `Fluent` avec `skip` / `limit` (2026-11-02).
