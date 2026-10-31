# Analyse — anch-rest

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

Non analysé.

## Observations

- Désactivé par défaut dans le `CMakeLists.txt` racine (`ANCH_BUILD_REST=OFF`) depuis la migration CMake.
- Les 19 tests du `build/` existant échouent (0/19) ; ce build est probablement ancien.
- Sous-modules : `common`, `router`, `client`, `curl`, `nginx`.
- Ne compile plus (vérifié le 2026-10-29, `-DANCH_BUILD_REST=ON`) : `common/inc/rest/impl/mapperRegistry.hpp:61` utilise `castMapper` (type de retour `auto&`) avant sa définition.
- `curl` et `nginx` n'étaient pas compilés par l'ancien `CMakeLists.txt` ; l'option `ANCH_REST_CURL_LIB`, sans effet, a été supprimée.

## À approfondir
- [ ] Décider si le module est réactivé, et à quel moment.
- [ ] Recompiler avec les versions actuelles des dépendances et relancer les tests.
