#include "uuid.hpp"

#include <iostream>

using anch::Uuid;

int
main(void) {
  Uuid mac;
  Uuid::generateUuid(mac, Uuid::Version::MAC_ADDRESS);
  Uuid md5;
  Uuid::generateUuid(md5, Uuid::Version::MD5_HASH, "Ta mère suce des bites en enfer !");
  Uuid random;
  Uuid::generateUuid(random);
  Uuid sha1;
  Uuid::generateUuid(sha1, Uuid::Version::SHA1_HASH, "Ta mère suce des bites en enfer !");
  std::cout << "MAC address UUID: " << mac << std::endl;
  std::cout << "MD5 hash UUID:    " << md5 << std::endl;
  std::cout << "Random UUID:      " << random << std::endl;
  std::cout << "SHA1 hash UUID:   " << sha1 << std::endl;
  std::string macStr = static_cast<std::string>(mac);
  std::cout << "UUID MAC address UUID conversion:   " << macStr << std::endl;
  std::cout << "UUID MAC address string conversion: " << Uuid(macStr) << std::endl;
  std::string md5Str = static_cast<std::string>(md5);
  std::cout << "UUID MD5 address UUID conversion:   " << md5Str << std::endl;
  std::cout << "UUID MD5 address string conversion: " << Uuid(md5Str) << std::endl;
  std::string randomStr = static_cast<std::string>(random);
  std::cout << "UUID RANDOM address UUID conversion:   " << randomStr << std::endl;
  std::cout << "UUID RANDOM address string conversion: " << Uuid(randomStr) << std::endl;
  std::string sha1Str = static_cast<std::string>(sha1);
  std::cout << "UUID SHA1 address UUID conversion:   " << sha1Str << std::endl;
  std::cout << "UUID SHA1 address string conversion: " << Uuid(sha1Str) << std::endl;
  return 0;
}
