# Analyse — anch-events

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : tous les headers (`eventBus`, `observable`, `observer`, `eventHandler`, `event`) et leurs `impl/`.
Complété le 2026-10-30 (E2 vérifié, E7 à E9).

## Constats

### E1. Liste des observateurs parcourue sans verrou — lecture — **corrigé le 2026-11-02**
`inc/events/impl/eventBus.hpp:120` : `processEvents` parcourt `_observers` sans tenir `_eventMutex`, alors que `addObserver` et `removeObserver` modifient ce `std::set` sous ce verrou. Un ajout ou un retrait pendant la diffusion peut invalider l'itérateur.
Piste : copier la liste sous verrou avant de diffuser, ou tenir le verrou pendant la diffusion (attention alors aux handlers qui s'abonnent ou se désabonnent).

### E2. `fireEvent` appelé depuis un handler : `std::terminate` — vérifié — **corrigé le 2026-11-02**
`fireEvent` (ligne 81) fait `_thread->join()` quand la file est vide. `processEvents` dépile l'événement avant d'appeler les handlers : si un handler émet un événement sur le même bus pendant le traitement du dernier événement, le thread du bus fait `join()` sur lui-même.
`std::thread::join` lève alors `resource_deadlock_would_occur`, dans une fonction `noexcept`, d'où `std::terminate`.
Chaîner des événements depuis un handler est pourtant un usage courant.
Reproduction (2026-10-30) : un observateur qui appelle `EventBus<int>::FireEvent(e.body + 1)` dans `handle()` ; le programme s'arrête avec « terminate called after throwing an instance of 'std::system_error' — Resource deadlock avoided ».

### E3. Exception dans un observateur : `std::terminate` — lecture — **conservé (contrat documenté)**
`processEvents` est `noexcept` et n'attrape pas les exceptions des `handle()`.

### E4. `fireEvent` bloque l'appelant — lecture — **corrigé le 2026-11-02**
`fireEvent` fait `join()` sur le thread précédent en tenant `_queueMutex`. L'appelant attend donc la fin des handlers en cours, ce qui annule l'intérêt de l'asynchronisme.
Piste : un thread de traitement permanent avec une variable de condition, au lieu de recréer un thread à chaque « vague » d'événements.

### E5. `release()` fait `delete this` sur un singleton statique — lecture — **documenté**
`inc/events/impl/eventBus.hpp:106` : l'instance vient de `Singleton<T>::getInstance()` (variable `static` locale). `delete this` sur un objet qui n'a pas été alloué par `new` est un comportement indéfini, et l'objet sera détruit à nouveau à la sortie du programme.

### E6. Destruction à la sortie du programme — lecture — **documenté**
Le destructeur fait `join()` sur le dernier thread. Si des handlers accèdent à d'autres objets statiques déjà détruits, l'ordre de destruction pose problème. À vérifier selon l'usage.

### E7. `Observable::notifyObservers` : interblocage si un observateur modifie la liste — vérifié — **corrigé le 2026-11-02**
`inc/events/impl/observable.hpp:60-62` : `notifyObservers` tient `_mutex` (non récursif) pendant tous les appels à `handle()`. Un observateur qui appelle `removeObserver(*this)`, `addObserver(...)` ou `notifyObservers(...)` sur le même `Observable` depuis `handle()` reprend `_mutex` : interblocage.
Reproduction : un observateur qui se désabonne dans `handle()` ; le programme ne se termine jamais (arrêté par `timeout`).
Impact : `anch-network` (`Socket` hérite d'`Observable<SocketEvent>`) et `anch-ut` (runners socket) utilisent ce mécanisme.
Piste : copier `_observers` sous verrou puis diffuser sans le verrou (le même correctif règle E1 pour l'`EventBus`). Les verrous manuels (`lock()` / `unlock()`) gagneraient à être remplacés par `std::lock_guard`.

### E8. `EventHandler` : désabonnement trop tardif à la destruction — lecture — **corrigé le 2026-11-02** (`unsubscribe()`)
`inc/events/impl/eventHandler.hpp:45` : le désabonnement du bus est fait dans `~EventHandler`, qui s'exécute **après** le destructeur de la classe dérivée `D`. Entre les deux, le thread du bus peut appeler `handle()`, qui fait `static_cast<D*>(this)` (ligne 52) sur un objet `D` déjà détruit.
Piste : documenter que la classe dérivée doit se désabonner dans son propre destructeur, ou fournir une méthode `unsubscribe()` à appeler explicitement.

### E9. `Observer` : pas de destructeur virtuel ni de désabonnement automatique — lecture — **corrigé le 2026-11-02**
`inc/events/observer.hpp` : `Observer<T>` est une classe polymorphe (méthode virtuelle pure) sans destructeur virtuel : détruire un observateur via un `Observer<T>*` est un comportement indéfini. Un observateur détruit sans `removeObserver` laisse un pointeur invalide dans `Observable` ou `EventBus`.

## Corrections du 2026-11-02

- **`ObserverList`** (nouveau, `inc/events/observerList.hpp`) : liste commune à `Observable` et `EventBus`. La diffusion se fait sans verrou sur une copie partagée de la liste (copie à l'écriture), donc un observateur peut ajouter ou retirer des observateurs, ou diffuser à nouveau, depuis `handle()` (E1, E7). Un observateur retiré pendant une diffusion n'est plus appelé par celle-ci. `remove` attend la fin des diffusions en cours dans les autres threads (compteur atomique, `std::atomic::wait`) : à son retour, l'observateur ne sera plus appelé et peut être détruit. Appelé depuis `handle()`, il n'attend pas les diffusions du thread courant. Les observateurs sont appelés dans leur ordre d'inscription (avant : ordre des adresses). Coût d'une notification : 14 ns au lieu de 10 ns (un verrou, une copie de `shared_ptr`).
- **`EventBus`** : un thread de traitement permanent (`std::jthread`, démarré au premier événement) et une variable de condition remplacent le thread recréé à chaque « vague » : `fireEvent` ne bloque plus (E4) et peut être appelé depuis un handler (E2). Le destructeur est public : un bus peut être un objet ordinaire (`release()` reste pour les bus alloués avec `new`, E5 documenté). Le destructeur diffuse les événements en file puis arrête le thread ; il ne doit pas être appelé depuis un observateur.
- **E3** : `Observer::handle` est `noexcept` par contrat ; documenté.
- **`Observer`** : destructeur virtuel ; la documentation impose de retirer l'observateur avant sa destruction, dans le destructeur de la classe la plus dérivée (E9).
- **`EventHandler`** : `unsubscribe()` à appeler dans le destructeur de la classe dérivée quand des événements peuvent arriver d'un autre thread (E8).
- `LessPtrCompare` n'est plus utilisé.
- Tests réécrits avec `anch-ut` (17 cas : réentrance, désabonnement pendant une diffusion, attente de `remove`, concurrence, bus non bloquant, événements chaînés, destruction). Sans signalement sous TSan / ASan. Sur l'ancienne version, les trois cas de réentrance bloquent (E7).

## Points positifs
- API simple (`FireEvent`, `AddObserver`) ; le commit `ca2e229f` a unifié `fireEvent` et `scheduleDeferred`.
- 17 tests (2026-11-02).

## À approfondir
- [x] Test : `addObserver` / `removeObserver` concurrents pendant une diffusion (sous TSan) (2026-11-02).
- [x] Ajouter des tests de non-régression pour E2 et E7 (handler qui émet un événement, observateur qui se désabonne) (2026-11-02).
