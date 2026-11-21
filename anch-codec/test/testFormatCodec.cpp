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
#include "codec/formatCodec.hpp"

#include <istream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::codec::Side;

// Formats +
namespace {

  // Format which writes strings with a prefix and refuses long inputs
  struct Prefixed {
    struct Options {
      std::string prefix = "> ";
      std::size_t maxSize = 0;
    };
    static constexpr std::string_view MEDIA_TYPE = "text/x-prefixed";

    static const Options&
    defaultOptions() noexcept {
      static const Options options;
      return options;
    }

    static void
    serialize(const std::string& value, std::ostream& out, const Options& options) {
      out << options.prefix << value;
    }

    static void
    deserialize(std::string& value, std::istream& input, const Options& options) {
      value.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
      if(options.maxSize != 0 && value.size() > options.maxSize) {
	throw std::length_error("too long");
      }
      if(value.starts_with(options.prefix)) {
	value.erase(0, options.prefix.size());
      }
    }
  };

  // Format with the default policy
  struct Plain {
    struct Options {
      bool upper = false;
    };
    static constexpr std::string_view MEDIA_TYPE = "text/x-plain";

    static Options
    defaultOptions() noexcept {
      return Options();
    }

    static void
    serialize(int value, std::ostream& out, const Options&) {
      out << value;
    }
  };

}

template<>
struct anch::codec::FormatPolicy<Prefixed> {
  static void
  configure(Prefixed::Options& options, Side side) {
    options.prefix = (side == Side::SERVER) ? "S> " : "C> ";
    options.maxSize = (side == Side::SERVER) ? 8 : 0;
  }

  static void
  translate(const std::exception& error) {
    if(dynamic_cast<const std::length_error*>(&error) != nullptr) {
      throw anch::codec::ContentTooLarge(error.what());
    }
  }
};

using PrefixedCodec = anch::codec::FormatCodec<Prefixed>;
using PlainCodec = anch::codec::FormatCodec<Plain>;
// Formats -

static_assert(anch::codec::DataFormat<Prefixed> && anch::codec::FormatSerializer<Prefixed, std::string>
	      && anch::codec::FormatDeserializer<Prefixed, std::string> && !anch::codec::FormatSerializer<Prefixed, int>);
static_assert(anch::codec::Serializer<PrefixedCodec, std::string> && anch::codec::Deserializer<PrefixedCodec, std::string>);
static_assert(anch::codec::Serializer<PlainCodec, int> && !anch::codec::Deserializer<PlainCodec, int>);
static_assert(!anch::codec::DataFormat<std::string>);

void
testOptions() {
  anch::ut::assertThat(PrefixedCodec::mediaType() == "text/x-prefixed", "Media type");
  anch::ut::assertThat(PrefixedCodec::options(Side::SERVER).prefix == "S> " && PrefixedCodec::options(Side::CLIENT).prefix == "C> ",
		       "Options configured by policy");
  std::ostringstream server;
  PrefixedCodec::serialize(std::string("a"), server, Side::SERVER);
  std::ostringstream defaults;
  PrefixedCodec::serialize(std::string("a"), defaults);
  anch::ut::assertThat(server.str() == "S> a" && defaults.str() == "> a", "Side and default options");
  // Options can be changed at initialization
  PrefixedCodec::options(Side::CLIENT).prefix = "c: ";
  std::ostringstream client;
  anch::codec::serialize<PrefixedCodec>(std::string("b"), client, anch::codec::contentTypeOf<PrefixedCodec>(std::string()), Side::CLIENT);
  anch::ut::assertThat(client.str() == "c: b", "Changed options '{}'", client.str());
  PrefixedCodec::options(Side::CLIENT).prefix = "C> ";
  // Default policy: default options for both sides
  anch::ut::assertThat(!PlainCodec::options(Side::SERVER).upper && !PlainCodec::options(Side::CLIENT).upper, "Default policy");
}

void
testTranslate() {
  std::string value;
  std::istringstream small("S> abc");
  PrefixedCodec::deserialize(value, small, Side::SERVER);
  anch::ut::assertThat(value == "abc", "Unexpected value '{}'", value);
  std::istringstream large("S> 0123456789");
  bool raised = false;
  try {
    PrefixedCodec::deserialize(value, large, Side::SERVER);
  } catch(const anch::codec::ContentTooLarge&) {
    raised = true;
  }
  anch::ut::assertThat(raised, "length_error has to be translated into ContentTooLarge");
  std::istringstream unbounded("C> 0123456789");
  PrefixedCodec::deserialize(value, unbounded, Side::CLIENT);
  anch::ut::assertThat(value == "0123456789", "No limit on client side");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH format codec unit tests")
    .description("Test anch::codec::FormatCodec on data formats")
    .add("options", testOptions)
    .add("translate", testTranslate)
    ;
}
