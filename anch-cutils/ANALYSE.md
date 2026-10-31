# Analyse — anch-cutils

Analyse du 2026-10-27, complétée le 2026-10-30. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : `inc/cutils/iostream.hpp`, `src/cutils/iostream.cpp` (flux C++ construits sur des fonctions de lecture et d'écriture, utilisés par `anch-network/tcpStream`).

## Constats

### CU1. Mode `INOUT` : lectures et écritures partagent le même tampon — vérifié
`src/cutils/iostream.cpp` : `CStreambuf` n'a qu'un tampon `_buffer.data`, utilisé à la fois comme zone de lecture (`setg` dans `underflow`, ligne 62) et zone d'écriture (`setp` dans `overflow` et `initBuffer`). Une lecture écrase les données écrites mais pas encore envoyées, et inversement.
Reproduction (2026-10-30) : sur un `CIOStream`, `io << "HELLO"` (non vidé), puis `io >> word` (le pair a envoyé `REPLY-FROM-PEER`), puis `io.flush()` : la fonction d'écriture reçoit `REPLY` au lieu de `HELLO`.
Impact : `TcpStream` (`anch-network`) hérite de `CIOStream` : tout échange question / réponse sans `flush()` explicite entre l'écriture et la lecture envoie des données corrompues.
Piste : deux tampons distincts (lecture et écriture), ou vider la zone d'écriture (`sync()`) au début de `underflow()`.

### CU2. Écritures partielles et erreurs ignorées — lecture
`overflow()` (ligne 75) ignore la valeur de retour de la fonction d'écriture : si elle écrit moins d'octets que demandé, le reste est perdu. `overflow()` renvoie toujours `not_eof`, donc `sync()` et `flush()` ne signalent jamais d'échec (le `failbit` n'est jamais positionné).
Piste : boucler jusqu'à ce que tout soit écrit, et renvoyer `eof()` si la fonction d'écriture renvoie 0.

### CU3. `TcpStream` : vidage final sur une socket déjà détruite — lecture
À la destruction d'un `TcpStream`, `~TcpStream` ferme la socket (`anch-network/src/network/tcpStream.cpp:43`), puis le membre `_socket` est détruit, et enfin le destructeur de la classe de base `CIOStream` supprime le `CStreambuf`, dont le destructeur (ligne 54) appelle `sync()`. Si des données sont encore en attente, la fonction d'écriture liée (`std::bind_front(&TcpSocket::write, &_socket)`) est appelée sur un objet détruit.
Piste : appeler `flush()` dans `~TcpStream` avant `_socket.close()`.

### CU4. Constructeur par défaut : tampon non initialisé — lecture
`CStreambuf() noexcept` (ligne 50) n'initialise pas `_buffer` (pointeur et taille indéterminés). Le destructeur appelle quand même `sync()`. Sans `setBuffer()` préalable, `overflow()` appelle `setp()` avec des valeurs indéterminées. Un second `setBuffer()` perd le tampon alloué par le premier (fuite).
Piste : `_buffer{}` dans la liste d'initialisation, et libérer l'ancien tampon dans `initBuffer()`.

### CU5. Messages d'erreur inversés dans `setBuffer` — lecture
`CIStream::setBuffer` (ligne 124) lève « output stream and null write function » quand la fonction de **lecture** manque ; `COStream::setBuffer` (ligne 149) lève « input stream and null read function » quand la fonction d'**écriture** manque. Les messages du constructeur (lignes 40 et 43) sont corrects.

## Points positifs
- Adaptateur simple et générique (`std::function` de lecture et d'écriture), réutilisable au-delà des sockets.
- 11 tests passent (sérialisation et désérialisation JSON à travers le flux).

## À approfondir
- [ ] Test de non-régression pour CU1 (écriture, lecture, puis `flush()` sur un `CIOStream`).
- [ ] Test avec une fonction d'écriture qui n'écrit qu'une partie des données.
