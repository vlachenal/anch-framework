# Analyse — anch-cutils

Analyse du 2026-10-27, complétée le 2026-10-30. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : `inc/cutils/iostream.hpp`, `src/cutils/iostream.cpp` (flux C++ construits sur des fonctions de lecture et d'écriture, utilisés par `anch-network/tcpStream`).

## Constats

### CU1. Mode `INOUT` : lectures et écritures partagent le même tampon — vérifié — **corrigé le 2026-11-02**
`src/cutils/iostream.cpp` : `CStreambuf` n'a qu'un tampon `_buffer.data`, utilisé à la fois comme zone de lecture (`setg` dans `underflow`, ligne 62) et zone d'écriture (`setp` dans `overflow` et `initBuffer`). Une lecture écrase les données écrites mais pas encore envoyées, et inversement.
Reproduction (2026-10-30) : sur un `CIOStream`, `io << "HELLO"` (non vidé), puis `io >> word` (le pair a envoyé `REPLY-FROM-PEER`), puis `io.flush()` : la fonction d'écriture reçoit `REPLY` au lieu de `HELLO`.
Impact : `TcpStream` (`anch-network`) hérite de `CIOStream` : tout échange question / réponse sans `flush()` explicite entre l'écriture et la lecture envoie des données corrompues.
Piste : deux tampons distincts (lecture et écriture), ou vider la zone d'écriture (`sync()`) au début de `underflow()`.

### CU2. Écritures partielles et erreurs ignorées — lecture — **corrigé le 2026-11-02**
`overflow()` (ligne 75) ignore la valeur de retour de la fonction d'écriture : si elle écrit moins d'octets que demandé, le reste est perdu. `overflow()` renvoie toujours `not_eof`, donc `sync()` et `flush()` ne signalent jamais d'échec (le `failbit` n'est jamais positionné).
Piste : boucler jusqu'à ce que tout soit écrit, et renvoyer `eof()` si la fonction d'écriture renvoie 0.

### CU3. `TcpStream` : vidage final sur une socket déjà détruite — lecture — **corrigé le 2026-11-02**
À la destruction d'un `TcpStream`, `~TcpStream` ferme la socket (`anch-network/src/network/tcpStream.cpp:43`), puis le membre `_socket` est détruit, et enfin le destructeur de la classe de base `CIOStream` supprime le `CStreambuf`, dont le destructeur (ligne 54) appelle `sync()`. Si des données sont encore en attente, la fonction d'écriture liée (`std::bind_front(&TcpSocket::write, &_socket)`) est appelée sur un objet détruit.
Piste : appeler `flush()` dans `~TcpStream` avant `_socket.close()`.

### CU4. Constructeur par défaut : tampon non initialisé — lecture — **corrigé le 2026-11-02**
`CStreambuf() noexcept` (ligne 50) n'initialise pas `_buffer` (pointeur et taille indéterminés). Le destructeur appelle quand même `sync()`. Sans `setBuffer()` préalable, `overflow()` appelle `setp()` avec des valeurs indéterminées. Un second `setBuffer()` perd le tampon alloué par le premier (fuite).
Piste : `_buffer{}` dans la liste d'initialisation, et libérer l'ancien tampon dans `initBuffer()`.

### CU5. Messages d'erreur inversés dans `setBuffer` — lecture — **corrigé le 2026-11-02**
`CIStream::setBuffer` (ligne 124) lève « output stream and null write function » quand la fonction de **lecture** manque ; `COStream::setBuffer` (ligne 149) lève « input stream and null read function » quand la fonction d'**écriture** manque. Les messages du constructeur (lignes 40 et 43) sont corrects.

### CU6. Tampon de taille 0 : récursion infinie — lecture — **corrigé le 2026-11-02**
Avec `size = 0`, la zone d'écriture est vide : `sputc` dans `overflow` rappelle `overflow`, indéfiniment. Une taille nulle est maintenant refusée (`std::invalid_argument`).

### CU7. Propriété du `CStreambuf` — lecture — **corrigé le 2026-11-02**
Les flux allouaient leur `CStreambuf` avec `new` et faisaient `delete rdbuf()` dans leur destructeur : après un appel à `rdbuf(autre)`, c'est le tampon de l'utilisateur qui était libéré (et le leur perdu). Le `CStreambuf` est maintenant un membre du flux.

### CU8. Appel virtuel dans le destructeur — lecture (clang-tidy) — **corrigé le 2026-11-02**
`~CStreambuf` appelait `sync()` (virtuelle), qui appelait `overflow()` (virtuelle) : pendant la destruction, les redéfinitions d'une classe dérivée ne sont pas appelées. Le destructeur appelle une fonction non virtuelle (`flushOutput`) et ignore les exceptions de la fonction d'écriture (une exception dans un destructeur appelle `std::terminate`).

### CU9. Tests incorrects — lecture — **corrigé le 2026-11-02**
La fonction de lecture des tests ignorait l'offset quand la taille du tampon dépasse la donnée, utilisait `substr(offset, offset + size)` (le 2e argument est une longueur) et `offset > length` au lieu de `>=`. Les tests n'avaient aucune assertion (affichage seulement).

## Corrections du 2026-11-02 : détails
- Zones de lecture et d'écriture distinctes (CU1). Les données écrites et non envoyées sont envoyées avant chaque lecture (comme un flux lié, `tie`) : un échange question / réponse n'a plus besoin de `flush()` explicite.
- La fonction d'écriture est rappelée jusqu'à ce que tout soit écrit ; `0` (ou une exception) est une erreur qui positionne le `badbit` (CU2).
- `cbuffer::write` prend un `const char*` (changement d'API : une fonction d'écriture `char*` ne compile plus ; `TcpSocket::write` prend déjà un `const char*`). Les champs de `cbuffer` ont des valeurs par défaut.
- Mémoire possédée par `std::unique_ptr<char[]>` (plus de `memset` inutile) ; `setBuffer` peut être appelé plusieurs fois (les données en attente sont envoyées avec l'ancienne fonction, plus de fuite) (CU4) ; messages d'erreur corrects (CU5).
- `TcpStream` (`anch-network`) : `flush()` avant de fermer la socket, puis le tampon est remplacé pour que la destruction du `CStreambuf` n'utilise plus la socket détruite (CU3).
- Tests : 21 cas avec assertions (JSON avec tampons de 1 à 1500 octets, question / réponse sans `flush`, écritures partielles, erreurs et exceptions d'écriture, fin de lecture, tampons invalides, `setBuffer` multiple, tampon fourni). Sans signalement sous ASan / TSan (les tests TCP / UDP d'`anch-network` échouent sous sanitizers à cause de N1, déjà connu).

## Points positifs
- Adaptateur simple et générique (`std::function` de lecture et d'écriture), réutilisable au-delà des sockets.
- 21 tests (2026-11-02).

## À approfondir
- [x] Test de non-régression pour CU1 (écriture, lecture, puis `flush()` sur un `CIOStream`) (2026-11-02).
- [x] Test avec une fonction d'écriture qui n'écrit qu'une partie des données (2026-11-02).
