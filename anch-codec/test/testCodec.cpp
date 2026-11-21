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
#include "codec/codec.hpp"
#include "codec/codecs.hpp"
#include "codec/format.hpp"
#include "codec/mediaType.hpp"
#include "codec/text.hpp"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::codec::MediaType;
using anch::codec::Side;
using anch::codec::Text;

// Codecs +
// Codec with every serialize / deserialize form, and media type parameters (as a multipart codec)
struct Bounded {
  static std::string_view
  mediaType() noexcept {
    return "multipart/x-test";
  }

  static MediaType
  contentType(const std::string& value) {
    MediaType type("multipart", "x-test");
    type.setParameter("boundary", "b" + std::to_string(value.size()));
    return type;
  }

  static void
  serialize(const std::string& value, std::ostream& out, const MediaType& contentType, Side side) {
    out << contentType.getParameter("boundary").value_or("") << ' ' << (side == Side::SERVER ? "server" : "client") << ' ' << value;
  }

  static void
  deserialize(std::string& value, std::istream& input, const MediaType& contentType, Side side) {
    value = std::string(contentType.getParameter("boundary").value_or("")) + ' ' + (side == Side::SERVER ? "server" : "client") + ' '
      + std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
  }
};
static_assert(anch::codec::Serializer<Bounded, std::string> && anch::codec::Deserializer<Bounded, std::string>);
static_assert(!anch::codec::Serializer<Bounded, int> && !anch::codec::Deserializer<Bounded, int>);

// Codec with side forms only
struct Upper {
  static std::string_view
  mediaType() noexcept {
    return "text/x-upper";
  }

  static void
  serialize(const std::string& value, std::ostream& out, Side side) {
    std::string res(value);
    std::transform(res.begin(), res.end(), res.begin(), [](char c) { return static_cast<char>(c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c); });
    out << res << (side == Side::SERVER ? "!" : "?");
  }
};
static_assert(anch::codec::Serializer<Upper, std::string> && !anch::codec::Deserializer<Upper, std::string>);
static_assert(anch::codec::Serializer<Text, int> && !anch::codec::Deserializer<Text, int>);
// Codecs -

void
testMediaType() {
  auto json = MediaType::parse("Application/JSON ; charset=\"UTF-8\"; foo=bar");
  anch::ut::assertThat(json.has_value(), "Media type has to be parsed");
  anch::ut::assertThat(json->essence() == "application/json", "Unexpected essence '{}'", json->essence());
  anch::ut::assertThat(json->getParameter("charset") == "UTF-8", "Unexpected charset");
  anch::ut::assertThat(json->toString() == "application/json;charset=UTF-8;foo=bar", "Unexpected format '{}'", json->toString());
  anch::ut::assertThat(!MediaType::parse("application").has_value(), "Media type without subtype is invalid");
  anch::ut::assertThat(!MediaType::parse("*/json").has_value(), "*/json is invalid");
  anch::ut::assertThat(!MediaType::parse("text/plain; charset").has_value(), "Parameter without value is invalid");
  anch::ut::assertThat(MediaType("*", "*").includes(*json) && MediaType("application", "*").includes(*json)
		       && !MediaType("text", "*").includes(*json), "Unexpected wildcard inclusion");
}

void
testAccept() {
  auto accept = MediaType::parseAccept("text/*;q=0.3, text/html;q=0.7, text/html;level=1, invalid, */*;q=0.5, application/xml;q=2");
  anch::ut::assertThat(accept.size() == 4, "Expected 4 media ranges, found {}", accept.size());
  anch::ut::assertThat(accept[0].toString() == "text/html;level=1" && accept[1].essence() == "text/html"
		       && accept[2].essence() == "*/*" && accept[3].essence() == "text/*", "Media ranges have to be sorted by quality");
  std::vector<MediaType> available{MediaType("application", "json"), MediaType("text", "plain"), MediaType("text", "html")};
  // Most specific range defines quality: text/html 0.7 > */* 0.5 > text/* 0.3
  anch::ut::assertThat(anch::codec::negotiate(MediaType::parseAccept("text/*;q=0.3, text/html;q=0.7, */*;q=0.5"), available) == 2u, "text/html expected");
  anch::ut::assertThat(anch::codec::negotiate({}, available) == 0u, "First media type expected without Accept");
  anch::ut::assertThat(anch::codec::negotiate(MediaType::parseAccept("*/*, application/json;q=0"), available) == 1u, "q=0 has to exclude application/json");
  anch::ut::assertThat(!anch::codec::negotiate(MediaType::parseAccept("image/png"), available).has_value(), "No media type expected");
}

void
testDispatch() {
  // Richest form is used, with contentType()
  const MediaType type = anch::codec::contentTypeOf<Bounded>(std::string("abc"));
  anch::ut::assertThat(type.toString() == "multipart/x-test;boundary=b3", "Unexpected content type '{}'", type.toString());
  std::ostringstream out;
  anch::codec::serialize<Bounded>(std::string("abc"), out, type, Side::CLIENT);
  anch::ut::assertThat(out.str() == "b3 client abc", "Unexpected serialization '{}'", out.str());
  std::istringstream input("xyz");
  std::string value;
  anch::codec::deserialize<Bounded>(value, input, type, Side::SERVER);
  anch::ut::assertThat(value == "b3 server xyz", "Unexpected deserialization '{}'", value);
  // Side form, mediaType() as content type
  const MediaType upper = anch::codec::contentTypeOf<Upper>(std::string("abc"));
  anch::ut::assertThat(upper.toString() == "text/x-upper", "Unexpected content type '{}'", upper.toString());
  std::ostringstream upperOut;
  anch::codec::serialize<Upper>(std::string("abc"), upperOut, upper, Side::SERVER);
  anch::ut::assertThat(upperOut.str() == "ABC!", "Unexpected serialization '{}'", upperOut.str());
}

void
testLimits() {
  std::istringstream small("0123456789");
  anch::ut::assertThat(anch::codec::readContent(small, 10) == "0123456789", "Content of maximum size has to be read");
  std::istringstream large("0123456789");
  bool raised = false;
  try {
    anch::codec::readContent(large, 9);
  } catch(const anch::codec::ContentTooLarge&) {
    raised = true;
  }
  anch::ut::assertThat(raised, "ContentTooLarge expected");
  anch::ut::assertThat(anch::codec::maxBodySize(Side::SERVER) == 2 * 1024 * 1024 && anch::codec::maxBodySize(Side::CLIENT) == 0,
		       "Unexpected default limits");
  const std::size_t previous = anch::codec::maxBodySize(Side::SERVER);
  anch::codec::maxBodySize(Side::SERVER) = 4;
  std::istringstream body("12345");
  raised = false;
  try {
    anch::codec::readBody(body, Side::SERVER);
  } catch(const anch::codec::ContentTooLarge&) {
    raised = true;
  }
  anch::codec::maxBodySize(Side::SERVER) = previous;
  anch::ut::assertThat(raised, "Server limit has to be applied");
  std::istringstream unbounded("12345");
  anch::ut::assertThat(anch::codec::readBody(unbounded, Side::CLIENT) == "12345", "Client has no default limit");
}

void
testText() {
  std::ostringstream out;
  anch::codec::serialize<Text>(42, out, anch::codec::contentTypeOf<Text>(42), Side::SERVER);
  anch::ut::assertThat(out.str() == "42", "Unexpected text '{}'", out.str());
  std::istringstream input("hello world");
  std::string value;
  anch::codec::deserialize<Text>(value, input, MediaType("text", "plain"), Side::CLIENT);
  anch::ut::assertThat(value == "hello world", "Unexpected text '{}'", value);
}

void
testCodecs() {
  using List = anch::codec::Codecs<Upper, Bounded, Text>;
  static_assert(List::SERIALIZES<std::string> && List::DESERIALIZES<std::string> && List::SERIALIZES<int>
		&& !List::DESERIALIZES<int>);
  const std::vector<MediaType> types = List::mediaTypes();
  anch::ut::assertThat(types.size() == 3 && types[0].essence() == "text/x-upper" && types[2].essence() == "text/plain",
		       "Unexpected media types");
  // Reading: codec selected by content type essence, parameters given to codec
  std::string value;
  std::istringstream input("data");
  auto contentType = MediaType::parse("Multipart/X-Test; boundary=srv");
  anch::ut::assertThat(List::deserialize(value, input, *contentType, Side::CLIENT) && value == "srv client data",
		       "Unexpected deserialization '{}'", value);
  std::istringstream textInput("plain");
  anch::ut::assertThat(List::deserialize(value, textInput, MediaType("text", "plain"), Side::SERVER) && value == "plain",
		       "Text codec expected");
  std::istringstream unknown("data");
  value = "untouched";
  anch::ut::assertThat(!List::deserialize(value, unknown, MediaType("text", "x-upper"), Side::SERVER) && value == "untouched",
		       "Upper codec does not deserialize");
  // Writing: negotiation among codecs which serialize the value
  std::ostringstream first;
  auto selected = List::serialize(std::string("abc"), first, {}, Side::SERVER);
  anch::ut::assertThat(selected && selected->essence() == "text/x-upper" && first.str() == "ABC!", "First codec expected without Accept");
  std::ostringstream bounded;
  selected = List::serialize(std::string("abc"), bounded, MediaType::parseAccept("text/*;q=0.5, multipart/*"), Side::CLIENT);
  anch::ut::assertThat(selected && selected->toString() == "multipart/x-test;boundary=b3" && bounded.str() == "b3 client abc",
		       "Bounded codec expected, found '{}'", bounded.str());
  std::ostringstream number;
  selected = List::serialize(12, number, MediaType::parseAccept("text/x-upper, text/plain;q=0.1"), Side::SERVER);
  anch::ut::assertThat(selected && selected->essence() == "text/plain" && number.str() == "12", "Only Text serializes int");
  std::ostringstream none;
  anch::ut::assertThat(!List::serialize(12, none, MediaType::parseAccept("application/json"), Side::SERVER) && none.str().empty(),
		       "No acceptable codec");
}

void
testFormats() {
  std::vector<anch::codec::Format> formats = anch::codec::formats();
  auto text = std::find_if(formats.begin(), formats.end(), [](const anch::codec::Format& format) { return format.name == "text"; });
  anch::ut::assertThat(text != formats.end() && text->mediaTypes == std::vector<std::string>{"text/plain"}, "text format has to be registered");
  anch::ut::assertThat(anch::codec::registerFormat(anch::codec::Format{"test", {"application/x-test"}}), "New format has to be registered");
  anch::ut::assertThat(!anch::codec::registerFormat(anch::codec::Format{"test", {"application/x-other"}}), "Format can be registered only once");
  formats = anch::codec::formats();
  anch::ut::assertThat(std::is_sorted(formats.begin(), formats.end(),
				      [](const anch::codec::Format& a, const anch::codec::Format& b) { return a.name < b.name; }),
		       "Formats have to be sorted by name");
  auto test = std::find_if(formats.begin(), formats.end(), [](const anch::codec::Format& format) { return format.name == "test"; });
  anch::ut::assertThat(test != formats.end() && test->mediaTypes == std::vector<std::string>{"application/x-test"},
		       "First registration has to be kept");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH codec unit tests")
    .description("Test AnCH codec library: media types, codecs, limits and formats")
    .add("media-type", testMediaType)
    .add("accept", testAccept)
    .add("dispatch", testDispatch)
    .add("limits", testLimits)
    .add("text", testText)
    .add("codecs", testCodecs)
    .add("formats", testFormats)
    ;
}
