/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2012 Vincent Lachenal

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
//! [example]
#include "uuid.hpp"
#include "crypto/hash/sha1.hpp"

#include <iostream>

int
main() {
  anch::UUID random = anch::UUID::random();                     // version 4
  std::cout << random << std::endl;

  anch::UUID parsed("550e8400-e29b-41d4-a716-446655440000");    // std::invalid_argument when invalid
  std::cout << parsed.toString() << " version " << static_cast<int>(parsed.getVersion()) << std::endl;

  anch::crypto::registerSHA1UUIDProvider();                     // version 5 provider (anch-crypto)
  anch::UUID named = anch::UUID::generateUUID(anch::UUID::Version::SHA1_HASH, "anch-framework");
  std::cout << named << " " << (named == anch::UUID::generateUUID(anch::UUID::Version::SHA1_HASH, "anch-framework")) << std::endl;
  return 0;
}
//! [example]
