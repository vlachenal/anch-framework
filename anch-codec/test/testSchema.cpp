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
// Format plugins with an external schema (Protobuf, Thrift): codecs, selection in anch::codec::Available and
// registered formats. Each plugin is tested when it is built (ANCH_CODEC_*_ENABLED from codec/all.hpp).
#include "codec/all.hpp"
#include "codec/format.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#ifdef ANCH_CODEC_PROTOBUF_ENABLED
#include "pet.pb.h"
#endif
#if defined(ANCH_CODEC_THRIFT_ENABLED) && !defined(ANCH_CODEC_TEST_NO_THRIFT_COMPILER)
#include "pet_types.h"
#endif

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::codec::Available;
using anch::codec::MediaType;
using anch::codec::Side;

namespace {

  bool
  registered(const std::string& name) {
    const std::vector<anch::codec::Format> formats = anch::codec::formats();
    return std::any_of(formats.begin(), formats.end(), [&name](const anch::codec::Format& format) { return format.name == name; });
  }

}

#ifdef ANCH_CODEC_PROTOBUF_ENABLED
using ProtoPet = anch::test::protobuf::Pet;
// Json does not handle Protobuf messages: Available selects Protobuf codecs
static_assert(!anch::codec::Serializer<anch::codec::Json, ProtoPet> && anch::codec::Serializer<anch::codec::ProtobufJson, ProtoPet>);
static_assert(Available::SERIALIZES<ProtoPet> && Available::DESERIALIZES<ProtoPet>);

void
testProtobuf() {
  ProtoPet pet;
  pet.set_id(3);
  pet.set_name("Rex");
  // Binary
  std::ostringstream binary;
  auto type = Available::serialize(pet, binary, MediaType::parseAccept("application/x-protobuf"), Side::CLIENT);
  anch::ut::assertThat(type && type->essence() == "application/x-protobuf", "Protobuf codec expected");
  ProtoPet read;
  std::istringstream binaryInput(binary.str());
  anch::ut::assertThat(Available::deserialize(read, binaryInput, *type, Side::SERVER) && read.id() == 3 && read.name() == "Rex",
		       "Unexpected binary pet");
  // JSON (ProtobufJson, not Json)
  std::ostringstream json;
  type = Available::serialize(pet, json, MediaType::parseAccept("application/json"), Side::SERVER);
  anch::ut::assertThat(type && type->essence() == "application/json" && json.str() == R"({"id":"3","name":"Rex"})",
		       "Unexpected Protobuf JSON '{}'", json.str());
  ProtoPet fromJson;
  std::istringstream jsonInput(R"({"id":"4","name":"Médor"})");
  anch::ut::assertThat(Available::deserialize(fromJson, jsonInput, MediaType("application", "json"), Side::SERVER)
		       && fromJson.id() == 4 && fromJson.name() == "Médor", "Unexpected JSON pet");
  // Without Accept: first codec which serializes Protobuf messages
  std::ostringstream first;
  type = Available::serialize(pet, first, {}, Side::SERVER);
  anch::ut::assertThat(type && type->essence() == "application/x-protobuf", "Protobuf codec expected without Accept");
  anch::ut::assertThat(registered("protobuf") && registered("protobuf-json"), "protobuf formats have to be registered");
}
#endif

#if defined(ANCH_CODEC_THRIFT_ENABLED) && !defined(ANCH_CODEC_TEST_NO_THRIFT_COMPILER)
using ThriftPet = anch::test::thrift::Pet;
static_assert(!anch::codec::Serializer<anch::codec::Json, ThriftPet> && Available::SERIALIZES<ThriftPet>
	      && Available::DESERIALIZES<ThriftPet>);

void
testThrift() {
  ThriftPet pet;
  pet.id = 5;
  pet.name = "Rex";
  for(anch::codec::ThriftProtocol protocol : {anch::codec::ThriftProtocol::BINARY, anch::codec::ThriftProtocol::COMPACT,
					    anch::codec::ThriftProtocol::JSON}) {
    const std::string_view mediaType = anch::codec::thriftMediaType(protocol);
    std::ostringstream out;
    auto type = Available::serialize(pet, out, MediaType::parseAccept(mediaType), Side::CLIENT);
    anch::ut::assertThat(type && type->essence() == mediaType, "{} codec expected", mediaType);
    ThriftPet read;
    std::istringstream input(out.str());
    anch::ut::assertThat(Available::deserialize(read, input, *type, Side::SERVER) && read.id == 5 && read.name == "Rex",
			 "Unexpected {} pet", mediaType);
  }
  ThriftPet invalid;
  // Truncated structure: string field 2 without its length. Field types are valid ones: libthrift converts the read
  // byte to TType without check (undefined behavior reported by UBSan for out of range values such as 0xff)
  std::istringstream garbage(std::string("\x0b\x00\x02\x00", 4));
  bool raised = false;
  try {
    Available::deserialize(invalid, garbage, MediaType("application", "vnd.apache.thrift.binary"), Side::SERVER);
  } catch(const std::invalid_argument&) {
    raised = true;
  }
  anch::ut::assertThat(raised, "Invalid Thrift structure has to raise std::invalid_argument");
  anch::ut::assertThat(registered("thrift"), "thrift format has to be registered");
}
#endif

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH codec schema plugins unit tests")
    .description("Test AnCH codec Protobuf and Thrift plugins")
#ifdef ANCH_CODEC_PROTOBUF_ENABLED
    .add("protobuf", testProtobuf)
#endif
#if defined(ANCH_CODEC_THRIFT_ENABLED) && !defined(ANCH_CODEC_TEST_NO_THRIFT_COMPILER)
    .add("thrift", testThrift)
#endif
    ;
}
