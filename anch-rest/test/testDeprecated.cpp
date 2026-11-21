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
// Deprecated anch::rest codec names (removed in 0.4): this file is compiled without deprecation warnings to check
// that aliases still work, and with deprecation warnings as errors to check that they are reported
#include "rest/codec.hpp"
#include "rest/jsonCodec.hpp"
#include "rest/mediaType.hpp"

#include <sstream>
#include <string>
#include <vector>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

static_assert(anch::rest::Serializer<anch::rest::Json, int> && anch::rest::Deserializer<anch::rest::Text, std::string>);

void
testAliases() {
  anch::rest::MediaType type = anch::rest::contentTypeOf<anch::rest::Json>(42);
  anch::ut::assertThat(type.essence() == anch::rest::media::APP_JSON, "Unexpected media type '{}'", type.essence());
  std::ostringstream out;
  anch::rest::serializeBody<anch::rest::Json>(42, out, type, anch::rest::Side::SERVER);
  anch::ut::assertThat(out.str() == "42", "Unexpected JSON '{}'", out.str());
  std::istringstream input("hello");
  std::string value;
  anch::rest::deserializeBody<anch::rest::Text>(value, input, anch::rest::MediaType("text", "plain"), anch::rest::Side::CLIENT);
  anch::ut::assertThat(value == "hello", "Unexpected text '{}'", value);
  const std::vector<anch::rest::MediaType> available{anch::rest::MediaType("text", "plain")};
  anch::ut::assertThat(anch::rest::negotiate({}, available) == 0u, "Unexpected negotiation");
  std::istringstream large("0123456789");
  bool raised = false;
  try {
    anch::rest::readContent(large, 4);
  } catch(const anch::rest::ContentTooLarge&) {
    raised = true;
  }
  anch::ut::assertThat(raised, "ContentTooLarge expected");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH REST deprecated codec names")
    .description("Test anch::rest codec aliases (moved to anch-codec)")
    .add("aliases", testAliases)
    ;
}
