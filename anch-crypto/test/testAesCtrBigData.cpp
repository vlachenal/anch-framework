#include <iostream>
#include <fstream>

#include "crypto/cipher/cbc.hpp"
#include "crypto/cipher/ctr.hpp"
#include "crypto/cipher/aes256.hpp"
#include "crypto/hash/sha1.hpp"

#include "crypto/padding/pkcs5Padding.hpp"

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

using anch::crypto::CBC;
using anch::crypto::CTR;
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
  std::cout << "Enter in AES - CTR tests" << std::endl;

  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  {
    std::cout << "Enter in OpenSSL CBC sequential with AES256 and PKCS5 padding tests" << std::endl;

    std::array<uint8_t, 16> iv = { {
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'
      } };

    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);

    std::ifstream* input = new std::ifstream("/home/winz/toto.avi", std::ifstream::binary);
    std::ofstream cbcOutCipher("Makefile.OpenSSL.cbc.aes256.pkcs5.bigdata.cipher", std::ofstream::binary);

    std::cout << "Cipher /home/winz/toto.avi" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    // Cipher with OpenSSL
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len = 0;

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

    // uint8_t* plaintext = new uint8_t[AES256::getBlockSize() * 4]();
    // uint8_t* ciphertext = new uint8_t[AES256::getBlockSize() * 4]();

    uint8_t plaintext[AES256::getBlockSize() * 4];
    uint8_t ciphertext[AES256::getBlockSize() * 4];

    /* Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    while(!input->eof()) {
      input->read(reinterpret_cast<char*>(plaintext), static_cast<std::streamsize>(AES256::getBlockSize() * 4));
      std::streamsize nbRead = input->gcount();
      if(nbRead == 0) {
	break;
      }
      if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, static_cast<int>(nbRead)))
	handleErrors();
      ciphertext_len = len;

      for(int i = 0 ; i < ciphertext_len ; i++) {
	cbcOutCipher << ciphertext[i];
      }
    }

    /* Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + ciphertext_len, &len))
      handleErrors();
    //ciphertext_len += len;

    for(int i = ciphertext_len ; i < ciphertext_len + len ; i++)
      cbcOutCipher << ciphertext[i];
    cbcOutCipher.flush();

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    EVP_cleanup();
    ERR_free_strings();

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cbcOutCipher.close();
    input->close();
    delete input;
    // delete[] plaintext;
    // delete[] ciphertext;
    std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Exit OpenSSL CBC with sequential AES256 tests" << std::endl;

  }

  std::cout << std::endl;

  std::array<uint8_t, 16> iv = { {
      'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'
    } };

  {
    std::cout << "Enter in CBC sequential with AES256 tests" << std::endl;

    std::ifstream* input = new std::ifstream("/home/winz/toto.avi", std::ifstream::binary);
    std::ofstream ctrOutCipher("Makefile.AnCH.cbc.aes256.pkcs5.bigdata.cipher", std::ofstream::binary);

    std::cout << "Cipher big data" << std::endl;
    CBC<AES256,PKCS5Padding> ctr(iv);
    start = std::chrono::high_resolution_clock::now();
    ctr.cipher(*input, ctrOutCipher, "foobar    rabooffoobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    ctrOutCipher.close();
    input->close();
    delete input;
    std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Exit CBC with sequential AES256 tests" << std::endl;
  }

  std::cout << std::endl;

  {
    std::cout << "Enter in CTR with parallel AES256 tests" << std::endl;

    // std::ifstream* input = new std::ifstream("/home/winz/toto.avi", std::ifstream::binary);
    // std::ofstream ctrOutCipher("Makefile.ctr.aes256.bigdata.async.cipher", std::ofstream::binary);

    // std::cout << "Cipher bigdata" << std::endl;
    // CTR<AES256> ctr(iv,2);
    // start = std::chrono::high_resolution_clock::now();
    // ctr.cipher(*input, ctrOutCipher, "foobar    raboof");
    // end = std::chrono::high_resolution_clock::now();
    // duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    // ctrOutCipher.close();
    // input->close();
    // delete input;
    // std::cout << "CTR parallel cipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Exit CTR with parallel AES256 tests" << std::endl;
  }

  std::cout << "Exit AES - CTR tests" << std::endl;
  return 0;
}
