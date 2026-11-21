/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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
/*
 * Consumer of an installed AnCH Framework: headers and libraries are found with find_package(Anch).
 */
#include "date/dateFormatter.hpp"
#include "json/json.hpp"
#include "log/loggerFactory.hpp"
#include "uuid.hpp"
#ifdef ANCH_TEST_CRYPTO
#include "crypto/hash/sha256.hpp"
#endif
#ifdef ANCH_TEST_REST
#include "codec/json.hpp"

#include "rest/api.hpp"
#endif

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

struct Pet {
  anch::UUID id;
  std::string name;
};

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Pet>& mapper) {
  mapper
    .registerField("id", &Pet::id)
    .registerField("name", &Pet::name, anch::json::required)
    ;
}

namespace {

  int failures = 0;

  void
  check(bool condition, const std::string& message) {
    std::cout << (condition ? "OK   " : "FAIL ") << message << std::endl;
    if(!condition) {
      ++failures;
    }
  }

}

int
main() {
  // anch-json and anch-uuid
  const Pet pet{anch::UUID("4a1b5c2d-3e4f-4a5b-8c6d-7e8f9a0b1c2d"), "Rex"};
  const std::string json = anch::json::serialize(pet, anch::json::DEFAULT_MAPPING_OPTIONS);
  check(json == R"({"id":"4a1b5c2d-3e4f-4a5b-8c6d-7e8f9a0b1c2d","name":"Rex"})", "anch-json and anch-uuid: " + json);
  std::istringstream in(json);
  Pet copy = anch::json::deserialize<Pet>(in, anch::json::DEFAULT_MAPPING_OPTIONS);
  check(copy.id == pet.id && copy.name == pet.name, "anch-json deserialization");
  // anch-date
  anch::date::DateFormatter formatter("%Y-%m-%d");
  std::string date;
  formatter.format(formatter.parse("2026-11-16"), date);
  check(date == "2026-11-16", "anch-date: " + date);
  // anch-log (and anch-conf: default configuration when no file)
  anch::log::LoggerFactory::getLogger("install-test").info("anch-log is working");
  check(true, "anch-log");
#ifdef ANCH_TEST_CRYPTO
  // anch-crypto
  const anch::crypto::SHA256 sha(std::string("abc"));
  const auto& digest = sha.digest();
  check(digest[0] == 0xba && digest[31] == 0xad, "anch-crypto SHA-256");
#endif
#ifdef ANCH_TEST_REST
  // anch-rest-server
  anch::rest::Api api;
  api.get("/pets/{id}", [&pet](const anch::UUID&) { return pet; }).produces<anch::codec::Json>();
  anch::rest::Request request(anch::rest::Method::GET, "/pets/4a1b5c2d-3e4f-4a5b-8c6d-7e8f9a0b1c2d");
  const anch::rest::Response response = api.handle(request);
  check(anch::rest::code(response.getStatus()) == 200 && response.content() == json, "anch-rest-server: " + response.content());
#endif
  std::cout << failures << " failure(s)" << std::endl;
  return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
