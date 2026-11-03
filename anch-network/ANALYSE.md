# Analyse — anch-network

Analyse du 2026-10-27 ; refonte le 2026-11-03. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : `socket.cpp`, `tcpSocket.cpp`, `udpSocket.cpp`, `tcpStream.cpp`, `socketEvent.cpp`.
Complété le 2026-10-30 (N1 et N3 vérifiés sous ASan, N7 à N10).

## Refonte du 2026-11-03

Contexte : la librairie n'a jamais servi, elle était incomplète et peu testée. Son but est de manipuler les sockets réseau comme des flux C++, ce que la bibliothèque standard repousse de norme en norme (Networking TS, puis `std::execution` en C++26, toujours sans réseau). Choix validés : refonte orientée flux et suppression du modèle par événements.

Ancienne architecture : `Socket` / `TcpSocket` / `UdpSocket` envoyaient des « messages » (`std::string` + `'\0'` final) et notifiaient des observateurs (`Observable<SocketEvent>`, une copie de chaque bloc lu ou écrit) ; `TcpStreamServer` / `TcpStreamClient` étaient une surcouche de flux qui ne fonctionnait pas (N10). Les sockets n'étaient jamais fermées par le destructeur et étaient copiables.

Nouvelle architecture (POSIX, IPv4 et IPv6) :
- `Address` : adresse copiée dans un `sockaddr_storage` (règle N1) ; `resolve` (tous les résultats de `getaddrinfo`), `getHost`, `getPort`, `toString` (`[::1]:80`).
- `Socket` : propriétaire RAII du descripteur (fermé par le destructeur, déplaçable, non copiable, `FD_CLOEXEC`) ; `connect` et `bind` essaient toutes les adresses résolues ; `accept`, `send` / `sendAll` / `receive`, `sendTo` / `receiveFrom`, `shutdown`, délais de réception et d'envoi, adresses locale et distante. `EINTR` est géré (y compris un `connect` interrompu) ; `SIGPIPE` est évité (`MSG_NOSIGNAL`, ou `SO_NOSIGPIPE`).
- `TcpStream` : connexion TCP en `std::iostream`, sur `CStreambuf` d'`anch-cutils` (tampons de lecture et d'écriture distincts, données en attente envoyées avant chaque lecture). La socket est une classe de base déclarée avant `CIOStream` : elle est encore ouverte quand le tampon envoie les données en attente à la destruction. Fin de flux = fermeture (ou demi-fermeture) par le pair ; erreur réseau = `badbit` (et `IOException` avec `exceptions(std::ios::badbit)`). `shutdown(TRANSMISSION)` vide le tampon puis demi-ferme. Non copiable ni déplaçable : construit depuis un hôte et un port, ou depuis une `Socket` connectée.
- `TcpServer` : `bind` + `listen` ; `accept()` renvoie un `TcpStream` (élision de copie garantie), `acceptSocket()` une `Socket` à transmettre à un autre thread.
- `UdpSocket` : datagrammes binaires (`'\0'` compris), adresse de l'émetteur, datagramme plus grand que le tampon signalé (`EMSGSIZE`).
- `IOException` dérive de `std::system_error` : `code()` donne `errno` (catégorie générique) ou l'erreur `getaddrinfo` (`addrinfoCategory()`).

Supprimé : `TcpSocket`, `SocketEvent`, l'héritage `Observable`, `TcpStreamServer` / `TcpStreamClient`, les définitions Windows (`SOCKET`, `closesocket`, …), le type `SocketType::POSIX`, la dépendance à `anch-events`, les anciens tests (deux `main` qui ne vérifiaient rien et un test de flux commenté).
Effet sur `anch-ut` : le squelette `SocketServerRunner` n'hérite plus de `Observer<SocketEvent>` (sa méthode `handle()` était vide) ; `anch-ut` ne dépend plus d'`anch-network` ni d'`anch-events`.

Tests (`anch-network-test`, anch-ut, ports éphémères, délai de réception de 5 s pour ne jamais bloquer) : écho ligne à ligne, 1 Mo binaire avec un tampon de 1000 octets et demi-fermeture, fin de flux après les dernières données, IPv6 (`::1`, ignoré si indisponible), connexion refusée (`ECONNREFUSED`), délai de réception (`EAGAIN` levé par le flux), propriété du descripteur, UDP aller-retour avec `'\0'`, datagramme tronqué. Les 9 passent, aussi sous ASan + UBSan.

## Constats

### N1. Use-after-free sur l'adresse résolue — vérifié (ASan) — **corrigé le 2026-11-03** (refonte)
`src/network/socket.cpp:111-120` : `_address` parcourt la liste renvoyée par `getaddrinfo`, puis `::freeaddrinfo(result)` libère toute la liste.
`bind()` (ligne 143) et `connect()` (ligne 151) lisent ensuite `_address->ai_addr` et `_address->ai_addrlen` dans la mémoire libérée. Les tests passent parce que la mémoire n'a pas encore été réutilisée.
Reproduction (2026-10-30) : serveur et client `TcpSocket` sur `127.0.0.1`, compilés avec `-fsanitize=address` : `heap-use-after-free` dans `Socket::bind()` (`socket.cpp:143`) et `Socket::connect()` (`socket.cpp:151`).
Piste : copier `ai_addr` dans un `sockaddr_storage` membre (avec `ai_addrlen`) avant `freeaddrinfo`, ou garder la liste et la libérer dans le destructeur.

### N2. Lecture au-delà du buffer en réception — lecture — **corrigé le 2026-11-03** (refonte)
`src/network/tcpSocket.cpp`, `receive()` : `read(buffer, BUFFER_SIZE)` peut remplir tout le buffer, puis `message += buffer` le lit comme une chaîne C. Sans `'\0'` final, la lecture déborde.
Piste : `message.append(buffer, n)` avec le nombre d'octets reçus.

### N3. `SocketEvent(buffer)` sur des données non terminées — vérifié (ASan) — **corrigé le 2026-11-03** (refonte)
`read()` et `write()` construisent `SocketEvent(buffer)` à partir d'un `const char*` brut : même débordement que N2, et l'événement ne reflète pas la taille réelle.
Reproduction (2026-10-30) : envoi de 3000 octets, `receive()` côté serveur : `stack-buffer-overflow` dans `TcpSocket::read` (`tcpSocket.cpp:106`). N2 a la même cause, juste après dans `receive()`.

### N4. `receive()` ne s'arrête qu'à la fermeture du pair — lecture — **corrigé le 2026-11-03** (refonte)
La boucle lit tant que `recv` renvoie plus de 0 : elle bloque jusqu'à ce que le pair ferme la connexion. `send()` envoie `size() + 1` (le `'\0'`) : il semble y avoir un protocole « terminé par `'\0'` » côté envoi, que la réception n'exploite pas.

### N5. Écritures partielles non gérées — lecture — **corrigé le 2026-11-03** (refonte)
`send()` ignore le nombre d'octets réellement envoyés. `write()` le renvoie, mais l'adaptateur de flux (`tcpStream.cpp`) doit alors boucler. `EINTR` n'est pas géré.

### N6. Erreurs sans `errno` — lecture — **corrigé le 2026-11-03** (refonte)
`IOException("Error on send()")` ne transmet pas `errno` dans `send`, `write` et `read`, contrairement à `getaddrinfo`.

### N7. `UdpSocket::receive` : tampon non terminé — lecture — **corrigé le 2026-11-03** (refonte)
`src/network/udpSocket.cpp:100-101` : `recvfrom` lit au plus 1023 octets dans un tampon non initialisé, puis `message += buffer` le lit comme une chaîne C. Cela ne fonctionne que si l'émetteur envoie lui-même le `'\0'` final (ce que fait `UdpSocket::send`, avec `size() + 1`) ; un datagramme d'un autre émetteur provoque une lecture de données non initialisées, voire hors du tampon.
Piste : `message.append(buffer, res)`.

### N8. `UdpSocket` : IPv4 uniquement — lecture — **corrigé le 2026-11-03** (refonte)
- `send()` (ligne 74) passe `sizeof(*(_address->ai_addr))`, soit `sizeof(sockaddr)` (16 octets), comme longueur d'adresse : trop court pour une adresse IPv6 (`sockaddr_in6`, 28 octets), `sendto` échoue. Il faudrait `_address->ai_addrlen` (et `_address` est de toute façon invalide, voir N1).
- `getDomain()` (ligne 119) renvoie toujours `AF_INET`.

### N9. Datagrammes UDP tronqués silencieusement — lecture — **corrigé le 2026-11-03** (refonte)
Le tampon de réception fait 1024 octets : un datagramme plus long est tronqué sans erreur (pas de `MSG_TRUNC`).

### N10. `TcpStreamServer` n'accepte aucune connexion — lecture — **corrigé le 2026-11-03** (refonte)
`src/network/tcpStream.cpp:51` : le constructeur fait `bind()` puis `listen()`, mais jamais `accept()`. Les lectures et écritures du flux portent sur la socket d'écoute, ce qui échoue. Cela explique sans doute que le test `anch-network-test-tcp-stream` soit commenté.

Correspondance avec la refonte : N1 (`Address` copie l'adresse), N2 / N3 / N7 (tailles réelles, plus de chaînes C ni d'événements), N4 (plus de protocole de messages : le flux lit ce qui est disponible), N5 (`sendAll`, boucle du tampon d'`anch-cutils`, `EINTR`), N6 (`IOException` porte `errno`), N8 (`ai_addrlen`, IPv6), N9 (`MSG_TRUNC` → `EMSGSIZE`), N10 (`TcpServer::accept`).

## À approfondir
- [x] Test avec un message d'au moins `BUFFER_SIZE` octets (`tcp-large`).
- [x] Clarifier le protocole de messages : supprimé, le flux TCP n'a pas de délimiteur (au protocole applicatif de le définir).
- [ ] Ajouter une option CMake pour compiler les tests avec ASan / UBSan (par exemple un preset `asan`) : vérifié à la main le 2026-11-03.
- [ ] Non bloquant (`poll`, délais de connexion), TLS (wrapper OpenSSL, voir `anch-crypto`), sockets Unix (`AF_UNIX`) : non implémentés.
- [ ] Runners socket d'`anch-ut` (T2 à T4) : pourront s'appuyer sur `TcpServer` / `TcpStream`.
