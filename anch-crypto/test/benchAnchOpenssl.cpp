#include <iostream>
#include <fstream>
#include <sstream>

#include "crypto/cipher/cbc.hpp"
#include "crypto/cipher/aes256.hpp"
#include "crypto/hash/sha1.hpp"

#include "crypto/padding/pkcs5Padding.hpp"

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

using anch::crypto::CBC;
using anch::crypto::AES256;
using anch::crypto::SHA1;
using anch::crypto::PKCS5Padding;


void
handleErrors(void) {
  ERR_print_errors_fp(stderr);
  abort();
}

int
main(void) {
  std::cout << "Enter in AES/CBC/PKCS5 benchmark tests" << std::endl;

  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::array<uint8_t, 16> iv = { {
      'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'
    } };

  {
    std::cout << "Enter in OpenSSL CBC sequential with AES256 and PKCS5 padding tests" << std::endl;

    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    //OPENSSL_config(NULL);

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cbcOutCipher("Makefile.OpenSSL.cbc.aes256.PKCS5.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    // Cipher with OpenSSL
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
      handleErrors();

    /* Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>("foobar    rabooffoobar    raboof"), iv.data()))
      handleErrors();

    std::ostringstream inStrStream;
    inStrStream << input->rdbuf();
    std::string plaintext = inStrStream.str();
    uint8_t* ciphertext = new uint8_t[plaintext.size() + 16];

    /* Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, reinterpret_cast<const unsigned char*>(plaintext.data()), static_cast<int>(plaintext.size())))
      handleErrors();
    ciphertext_len = len;

    /* Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
      handleErrors();
    ciphertext_len += len;

    for(int i = 0 ; i < ciphertext_len ; i++)
      cbcOutCipher << ciphertext[i];
    cbcOutCipher.flush();

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cbcOutCipher.close();
    input->close();
    delete input;
    std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

    EVP_cleanup();
    ERR_free_strings();

    std::cout << "Decipher Makefile.OpenSSL.cbc.aes256.PKCS5.cipher" << std::endl;
    input = new std::ifstream("Makefile.OpenSSL.cbc.aes256.PKCS5.cipher", std::ifstream::binary);
    std::ofstream cbcOutDecipher("Makefile.OpenSSL.cbc.aes256.PKCS5.decipher");
    std::ostringstream inCipherStream;
    inCipherStream << input->rdbuf();
    std::string cipherText = inCipherStream.str();
    uint8_t* plainText = new uint8_t[cipherText.size() + 16];

    start = std::chrono::high_resolution_clock::now();

    // Decipher with OpenSSL
    EVP_CIPHER_CTX *ctxd;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctxd = EVP_CIPHER_CTX_new()))
      handleErrors();

    /* Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if(1 != EVP_DecryptInit_ex(ctxd, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>("foobar    rabooffoobar    raboof"), iv.data()))
      handleErrors();

    /* Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_DecryptUpdate(ctxd, plainText, &len, reinterpret_cast<const unsigned char*>(cipherText.data()), static_cast<int>(cipherText.size())))
      handleErrors();
    plaintext_len = len;

    /* Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctxd, plainText + len, &len))
      handleErrors();
    plaintext_len += len;

    for(int i = 0 ; i < plaintext_len ; i++)
      cbcOutDecipher << plainText[i];
    cbcOutDecipher.flush();

    /* Clean up */
    EVP_CIPHER_CTX_free(ctxd);


    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cbcOutDecipher.close();
    input->close();
    delete input;
    std::cout << "CBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

    EVP_cleanup();
    ERR_free_strings();

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.OpenSSL.cbc.aes256.PKCS5.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.OpenSSL.cbc.aes256.PKCS5.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit OpenSSL CBC with sequential AES256 and PKCS5 padding tests" << std::endl;
  }

  std::cout << std::endl;

  {
    std::cout << "Enter in AnCH CBC sequential with AES256 and PKCS5 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cbcOutCipher("Makefile.AnCH.cbc.aes256.PKCS5.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    CBC<AES256,PKCS5Padding> cbc(iv);
    cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cbcOutCipher.close();
    input->close();
    delete input;
    std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Decipher Makefile.AnCH.cbc.aes256.PKCS5.cipher" << std::endl;
    input = new std::ifstream("Makefile.AnCH.cbc.aes256.PKCS5.cipher", std::ifstream::binary);
    std::ofstream cbcOutDecipher("Makefile.AnCH.cbc.aes256.PKCS5.decipher");
    start = std::chrono::high_resolution_clock::now();
    cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cbcOutDecipher.close();
    input->close();
    delete input;
    std::cout << "CBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.AnCH.cbc.aes256.PKCS5.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.AnCH.cbc.aes256.PKCS5.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit AnCH CBC with sequential AES256 and PKCS5 padding tests" << std::endl;
  }

  std::cout << "Exit AES/CBC/PKCS5 benchmark tests" << std::endl;
  return 0;
}
