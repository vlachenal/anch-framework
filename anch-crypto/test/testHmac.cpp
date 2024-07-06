#include "crypto/hash/md5.hpp"
#include "crypto/hmac.hpp"
#include "crypto/hash/sha1.hpp"
#include "crypto/hash/sha224.hpp"
#include "crypto/hash/sha256.hpp"
#include "crypto/hash/sha384.hpp"
#include "crypto/hash/sha512.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::crypto::MD5;
using anch::crypto::SHA1;
using anch::crypto::SHA224;
using anch::crypto::SHA256;
using anch::crypto::SHA384;
using anch::crypto::SHA512;
using anch::crypto::HMAC;

void
testHmacMD5() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in HMAC-MD5 test" << std::endl;
  std::ostringstream out;
  start = std::chrono::high_resolution_clock::now();
  out << HMAC<MD5>("key", "The quick brown fox jumps over the lazy dog");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  std::string res = out.str();
  anch::ut::assertTrue(res == "80070713463e7749b90c2dc24911e275");
  // if(res != "80070713463e7749b90c2dc24911e275") {
  //   std::cerr << "Found      " << res << std::endl;
  //   std::cerr << "Instead of 80070713463e7749b90c2dc24911e275" << std::endl;
  //   return 1;
  // }
  std::cout << "Found HMAC-MD5 " << res << std::endl;
  std::cout << "HMAC-MD5 duration: " << duration.count() << "µs" << std::endl;
  std::cout << "Exit HMAC-MD5 test" << std::endl;
}

void
testHmacSHA1() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in HMAC-SHA1 test" << std::endl;
  std::ostringstream out;
  start = std::chrono::high_resolution_clock::now();
  out << HMAC<SHA1>("key", "The quick brown fox jumps over the lazy dog");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  std::string res = out.str();
  anch::ut::assertTrue(res == "de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9");
  // if(res != "de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9") {
  //   std::cerr << "Found      " << res << std::endl;
  //   std::cerr << "Instead of de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9" << std::endl;
  //   return 1;
  // }
  std::cout << "Found HMAC-SHA1 " << res << std::endl;
  std::cout << "HMAC-SHA1 duration: " << duration.count() << "µs" << std::endl;
  std::cout << "Exit HMAC-SHA1 test" << std::endl;
}

void
testHmacSHA224() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in HMAC-SHA224 test" << std::endl;
  std::ostringstream out;
  start = std::chrono::high_resolution_clock::now();
  out << HMAC<SHA224>("key", "The quick brown fox jumps over the lazy dog");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  std::string res = out.str();
  anch::ut::assertTrue(res == "88ff8b54675d39b8f72322e65ff945c52d96379988ada25639747e69");
  // if(res != "88ff8b54675d39b8f72322e65ff945c52d96379988ada25639747e69") {
  //   std::cerr << "Found      " << res << std::endl;
  //   std::cerr << "Instead of 88ff8b54675d39b8f72322e65ff945c52d96379988ada25639747e69" << std::endl;
  //   return 1;
  // }
  std::cout << "Found HMAC-SHA224 " << res << std::endl;
  std::cout << "HMAC-SHA224 duration: " << duration.count() << "µs" << std::endl;
  std::cout << "Exit HMAC-SHA224 test" << std::endl;
}

void
testHmacSHA256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in HMAC-SHA256 test" << std::endl;
  std::ostringstream out;
  start = std::chrono::high_resolution_clock::now();
  out << HMAC<SHA256>("key", "The quick brown fox jumps over the lazy dog");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  std::string res = out.str();
  anch::ut::assertTrue(res == "f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8");
  // if(res != "f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8") {
  //   std::cerr << "Found      " << res << std::endl;
  //   std::cerr << "Instead of f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8" << std::endl;
  //   return 1;
  // }
  std::cout << "Found HMAC-SHA256 " << res << std::endl;
  std::cout << "HMAC-SHA256 duration: " << duration.count() << "µs" << std::endl;
  std::cout << "Exit HMAC-SHA256 test" << std::endl;
}

void
testHmacSHA384() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in HMAC-SHA384 test" << std::endl;
  std::ostringstream out;
  start = std::chrono::high_resolution_clock::now();
  out << HMAC<SHA384>("key", "The quick brown fox jumps over the lazy dog");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  std::string res = out.str();
  anch::ut::assertTrue(res == "d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237");
  // if(res != "d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237") {
  //   std::cerr << "Found      " << res << std::endl;
  //   std::cerr << "Instead of d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237" << std::endl;
  //   return 1;
  // }
  std::cout << "Found HMAC-SHA384 " << res << std::endl;
  std::cout << "HMAC-SHA384 duration: " << duration.count() << "µs" << std::endl;
  std::cout << "Exit HMAC-SHA384 test" << std::endl;
}

void
testHmacSHA512() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in HMAC-SHA512 test" << std::endl;
  std::ostringstream out;
  start = std::chrono::high_resolution_clock::now();
  out << HMAC<SHA512>("key", "The quick brown fox jumps over the lazy dog");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  std::string res = out.str();
  anch::ut::assertTrue(res == "b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a");
  // if(res != "b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a") {
  //   std::cerr << "Found      " << res << std::endl;
  //   std::cerr << "Instead of b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a" << std::endl;
  //   return 1;
  // }
  std::cout << "Found HMAC-SHA512 " << res << std::endl;
  std::cout << "HMAC-SHA512 duration: " << duration.count() << "µs" << std::endl;
  std::cout << "Exit HMAC-SHA512 test" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH HMAC unit tests")
    .description("Test AnCH HMAC library")
    .add("hmac-md5", testHmacMD5)
    .add("hmac-sha1", testHmacSHA1)
    .add("hmac-sha224", testHmacSHA224)
    .add("hmac-sha256", testHmacSHA256)
    .add("hmac-sha384", testHmacSHA384)
    .add("hmac-sha512", testHmacSHA512)
    ;
}

// int
// main(void) {
//   std::cout << "Enter in HMAC tests" << std::endl;

//   std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
//   std::chrono::microseconds duration;

  // {
  //   std::cout << "Enter in HMAC-MD5 test" << std::endl;
  //   std::ostringstream out;
  //   start = std::chrono::high_resolution_clock::now();
  //   out << HMAC<MD5>("key", "The quick brown fox jumps over the lazy dog");
  //   end = std::chrono::high_resolution_clock::now();
  //   duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  //   std::string res = out.str();
  //   if(res != "80070713463e7749b90c2dc24911e275") {
  //     std::cerr << "Found      " << res << std::endl;
  //     std::cerr << "Instead of 80070713463e7749b90c2dc24911e275" << std::endl;
  //     return 1;
  //   }
  //   std::cout << "Found HMAC-MD5 " << res << std::endl;
  //   std::cout << "HMAC-MD5 duration: " << duration.count() << "µs" << std::endl;
  //   std::cout << "Exit HMAC-MD5 test" << std::endl;
  // }

  // {
  //   std::cout << "Enter in HMAC-SHA1 test" << std::endl;
  //   std::ostringstream out;
  //   start = std::chrono::high_resolution_clock::now();
  //   out << HMAC<SHA1>("key", "The quick brown fox jumps over the lazy dog");
  //   end = std::chrono::high_resolution_clock::now();
  //   duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  //   std::string res = out.str();
  //   if(res != "de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9") {
  //     std::cerr << "Found      " << res << std::endl;
  //     std::cerr << "Instead of de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9" << std::endl;
  //     return 1;
  //   }
  //   std::cout << "Found HMAC-SHA1 " << res << std::endl;
  //   std::cout << "HMAC-SHA1 duration: " << duration.count() << "µs" << std::endl;
  //   std::cout << "Exit HMAC-SHA1 test" << std::endl;
  // }

  // {
  //   std::cout << "Enter in HMAC-SHA224 test" << std::endl;
  //   std::ostringstream out;
  //   start = std::chrono::high_resolution_clock::now();
  //   out << HMAC<SHA224>("key", "The quick brown fox jumps over the lazy dog");
  //   end = std::chrono::high_resolution_clock::now();
  //   duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  //   std::string res = out.str();
  //   if(res != "88ff8b54675d39b8f72322e65ff945c52d96379988ada25639747e69") {
  //     std::cerr << "Found      " << res << std::endl;
  //     std::cerr << "Instead of 88ff8b54675d39b8f72322e65ff945c52d96379988ada25639747e69" << std::endl;
  //     return 1;
  //   }
  //   std::cout << "Found HMAC-SHA224 " << res << std::endl;
  //   std::cout << "HMAC-SHA224 duration: " << duration.count() << "µs" << std::endl;
  //   std::cout << "Exit HMAC-SHA224 test" << std::endl;
  // }

  // {
  //   std::cout << "Enter in HMAC-SHA256 test" << std::endl;
  //   std::ostringstream out;
  //   start = std::chrono::high_resolution_clock::now();
  //   out << HMAC<SHA256>("key", "The quick brown fox jumps over the lazy dog");
  //   end = std::chrono::high_resolution_clock::now();
  //   duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  //   std::string res = out.str();
  //   if(res != "f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8") {
  //     std::cerr << "Found      " << res << std::endl;
  //     std::cerr << "Instead of f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8" << std::endl;
  //     return 1;
  //   }
  //   std::cout << "Found HMAC-SHA256 " << res << std::endl;
  //   std::cout << "HMAC-SHA256 duration: " << duration.count() << "µs" << std::endl;
  //   std::cout << "Exit HMAC-SHA256 test" << std::endl;
  // }

  // {
  //   std::cout << "Enter in HMAC-SHA384 test" << std::endl;
  //   std::ostringstream out;
  //   start = std::chrono::high_resolution_clock::now();
  //   out << HMAC<SHA384>("key", "The quick brown fox jumps over the lazy dog");
  //   end = std::chrono::high_resolution_clock::now();
  //   duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  //   std::string res = out.str();
  //   if(res != "d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237") {
  //     std::cerr << "Found      " << res << std::endl;
  //     std::cerr << "Instead of d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237" << std::endl;
  //     return 1;
  //   }
  //   std::cout << "Found HMAC-SHA384 " << res << std::endl;
  //   std::cout << "HMAC-SHA384 duration: " << duration.count() << "µs" << std::endl;
  //   std::cout << "Exit HMAC-SHA384 test" << std::endl;
  // }

//   {
//     std::cout << "Enter in HMAC-SHA512 test" << std::endl;
//     std::ostringstream out;
//     start = std::chrono::high_resolution_clock::now();
//     out << HMAC<SHA512>("key", "The quick brown fox jumps over the lazy dog");
//     end = std::chrono::high_resolution_clock::now();
//     duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
//     std::string res = out.str();
//     if(res != "b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a") {
//       std::cerr << "Found      " << res << std::endl;
//       std::cerr << "Instead of b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a" << std::endl;
//       return 1;
//     }
//     std::cout << "Found HMAC-SHA512 " << res << std::endl;
//     std::cout << "HMAC-SHA512 duration: " << duration.count() << "µs" << std::endl;
//     std::cout << "Exit HMAC-SHA512 test" << std::endl;
//   }

//   std::cout << "Exit HMAC tests" << std::endl;
//   return 0;
// }
