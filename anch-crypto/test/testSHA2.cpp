#include "crypto/hash/sha224.hpp"
#include "crypto/hash/sha256.hpp"
#include "crypto/hash/sha384.hpp"
#include "crypto/hash/sha512.hpp"

#include <iostream>
#include <sstream>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::crypto::SHA224;
using anch::crypto::SHA256;
using anch::crypto::SHA384;
using anch::crypto::SHA512;
using std::string;

void
testSHA224() {
  std::string msg = "The quick brown fox jumps over the lazy dog";
  std::cout << "Hash SHA224 message: " << msg << std::endl;
  SHA224 hash(msg);
  std::ostringstream out;
  out << hash;
  std::string res = out.str();

  anch::ut::assertTrue(res == "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525");
  // std::string sum = "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525";
  // if(res != sum) {
  //   std::cerr << "Hash are differents:" << std::endl;
  //   std::cerr << "AnCH result: " << res << std::endl;
  //   std::cerr << "Instead of:  " << sum << std::endl;
  //   return 1;

  // } else {
  //   std::cout << "Found value: " << res << std::endl << std::endl;
  // }
  std::cout << "Found value: " << res << std::endl << std::endl;
}

void
testSHA256() {
  std::string msg = "The quick brown fox jumps over the lazy dog";
  std::cout << "Hash SHA256 message: " << msg << std::endl;
  SHA256 hash(msg);
  std::ostringstream out;
  out << hash;
  std::string res = out.str();

  anch::ut::assertTrue(res == "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");
  // std::string sum = "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592";
  // if(res != sum) {
  //   std::cerr << "Hash are differents:" << std::endl;
  //   std::cerr << "AnCH result: " << res << std::endl;
  //   std::cerr << "Instead of:  " << sum << std::endl;
  //   return 1;

  // } else {
  //   std::cout << "Found value: " << res << std::endl << std::endl;
  // }
  std::cout << "Found value: " << res << std::endl << std::endl;
}

void
testSHA384() {
  std::string msg = "The quick brown fox jumps over the lazy dog";
  std::cout << "Hash SHA384 message: " << msg << std::endl;
  SHA384 hash(msg);
  std::ostringstream out;
  out << hash;
  std::string res = out.str();

  anch::ut::assertTrue(res == "ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c494011e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1");
  // std::string sum = "ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c494011e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1";
  // if(res != sum) {
  //   std::cerr << "Hash are differents:" << std::endl;
  //   std::cerr << "AnCH result: " << res << std::endl;
  //   std::cerr << "Instead of:  " << sum << std::endl;
  //   return 1;

  // } else {
  //   std::cout << "Found value: " << res << std::endl << std::endl;
  // }
  std::cout << "Found value: " << res << std::endl << std::endl;
}

void
testSHA512() {
  std::string msg = "The quick brown fox jumps over the lazy dog";
  std::cout << "Hash SHA512 message: " << msg << std::endl;
  SHA512 hash(msg);
  std::ostringstream out;
  out << hash;
  std::string res = out.str();

  anch::ut::assertTrue(res == "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6");
  // std::string sum = "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6";
  // if(res != sum) {
  //   std::cerr << "Hash are differents:" << std::endl;
  //   std::cerr << "AnCH result: " << res << std::endl;
  //   std::cerr << "Instead of:  " << sum << std::endl;
  //   return 1;

  // } else {
  //   std::cout << "Found value: " << res << std::endl << std::endl;
  // }
  std::cout << "Found value: " << res << std::endl << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH SHA2 unit tests")
    .description("Test AnCH SHA2 library")
    .add("sha2-224", testSHA224)
    .add("sha2-256", testSHA256)
    .add("sha2-384", testSHA384)
    .add("sha2-512", testSHA512)
    ;
}

// int
// main(int argc, char** argv) {

//   bool all = true;
//   if(argc == 2) {
//     all = false;
//   }

//   std::cout << "Enter in SHA2 tests" << std::endl;

//   if(all || strcmp("224", argv[1]) == 0) {
//     std::string msg = "The quick brown fox jumps over the lazy dog";
//     std::cout << "Hash SHA224 message: " << msg << std::endl;
//     SHA224 hash(msg);
//     std::ostringstream out;
//     out << hash;
//     std::string res = out.str();

//     std::string sum = "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525";
//     if(res != sum) {
//       std::cerr << "Hash are differents:" << std::endl;
//       std::cerr << "AnCH result: " << res << std::endl;
//       std::cerr << "Instead of:  " << sum << std::endl;
//       return 1;

//     } else {
//       std::cout << "Found value: " << res << std::endl << std::endl;
//     }
//   }

//   if(all || strcmp("256", argv[1]) == 0) {
//     std::string msg = "The quick brown fox jumps over the lazy dog";
//     std::cout << "Hash SHA256 message: " << msg << std::endl;
//     SHA256 hash(msg);
//     std::ostringstream out;
//     out << hash;
//     std::string res = out.str();

//     std::string sum = "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592";
//     if(res != sum) {
//       std::cerr << "Hash are differents:" << std::endl;
//       std::cerr << "AnCH result: " << res << std::endl;
//       std::cerr << "Instead of:  " << sum << std::endl;
//       return 1;

//     } else {
//       std::cout << "Found value: " << res << std::endl << std::endl;
//     }
//   }

//   if(all || strcmp("384", argv[1]) == 0) {
//     std::string msg = "The quick brown fox jumps over the lazy dog";
//     std::cout << "Hash SHA384 message: " << msg << std::endl;
//     SHA384 hash(msg);
//     std::ostringstream out;
//     out << hash;
//     std::string res = out.str();

//     std::string sum = "ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c494011e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1";
//     if(res != sum) {
//       std::cerr << "Hash are differents:" << std::endl;
//       std::cerr << "AnCH result: " << res << std::endl;
//       std::cerr << "Instead of:  " << sum << std::endl;
//       return 1;

//     } else {
//       std::cout << "Found value: " << res << std::endl << std::endl;
//     }
//   }

//   if(all || strcmp("512", argv[1]) == 0) {
//     std::string msg = "The quick brown fox jumps over the lazy dog";
//     std::cout << "Hash SHA512 message: " << msg << std::endl;
//     SHA512 hash(msg);
//     std::ostringstream out;
//     out << hash;
//     std::string res = out.str();

//     std::string sum = "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6";
//     if(res != sum) {
//       std::cerr << "Hash are differents:" << std::endl;
//       std::cerr << "AnCH result: " << res << std::endl;
//       std::cerr << "Instead of:  " << sum << std::endl;
//       return 1;

//     } else {
//       std::cout << "Found value: " << res << std::endl << std::endl;
//     }
//   }

//   std::cout << "Exit SHA2 tests" << std::endl;
//   return 0;
// }
