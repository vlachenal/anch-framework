#include "uuid.hpp"

#include <iostream>

#include "crypto/hash/md5.hpp"
#include "crypto/hash/sha1.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

void
testUUIDMD5() {
    anch::crypto::registerMD5UUIDProvider();
    anch::UUID md5 = anch::UUID::generateUUID(anch::UUID::Version::MD5_HASH, "Ta mère suce des bites en enfer !");
    std::cout << "MD5 hash UUID:    " << md5 << std::endl;
    std::string md5Str = static_cast<std::string>(md5);
    std::cout << "UUID MD5 address UUID conversion:   " << md5Str << std::endl;
    std::cout << "UUID MD5 address string conversion: " << anch::UUID(md5Str) << std::endl;
}

void
testUUIDSHA1() {
    anch::crypto::registerSHA1UUIDProvider();
    anch::UUID sha1 = anch::UUID::generateUUID(anch::UUID::Version::SHA1_HASH, "Ta mère suce des bites en enfer !");
    std::cout << "SHA1 hash UUID:   " << sha1 << std::endl;
    std::string sha1Str = static_cast<std::string>(sha1);
    std::cout << "UUID SHA1 address UUID conversion:   " << sha1Str << std::endl;
    std::cout << "UUID SHA1 address string conversion: " << anch::UUID(sha1Str) << std::endl;
}

/*!
 * Check name based UUID against reference value (computed with Python uuid module, RFC 9562)
 */
void
checkNameUUID(anch::UUID::Version version, const anch::UUID& ns, const std::string& name, const std::string& expected) {
  anch::UUID uuid = anch::UUID::generateUUID(version, ns, name);
  std::cout << name << " -> " << uuid << std::endl;
  anch::ut::assertThat(uuid.toString() == expected, "UUID for '{}': expected {} ; found {}", name, expected, uuid.toString());
  anch::ut::assertThat(uuid.getVersion() == version, "Bad version for '{}'", name);
}

/*!
 * Check version and RFC 9562 variant on many names
 */
void
checkVersionVariant(anch::UUID::Version version, char versionChar) {
  for(int i = 0 ; i < 10000 ; ++i) {
    std::string str = anch::UUID::generateUUID(version, anch::UUID::namespaceDNS(), "name" + std::to_string(i)).toString();
    anch::ut::assertThat(str[14] == versionChar, "Bad version in {}", str);
    anch::ut::assertThat(str[19] == '8' || str[19] == '9' || str[19] == 'a' || str[19] == 'b', "Bad variant in {}", str);
  }
}

void
testUUIDMD5RFC() {
  anch::crypto::registerMD5UUIDProvider();
  checkNameUUID(anch::UUID::Version::MD5_HASH, anch::UUID::namespaceDNS(), "www.example.com", "5df41881-3aed-3515-88a7-2f4a814cf09e"); // RFC 9562 A.2
  checkNameUUID(anch::UUID::Version::MD5_HASH, anch::UUID::namespaceURL(), "https://www.example.com/", "7fed185f-0864-319f-875b-a3d5458e30ac");
  checkNameUUID(anch::UUID::Version::MD5_HASH, anch::UUID::namespaceOID(), "1.3.6.1", "dd1a1cef-13d5-368a-ad82-eca71acd4cd1");
  checkNameUUID(anch::UUID::Version::MD5_HASH, anch::UUID::namespaceX500(), "cn=John Doe,o=Example", "1e2e71c2-4075-3b4d-82cf-6e0af31b0154");
  checkNameUUID(anch::UUID::Version::MD5_HASH, anch::UUID::namespaceDNS(), "caf\xC3\xA9.example", "773aaf32-e3d4-3a92-be2c-99fb1575905a");
  checkVersionVariant(anch::UUID::Version::MD5_HASH, '3');
}

void
testUUIDSHA1RFC() {
  anch::crypto::registerSHA1UUIDProvider();
  checkNameUUID(anch::UUID::Version::SHA1_HASH, anch::UUID::namespaceDNS(), "www.example.com", "2ed6657d-e927-568b-95e1-2665a8aea6a2"); // RFC 9562 A.4
  checkNameUUID(anch::UUID::Version::SHA1_HASH, anch::UUID::namespaceURL(), "https://www.example.com/", "3d3ed9d2-aa3d-5fa6-90e8-ed662e90f559");
  checkNameUUID(anch::UUID::Version::SHA1_HASH, anch::UUID::namespaceOID(), "1.3.6.1", "1447fa61-5277-5fef-a9b3-fbc6e44f4af3");
  checkNameUUID(anch::UUID::Version::SHA1_HASH, anch::UUID::namespaceX500(), "cn=John Doe,o=Example", "af514fe8-6655-5388-a197-79d1296fbf5a");
  checkNameUUID(anch::UUID::Version::SHA1_HASH, anch::UUID::namespaceDNS(), "caf\xC3\xA9.example", "1f25f992-3aeb-54f1-b196-ccca88f733b1");
  checkVersionVariant(anch::UUID::Version::SHA1_HASH, '5');
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH hash UUID unit tests")
    .description("Test AnCH hash UUID library")
    .add("MD5", testUUIDMD5)
    .add("SHA1", testUUIDSHA1)
    .add("MD5-RFC", testUUIDMD5RFC)
    .add("SHA1-RFC", testUUIDSHA1RFC)
    ;
}
