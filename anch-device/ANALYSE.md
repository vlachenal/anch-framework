# Analyse — anch-device

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).
Statut : **vérifié** = reproduit ; **lecture** = déduit du code.

Périmètre : symboles définis dans les headers publics, `network.cpp`, `networkInterface.cpp` (complété le 2026-10-30). `uuidProvider` n'est pas analysé : des modifications sont en cours (`uuidProvider.cpp`, `testUuid.cpp`).

## Constats

### DV1. `anch_cpuid` défini sans `inline` dans un header — vérifié — **corrigé le 2026-10-29**
`inc/device/cpu.hpp:33` : `void anch_cpuid(int info[4], int InfoType){...}` est une fonction libre définie dans le header. `nm` montre un symbole fort (`T`) dans tout fichier qui inclut `cpu.hpp` : inclure ce header dans deux `.cpp` fait échouer l'édition de liens.
Piste : `inline` (ou `static inline`), ou déplacer la définition dans un `.cpp`.

### DV2. Une seule interface atypique empêche de lister toutes les interfaces — lecture
`src/device/networkInterface.cpp` : le constructeur de `NetworkInterface` enchaîne six `ioctl` (drapeaux, adresse, MTU, adresse matérielle, broadcast, masque) et lève `DeviceException` au premier échec. `Network::load()` ne rattrape pas l'exception : une seule interface qui ne supporte pas l'un de ces appels (par exemple `SIOCGIFBRDADDR`, ligne 84, sur une interface point à point comme un tunnel VPN `tun0` / `wg0`) fait échouer `getInterface()` et `getInterfaces()` pour **toutes** les interfaces. Non reproduit ici (interfaces `lo`, `eno1`, `wlo1` uniquement).
Piste : rendre les informations facultatives (adresse de broadcast absente) ou ignorer l'interface fautive.

### DV3. `_metric` lu sans appel système — lecture
`src/device/networkInterface.cpp:99` : `ifr_metric` fait partie de l'union de `struct ifreq`, qui contient à ce moment le résultat de `SIOCGIFNETMASK`. Sans `ioctl(SIOCGIFMETRIC)`, la valeur lue est un morceau du masque réseau.

### DV4. Énumération limitée et IPv4 uniquement — lecture
`src/device/network.cpp:110` : `SIOCGIFCONF` avec un tampon de 1024 octets (environ 25 interfaces au maximum, troncature silencieuse). `SIOCGIFCONF` ne renvoie que les interfaces ayant une adresse IPv4 ; les adresses sont décodées en `sockaddr_in`.
Piste : `getifaddrs()`, qui couvre IPv4, IPv6 et les interfaces sans adresse, sans limite de taille.

### DV5. `reload()` invalide les références déjà distribuées — lecture
`getInterface()` renvoie un pointeur et `getInterfaces()` une référence (ligne 82) vers la map statique, après avoir relâché `MUTEX`. `reload()` vide cette map (ligne 89) : les pointeurs et références obtenus auparavant deviennent invalides, et un parcours de `getInterfaces()` concurrent d'un `reload()` est une course.
Piste : renvoyer des copies, ou un `std::shared_ptr` vers une map immuable remplacée à chaque `reload()`.

### DV6. Codes d'erreur sans `errno` — lecture
`DeviceException(..., ret)` reçoit la valeur de retour (`-1`) au lieu d'`errno` : le message ne permet pas de connaître la cause.

## À approfondir
- [ ] Analyser `uuidProvider` une fois les modifications en cours terminées.
