# Analyse — anch-crypto

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

Hors périmètre : la librairie est un projet « pour le fun », destiné à devenir un wrapper sur OpenSSL. Les algorithmes n'ont pas été analysés.

## Constats

### C1. Dépendance au `ThreadPool` défectueux — vérifié — **résolu le 2026-10-29** (via U1)
Tous les headers de modes de chiffrement (`cbc`, `cfb`, `ctr`, `ecb`, `ofb`, `pcbc`, `aes128/192/256`, via `bcModOp.hpp`) incluent `threadPool.hpp`. Ils héritent du constat U1 de [../anch-utils/ANALYSE.md](../anch-utils/ANALYSE.md) : définitions multiples si deux `.cpp` les incluent.

### C2. Lecture hors limites lors de l'expansion de clé AES-192 (AES-NI) — vérifié — **corrigé le 2026-10-29**
`inc/crypto/cipher/aes.hpp` : la clé AES-192 fait 24 octets, mais `aesni192ExpandKey` charge 16 octets à partir de l'octet 16 (`_mm_loadu_si128(key + 16)`) : 8 octets lus au-delà du tampon `aesKey[4*K]`. GCC le détecte en `-O3` (`-Warray-bounds`), ce qui bloquait le build Release.
Correction : le tampon local est arrondi au multiple de 16 octets supérieur et initialisé à zéro (`uint8_t aesKey[(4*K + 15) / 16 * 16] = {}`), dans les deux branches (`ANCH_CPU_DETECTION` et `ANCH_CPU_AES`).

### C3. SHA-2 : inversion des octets de mots non initialisés — vérifié (`-Wuninitialized`) — **corrigé le 2026-11-02**
`inc/crypto/hash/impl/sha2.hpp`, `transform` : `B` est la taille du bloc en octets (64 ou 128), mais `bytesSwap(chunk->words, B)` inversait `B` mots alors que le bloc n'en contient que 16 (64/4 et 128/8). Les mots suivants n'étaient pas initialisés (comportement indéfini), puis réécrits par le calcul du message schedule : le hash était juste. Détecté par GCC quand `anch::byteSwap` est passé à `std::byteswap` + `memcpy`.
Correction : inverser `B / sizeof(W)` mots. Reste : la boucle du message schedule va jusqu'à `B` (128 pour SHA-384/512) au lieu de `R` (80) : calculs inutiles.

## Points positifs
- 117 tests passent, plus un benchmark comparatif avec OpenSSL (`benchAnchOpenssl.cpp`), utile pour la future migration.

## À approfondir
- [ ] Définir l'API du wrapper OpenSSL (EVP) en gardant les interfaces actuelles (`Hash`, `BlockCipher`, `HMAC`).
