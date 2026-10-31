# Analyse — anch-network

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre lu : `socket.cpp`, `tcpSocket.cpp`, `udpSocket.cpp`, `tcpStream.cpp`, `socketEvent.cpp`.
Complété le 2026-10-30 (N1 et N3 vérifiés sous ASan, N7 à N10).

## Constats

### N1. Use-after-free sur l'adresse résolue — vérifié (ASan)
`src/network/socket.cpp:111-120` : `_address` parcourt la liste renvoyée par `getaddrinfo`, puis `::freeaddrinfo(result)` libère toute la liste.
`bind()` (ligne 143) et `connect()` (ligne 151) lisent ensuite `_address->ai_addr` et `_address->ai_addrlen` dans la mémoire libérée. Les tests passent parce que la mémoire n'a pas encore été réutilisée.
Reproduction (2026-10-30) : serveur et client `TcpSocket` sur `127.0.0.1`, compilés avec `-fsanitize=address` : `heap-use-after-free` dans `Socket::bind()` (`socket.cpp:143`) et `Socket::connect()` (`socket.cpp:151`).
Piste : copier `ai_addr` dans un `sockaddr_storage` membre (avec `ai_addrlen`) avant `freeaddrinfo`, ou garder la liste et la libérer dans le destructeur.

### N2. Lecture au-delà du buffer en réception — lecture
`src/network/tcpSocket.cpp`, `receive()` : `read(buffer, BUFFER_SIZE)` peut remplir tout le buffer, puis `message += buffer` le lit comme une chaîne C. Sans `'\0'` final, la lecture déborde.
Piste : `message.append(buffer, n)` avec le nombre d'octets reçus.

### N3. `SocketEvent(buffer)` sur des données non terminées — vérifié (ASan)
`read()` et `write()` construisent `SocketEvent(buffer)` à partir d'un `const char*` brut : même débordement que N2, et l'événement ne reflète pas la taille réelle.
Reproduction (2026-10-30) : envoi de 3000 octets, `receive()` côté serveur : `stack-buffer-overflow` dans `TcpSocket::read` (`tcpSocket.cpp:106`). N2 a la même cause, juste après dans `receive()`.

### N4. `receive()` ne s'arrête qu'à la fermeture du pair — lecture
La boucle lit tant que `recv` renvoie plus de 0 : elle bloque jusqu'à ce que le pair ferme la connexion. `send()` envoie `size() + 1` (le `'\0'`) : il semble y avoir un protocole « terminé par `'\0'` » côté envoi, que la réception n'exploite pas.

### N5. Écritures partielles non gérées — lecture
`send()` ignore le nombre d'octets réellement envoyés. `write()` le renvoie, mais l'adaptateur de flux (`tcpStream.cpp`) doit alors boucler. `EINTR` n'est pas géré.

### N6. Erreurs sans `errno` — lecture
`IOException("Error on send()")` ne transmet pas `errno` dans `send`, `write` et `read`, contrairement à `getaddrinfo`.

### N7. `UdpSocket::receive` : tampon non terminé — lecture
`src/network/udpSocket.cpp:100-101` : `recvfrom` lit au plus 1023 octets dans un tampon non initialisé, puis `message += buffer` le lit comme une chaîne C. Cela ne fonctionne que si l'émetteur envoie lui-même le `'\0'` final (ce que fait `UdpSocket::send`, avec `size() + 1`) ; un datagramme d'un autre émetteur provoque une lecture de données non initialisées, voire hors du tampon.
Piste : `message.append(buffer, res)`.

### N8. `UdpSocket` : IPv4 uniquement — lecture
- `send()` (ligne 74) passe `sizeof(*(_address->ai_addr))`, soit `sizeof(sockaddr)` (16 octets), comme longueur d'adresse : trop court pour une adresse IPv6 (`sockaddr_in6`, 28 octets), `sendto` échoue. Il faudrait `_address->ai_addrlen` (et `_address` est de toute façon invalide, voir N1).
- `getDomain()` (ligne 119) renvoie toujours `AF_INET`.

### N9. Datagrammes UDP tronqués silencieusement — lecture
Le tampon de réception fait 1024 octets : un datagramme plus long est tronqué sans erreur (pas de `MSG_TRUNC`).

### N10. `TcpStreamServer` n'accepte aucune connexion — lecture
`src/network/tcpStream.cpp:51` : le constructeur fait `bind()` puis `listen()`, mais jamais `accept()`. Les lectures et écritures du flux portent sur la socket d'écoute, ce qui échoue. Cela explique sans doute que le test `anch-network-test-tcp-stream` soit commenté.

## À approfondir
- [ ] Test avec un message d'au moins `BUFFER_SIZE` octets.
- [ ] Clarifier le protocole de messages (terminé par `'\0'` ? préfixé par la longueur ?).
- [ ] Ajouter une option CMake pour compiler les tests avec ASan / UBSan (par exemple un preset `asan`).
