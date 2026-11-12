# Analyse — anch-rest

Analyse du 2026-10-27, revue le 2026-11-12. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

## État

Réécriture décidée et terminée le 2026-11-12 : conception et étapes dans [anch-rest.todo](anch-rest.todo).

- **Étape 1 terminée le 2026-11-12** : `anch-rest-common` (modèle HTTP, codecs `Json` / `Text`) et `anch-rest-server` (`Api` : déclaration des endpoints, routage, négociation, filtres) ; 17 tests, propres sous ASan / UBSan.
- **Étape 2 terminée le 2026-11-12** : `anch-rest-client` (`Client`, `Call`, `Transport`, filtres d'authentification) et `anch-rest-curl` (`CurlTransport`, option `ANCH_REST_CURL`) ; 16 tests de plus (33 au total), dont 8 de bout en bout contre un serveur HTTP de test (anch-network), propres sous ASan / UBSan.
- **Étape 3 terminée le 2026-11-12** : module nginx en C (`ngx_http_anch_rest_module`, directives `anch_rest_app` et `anch_rest_thread_pool`), ABI C et pont C++ `anch-rest-nginx` (`ANCH_REST_APPLICATION`, option `ANCH_REST_NGINX`) ; 5 tests du pont par `dlopen` (38 au total), et test d'intégration `nginx/test/run-nginx-test.sh` validé avec nginx 1.30.6 (18 vérifications, pool de threads non bloquant).
- Toujours désactivé par défaut (`ANCH_BUILD_REST=OFF`), mais activé en CI : tests sous ASan / UBSan dans le job `asan`, module testé avec nginx 1.30.6 par le workflow `nginx` (version et SHA-256 de l'archive dans `.github/workflows/nginx.yml`).

## Ancien code (avant réécriture)

- Ne compilait plus : `castMapper` (type de retour `auto&`) utilisé avant d'être défini, et défini par l'utilisateur (le test le définissait).
- Routeur : `std::map<EndPoint, EndpointFunc*>` sans `operator<`, regex testée sur chaque route à chaque requête, paramètres de chemin anonymes, statut transmis par exception, `std::cout` de débogage.
- Négociation : `Content-Type` comparé tel quel (paramètres compris), `Accept` sans `q` ni jokers.
- Pas de paramètres de requête ; noms de headers sensibles à la casse ; `IM_USED` valait 209 au lieu de 226.
- Client : stubs (réponses vides), `CURLClient` non implémenté ; nginx : squelette.

## Constats annexes

- anch-json : `anch::json::serialize(std::string, out)` ne compile pas : `std::string` correspond à la surcharge des conteneurs `C<T>` avec `T = char`. Contourné dans le codec `Json` (type explicite).
