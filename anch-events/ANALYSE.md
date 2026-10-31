# Analyse — anch-events

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : tous les headers (`eventBus`, `observable`, `observer`, `eventHandler`, `event`) et leurs `impl/`.
Complété le 2026-10-30 (E2 vérifié, E7 à E9).

## Constats

### E1. Liste des observateurs parcourue sans verrou — lecture
`inc/events/impl/eventBus.hpp:120` : `processEvents` parcourt `_observers` sans tenir `_eventMutex`, alors que `addObserver` et `removeObserver` modifient ce `std::set` sous ce verrou. Un ajout ou un retrait pendant la diffusion peut invalider l'itérateur.
Piste : copier la liste sous verrou avant de diffuser, ou tenir le verrou pendant la diffusion (attention alors aux handlers qui s'abonnent ou se désabonnent).

### E2. `fireEvent` appelé depuis un handler : `std::terminate` — vérifié
`fireEvent` (ligne 81) fait `_thread->join()` quand la file est vide. `processEvents` dépile l'événement avant d'appeler les handlers : si un handler émet un événement sur le même bus pendant le traitement du dernier événement, le thread du bus fait `join()` sur lui-même.
`std::thread::join` lève alors `resource_deadlock_would_occur`, dans une fonction `noexcept`, d'où `std::terminate`.
Chaîner des événements depuis un handler est pourtant un usage courant.
Reproduction (2026-10-30) : un observateur qui appelle `EventBus<int>::FireEvent(e.body + 1)` dans `handle()` ; le programme s'arrête avec « terminate called after throwing an instance of 'std::system_error' — Resource deadlock avoided ».

### E3. Exception dans un observateur : `std::terminate` — lecture
`processEvents` est `noexcept` et n'attrape pas les exceptions des `handle()`.

### E4. `fireEvent` bloque l'appelant — lecture
`fireEvent` fait `join()` sur le thread précédent en tenant `_queueMutex`. L'appelant attend donc la fin des handlers en cours, ce qui annule l'intérêt de l'asynchronisme.
Piste : un thread de traitement permanent avec une variable de condition, au lieu de recréer un thread à chaque « vague » d'événements.

### E5. `release()` fait `delete this` sur un singleton statique — lecture
`inc/events/impl/eventBus.hpp:106` : l'instance vient de `Singleton<T>::getInstance()` (variable `static` locale). `delete this` sur un objet qui n'a pas été alloué par `new` est un comportement indéfini, et l'objet sera détruit à nouveau à la sortie du programme.

### E6. Destruction à la sortie du programme — lecture
Le destructeur fait `join()` sur le dernier thread. Si des handlers accèdent à d'autres objets statiques déjà détruits, l'ordre de destruction pose problème. À vérifier selon l'usage.

### E7. `Observable::notifyObservers` : interblocage si un observateur modifie la liste — vérifié
`inc/events/impl/observable.hpp:60-62` : `notifyObservers` tient `_mutex` (non récursif) pendant tous les appels à `handle()`. Un observateur qui appelle `removeObserver(*this)`, `addObserver(...)` ou `notifyObservers(...)` sur le même `Observable` depuis `handle()` reprend `_mutex` : interblocage.
Reproduction : un observateur qui se désabonne dans `handle()` ; le programme ne se termine jamais (arrêté par `timeout`).
Impact : `anch-network` (`Socket` hérite d'`Observable<SocketEvent>`) et `anch-ut` (runners socket) utilisent ce mécanisme.
Piste : copier `_observers` sous verrou puis diffuser sans le verrou (le même correctif règle E1 pour l'`EventBus`). Les verrous manuels (`lock()` / `unlock()`) gagneraient à être remplacés par `std::lock_guard`.

### E8. `EventHandler` : désabonnement trop tardif à la destruction — lecture
`inc/events/impl/eventHandler.hpp:45` : le désabonnement du bus est fait dans `~EventHandler`, qui s'exécute **après** le destructeur de la classe dérivée `D`. Entre les deux, le thread du bus peut appeler `handle()`, qui fait `static_cast<D*>(this)` (ligne 52) sur un objet `D` déjà détruit.
Piste : documenter que la classe dérivée doit se désabonner dans son propre destructeur, ou fournir une méthode `unsubscribe()` à appeler explicitement.

### E9. `Observer` : pas de destructeur virtuel ni de désabonnement automatique — lecture
`inc/events/observer.hpp` : `Observer<T>` est une classe polymorphe (méthode virtuelle pure) sans destructeur virtuel : détruire un observateur via un `Observer<T>*` est un comportement indéfini. Un observateur détruit sans `removeObserver` laisse un pointeur invalide dans `Observable` ou `EventBus`.

## Points positifs
- API simple (`FireEvent`, `AddObserver`) ; le commit `ca2e229f` a unifié `fireEvent` et `scheduleDeferred`.
- 3 tests passent.

## À approfondir
- [ ] Test : `addObserver` / `removeObserver` concurrents pendant une diffusion (sous TSan).
- [ ] Ajouter des tests de non-régression pour E2 et E7 (handler qui émet un événement, observateur qui se désabonne).
