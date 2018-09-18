#include "uuid.hpp"

#include <iostream>

#include "device/uuidProvider.hpp"
#include "crypto/hash/md5.hpp"
#include "crypto/hash/sha1.hpp"

using anch::UUID;

int
main(void) {
  anch::device::registerMACAddressUUIDProvider();
  anch::crypto::registerMD5UUIDProvider();
  anch::crypto::registerSHA1UUIDProvider();

  UUID mac = UUID::generateUUID(UUID::Version::MAC_ADDRESS);
  std::cout << "MAC address UUID: " << mac << std::endl;
  UUID md5 = UUID::generateUUID(UUID::Version::MD5_HASH, "Ta mère suce des bites en enfer !");
  std::cout << "MD5 hash UUID:    " << md5 << std::endl;
  UUID random = UUID::random();
  std::cout << "Random UUID:      " << random << std::endl;
  UUID sha1 = UUID::generateUUID(UUID::Version::SHA1_HASH, "Ta mère suce des bites en enfer !");
  std::cout << "SHA1 hash UUID:   " << sha1 << std::endl;
  std::string macStr = static_cast<std::string>(mac);
  std::cout << "UUID MAC address UUID conversion:   " << macStr << std::endl;
  std::cout << "UUID MAC address string conversion: " << UUID(macStr) << std::endl;
  std::string md5Str = static_cast<std::string>(md5);
  std::cout << "UUID MD5 address UUID conversion:   " << md5Str << std::endl;
  std::cout << "UUID MD5 address string conversion: " << UUID(md5Str) << std::endl;
  std::string randomStr = static_cast<std::string>(random);
  std::cout << "UUID RANDOM address UUID conversion:   " << randomStr << std::endl;
  std::cout << "UUID RANDOM address string conversion: " << UUID(randomStr) << std::endl;
  std::string sha1Str = static_cast<std::string>(sha1);
  std::cout << "UUID SHA1 address UUID conversion:   " << sha1Str << std::endl;
  std::cout << "UUID SHA1 address string conversion: " << UUID(sha1Str) << std::endl;

  std::string test = "26c280b6-c607-4134-a01b-2468357fd51e";
  std::cout << "test: " << test << std::endl;
  std::cout << "UUID: " << UUID(test) << std::endl;

  return 0;
}
