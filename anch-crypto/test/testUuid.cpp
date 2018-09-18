#include "uuid.hpp"

#include <iostream>

#include "crypto/hash/md5.hpp"
#include "crypto/hash/sha1.hpp"

int
main(int argc, char** argv) {
  bool all = true;
  if(argc == 2) {
    all = false;
  }

  if(all || strcmp("MD5", argv[1]) == 0) {
    anch::crypto::registerMD5UUIDProvider();
    anch::UUID md5 = anch::UUID::generateUUID(anch::UUID::Version::MD5_HASH, "Ta mère suce des bites en enfer !");
    std::cout << "MD5 hash UUID:    " << md5 << std::endl;
    std::string md5Str = static_cast<std::string>(md5);
    std::cout << "UUID MD5 address UUID conversion:   " << md5Str << std::endl;
    std::cout << "UUID MD5 address string conversion: " << anch::UUID(md5Str) << std::endl;
  }

  if(all || strcmp("SHA1", argv[1]) == 0) {
    anch::crypto::registerSHA1UUIDProvider();
    anch::UUID sha1 = anch::UUID::generateUUID(anch::UUID::Version::SHA1_HASH, "Ta mère suce des bites en enfer !");
    std::cout << "SHA1 hash UUID:   " << sha1 << std::endl;
    std::string sha1Str = static_cast<std::string>(sha1);
    std::cout << "UUID SHA1 address UUID conversion:   " << sha1Str << std::endl;
    std::cout << "UUID SHA1 address string conversion: " << anch::UUID(sha1Str) << std::endl;
  }

  return 0;
}
