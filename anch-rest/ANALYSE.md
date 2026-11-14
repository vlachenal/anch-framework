# Analyse — anch-rest

Analyse du 2026-10-27, revue le 2026-11-12. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

## État

Réécriture décidée et terminée le 2026-11-12 : conception et étapes dans [anch-rest.todo](anch-rest.todo).

- **Étape 1 terminée le 2026-11-12** : `anch-rest-common` (modèle HTTP, codecs `Json` / `Text`) et `anch-rest-server` (`Api` : déclaration des endpoints, routage, négociation, filtres) ; 17 tests, propres sous ASan / UBSan.
- **Étape 2 terminée le 2026-11-12** : `anch-rest-client` (`Client`, `Call`, `Transport`, filtres d'authentification) et `anch-rest-curl` (`CurlTransport`, option `ANCH_REST_CURL`) ; 16 tests de plus (33 au total), dont 8 de bout en bout contre un serveur HTTP de test (anch-network), propres sous ASan / UBSan.
- **Étape 3 terminée le 2026-11-12** : module nginx en C (`ngx_http_anch_rest_module`, directives `anch_rest_app` et `anch_rest_thread_pool`), ABI C et pont C++ `anch-rest-nginx` (`ANCH_REST_APPLICATION`, option `ANCH_REST_NGINX`) ; 5 tests du pont par `dlopen` (38 au total), et test d'intégration `nginx/test/run-nginx-test.sh` validé avec nginx 1.30.6 (18 vérifications, pool de threads non bloquant).
- **OPTIONS automatique et CORS ajoutés le 2026-11-14** : `204` + `Allow` sans route OPTIONS, `Api::cors(CorsOptions)` (préflight avant les filtres, en-têtes CORS sur toutes les réponses) ; 45 tests, propres sous ASan / UBSan, test nginx à 22 vérifications.
- Toujours désactivé par défaut (`ANCH_BUILD_REST=OFF`), mais activé en CI : tests sous ASan / UBSan dans le job `asan`, module testé avec nginx 1.30.6 par le workflow `nginx` (version et SHA-256 de l'archive dans `.github/workflows/nginx.yml`).

## Ancien code (avant réécriture)

- Ne compilait plus : `castMapper` (type de retour `auto&`) utilisé avant d'être défini, et défini par l'utilisateur (le test le définissait).
- Routeur : `std::map<EndPoint, EndpointFunc*>` sans `operator<`, regex testée sur chaque route à chaque requête, paramètres de chemin anonymes, statut transmis par exception, `std::cout` de débogage.
- Négociation : `Content-Type` comparé tel quel (paramètres compris), `Accept` sans `q` ni jokers.
- Pas de paramètres de requête ; noms de headers sensibles à la casse ; `IM_USED` valait 209 au lieu de 226.
- Client : stubs (réponses vides), `CURLClient` non implémenté ; nginx : squelette.

## Constats annexes

- anch-json : `anch::json::serialize(std::string, out)` ne compile pas : `std::string` correspond à la surcharge des conteneurs `C<T>` avec `T = char`. Contourné dans le codec `Json` (type explicite).

## Passe du 2026-11-14

Relecture des sources de `common`, `server`, `client`, `curl`, `nginx` (pont C++ et module C), `protobuf` et `grpc` (`thrift` et les en-têtes du client non relus), et vérifications par exécution sous ASan / UBSan. Aucun défaut majeur.

Vérifié par exécution :
- `multipart/form-data` : délimiteur placé à chaque position autour de la limite du bloc de lecture de 64 Kio (140 positions, parts en mémoire puis en fichier temporaire, contenu terminé par `\r` ou contenant des quasi-délimiteurs) : contenu relu à l'identique. 30 000 corps mutés : aucun plantage, aucun blocage, aucun fichier temporaire oublié.
- Cas limites : préambule et épilogue ignorés, fins de ligne `\n` seules refusées, en-têtes repliés refusés, `filename*` prioritaire sur `filename`.

Constats mineurs :

### R1. Nom de fichier multipart : les `\` sont supprimés — vérifié
`filename="C:\dir\f.txt"` est lu `C:dirf.txt` : `parseDisposition` traite `\` comme un échappement de chaîne entre guillemets, alors que les navigateurs n'échappent pas (ils encodent `"` en `%22`, ce que `escape()` fait à l'écriture). Le sens écriture / lecture n'est pas symétrique non plus : un nom `a"b` est écrit `a%22b` et relu `a%22b`.
Piste : ne traiter que `\"` et `\\` comme échappements, ou aucun, et décoder `%22`, `%0D`, `%0A` à la lecture.

### R2. Métadonnées de réponse RPC à valeurs multiples — lecture
`handleRpc` (`protobuf/inc/rest/impl/rpcApi.hpp`) recopie les métadonnées avec `Response::header`, qui remplace la valeur : seule la dernière valeur d'un nom est envoyée (Connect et gRPC-Web), alors que le serveur gRPC les envoie toutes.

### R3. `CurlTransport` : corps en flux et redirection — lecture
Un corps envoyé en flux (`Content-Length` connu) ne peut pas être relu : sans `CURLOPT_SEEKFUNCTION`, une redirection 307 / 308 ou une authentification qui renvoie le corps échoue (`CURLE_SEND_FAIL_REWIND`). Les valeurs d'en-têtes ne sont pas contrôlées (`\r\n` dans une valeur ajoute des en-têtes), et la taille de la réponse lue en mémoire n'est pas bornée.

### R4. Réponse à `HEAD` : `Content-Length: 0` — lecture
`Api::dispatch` sert `HEAD` avec la route `GET` puis retire le corps ; le module nginx envoie alors `Content-Length: 0` au lieu de la longueur de la réponse `GET`.

À savoir : le corps JSON n'a pas de limite propre côté serveur (voir J19 dans `anch-json`).
