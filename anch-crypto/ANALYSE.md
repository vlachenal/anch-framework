# Analyse — anch-crypto

Analyse du 2026-10-27. Vue d'ensemble : [../ANALYSE.md](../ANALYSE.md).

Hors périmètre : la librairie est un projet « pour le fun », destiné à devenir un wrapper sur OpenSSL. Les algorithmes n'ont pas été analysés.

## Constats

### C1. Dépendance au `ThreadPool` défectueux — vérifié — **résolu le 2026-10-29** (via U1)
Tous les headers de modes de chiffrement (`cbc`, `cfb`, `ctr`, `ecb`, `ofb`, `pcbc`, `aes128/192/256`, via `bcModOp.hpp`) incluent `threadPool.hpp`. Ils héritent du constat U1 de [../anch-utils/ANALYSE.md](../anch-utils/ANALYSE.md) : définitions multiples si deux `.cpp` les incluent.
Le 2026-11-02 : include et `friend class anch::ThreadPool` supprimés de `bcModOp.hpp` (les modes de chiffrement créent leurs propres `std::thread`) ; ajout des includes standards dont il profitait par transitivité (`<deque>`, `<vector>`...).

### C2. Lecture hors limites lors de l'expansion de clé AES-192 (AES-NI) — vérifié — **corrigé le 2026-10-29**
`inc/crypto/cipher/aes.hpp` : la clé AES-192 fait 24 octets, mais `aesni192ExpandKey` charge 16 octets à partir de l'octet 16 (`_mm_loadu_si128(key + 16)`) : 8 octets lus au-delà du tampon `aesKey[4*K]`. GCC le détecte en `-O3` (`-Warray-bounds`), ce qui bloquait le build Release.
Correction : le tampon local est arrondi au multiple de 16 octets supérieur et initialisé à zéro (`uint8_t aesKey[(4*K + 15) / 16 * 16] = {}`), dans les deux branches (`ANCH_CPU_DETECTION` et `ANCH_CPU_AES`).

### C3. SHA-2 : inversion des octets de mots non initialisés — vérifié (`-Wuninitialized`) — **corrigé le 2026-11-02**
`inc/crypto/hash/impl/sha2.hpp`, `transform` : `B` est la taille du bloc en octets (64 ou 128), mais `bytesSwap(chunk->words, B)` inversait `B` mots alors que le bloc n'en contient que 16 (64/4 et 128/8). Les mots suivants n'étaient pas initialisés (comportement indéfini), puis réécrits par le calcul du message schedule : le hash était juste. Détecté par GCC quand `anch::byteSwap` est passé à `std::byteswap` + `memcpy`.
Correction : inverser `B / sizeof(W)` mots. Reste : la boucle du message schedule va jusqu'à `B` (128 pour SHA-384/512) au lieu de `R` (80) : calculs inutiles.

### C4. SHA-224 / SHA-384 : écriture au-delà du condensat — vérifié (ASan) — **corrigé le 2026-11-12**
`inc/crypto/hash/impl/sha2.hpp`, `finalize` : les 8 mots d'état étaient toujours copiés dans le condensat (32 octets en SHA-224, 64 octets en SHA-384), qui ne fait que 28 ou 48 octets. En SHA-384, 16 octets étaient écrits au-delà de l'objet (`stack-buffer-overflow`). En SHA-224, les 4 octets en trop tombaient par chance dans le bourrage d'alignement du contexte. La branche gros-boutiste copiait `sizeof(W) * O` octets et lisait 16 octets dans un entier de 8 pour la taille du message en SHA-384/512.
Correction : seuls les `O / sizeof(W)` premiers mots sont copiés (`static_assert` sur la taille), `memcpy` de `O` octets et taille du message sur 8 octets en gros-boutiste ; `if constexpr` sur le boutisme.

### C5. `Hash::digest(const std::string&)` : référence sur un pointeur nul en SHA-1 — vérifié (UBSan) — **corrigé le 2026-11-12**
`inc/crypto/hash/impl/hash.hpp` renvoyait `*_digest`, pointeur vers le condensat que `SHA1` ne renseignait jamais (les tests passaient par chance). Ce pointeur interne devenait aussi invalide après copie d'un objet de hachage (il pointait vers le condensat de l'original).
Correction : `_digest` est supprimé de `Hash` (et de `SHA2`, `SHA224_256`, `MD5`) ; les méthodes `digest(...)` renvoient le résultat de la méthode virtuelle `digest()`.

### C6. Tests : lecture hors limites et fuites — vérifié (ASan) — **corrigé le 2026-11-12**
`test/testSHA1.cpp` (`sha1-sec-password`) copiait `getBlockSize()` (64) octets du condensat SHA-1 au lieu de `getOutputSize()` (20). `test/benchAnchOpenssl.cpp` ne libérait pas ses tampons `new uint8_t[]` (remplacés par `std::vector`).

## Points positifs
- 117 tests passent, plus un benchmark comparatif avec OpenSSL (`benchAnchOpenssl.cpp`), utile pour la future migration.

### C7. Base64 : données binaires tronquées au premier octet nul — vérifié (2026-11-14)
`Base64::decode(const char*, uint64_t, std::ostream&, uint8_t)` (`src/crypto/b2t/base64.cpp`) écrit chaque groupe décodé avec `output << buffer`, c'est-à-dire comme une chaîne C : l'écriture s'arrête au premier octet nul du groupe.
Reproduction : les 6 octets `00 01 02 41 00 42` sont encodés en `AAECQQBC` ; leur décodage rend 1 octet (`41`). Seul le texte sans octet nul fait l'aller-retour.
Piste : compter les octets décodés de chaque groupe et les écrire avec `output.write`.

### C8. Base64 : lecture hors limites sans remplissage — vérifié (ASan, 2026-11-14)
La même fonction avance de 4 caractères par groupe avec `length -= 4` sur un `uint64_t` : quand la longueur n'est pas un multiple de 4, elle passe sous zéro et la boucle continue au-delà des données, jusqu'à un `=` ou un caractère invalide (`std::bad_cast`).
Reproduction : `Base64::encode("AB", URL | NOPADDING)` donne `QUI` ; `Base64::decode` de ces 3 caractères (tampon de 3 octets) : `heap-buffer-overflow` en lecture. La bibliothèque ne relit donc pas sa propre sortie sans remplissage.
Un groupe final d'un seul caractère (invalide) lisait aussi `bytes[1]` non initialisé : le tableau est maintenant initialisé à zéro (avertissement `-Wmaybe-uninitialized` devenu bloquant en release après X1) ; le reste n'est pas corrigé.
Piste : borner chaque groupe à `min(4, length)` et refuser un groupe d'un seul caractère.

## À approfondir
- [ ] Définir l'API du wrapper OpenSSL (EVP) en gardant les interfaces actuelles (`Hash`, `BlockCipher`, `HMAC`).
