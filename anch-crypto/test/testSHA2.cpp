/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2013 Vincent Lachenal

  This file is part of ANCH Framework.

  ANCH Framework is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ANCH Framework is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "crypto/hash/sha224.hpp"
#include "crypto/hash/sha256.hpp"
#include "crypto/hash/sha384.hpp"
#include "crypto/hash/sha512.hpp"
#include "crypto/hash/md5.hpp"
#include "crypto/hash/sha1.hpp"

#include <iomanip>
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

/*!
 * Hexadecimal form of a digest
 */
template<std::size_t O>
std::string
hex(const std::array<uint8_t,O>& digest) {
  std::ostringstream out;
  for(uint8_t byte : digest) {
    out << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(byte);
  }
  return out.str();
}

/*!
 * Check data hashing methods of a hash class (they were hidden by digest() override)
 */
template<typename H>
void
checkDigestOverloads(const std::string& expected, const std::string& expectedEmpty) {
  const std::string msg = "The quick brown fox jumps over the lazy dog";
  H hash;
  anch::ut::assertThat(hex(hash.digest(msg)) == expected, "digest(std::string): {} instead of {}", hex(hash.digest()), expected);
  // The same object is reset before each computation
  anch::ut::assertThat(hex(hash.digest(std::string())) == expectedEmpty, "digest(\"\"): {} instead of {}", hex(hash.digest()), expectedEmpty);
  anch::ut::assertThat(hex(hash.digest(reinterpret_cast<const uint8_t*>(msg.data()), msg.size())) == expected,
		       "digest(const uint8_t*, std::size_t): {} instead of {}", hex(hash.digest()), expected);
  std::istringstream stream(msg);
  anch::ut::assertThat(hex(hash.digest(stream)) == expected, "digest(std::istream&): {} instead of {}", hex(hash.digest()), expected);
}

void
testDigestOverloads() {
  checkDigestOverloads<anch::crypto::MD5>("9e107d9d372bb6826bd81d3542a419d6", "d41d8cd98f00b204e9800998ecf8427e");
  checkDigestOverloads<anch::crypto::SHA1>("2fd4e1c67a2d28fced849ee1bb76e7391b93eb12", "da39a3ee5e6b4b0d3255bfef95601890afd80709");
  checkDigestOverloads<SHA256>("d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592",
			       "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
  checkDigestOverloads<anch::crypto::SHA512>("07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6",
					     "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
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
    .add("hash-digest-overloads", testDigestOverloads)
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
