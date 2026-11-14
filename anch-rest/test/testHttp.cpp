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
#include "rest/form.hpp"
#include "rest/formCodec.hpp"
#include "rest/headers.hpp"
#include "rest/mediaType.hpp"
#include "rest/multipartCodec.hpp"
#include "rest/method.hpp"
#include "rest/request.hpp"
#include "rest/response.hpp"
#include "rest/status.hpp"

#include <iterator>
#include <sstream>
#include <stdexcept>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::rest::Form;
using anch::rest::FormUrlEncoded;
using anch::rest::Headers;
using anch::rest::MediaType;
using anch::rest::Method;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Status;

void
testHeaders() {
  Headers headers;
  headers.add("Accept", "application/json");
  headers.add("accept", "text/plain");
  headers.set("Content-Type", "text/plain");
  anch::ut::assertThat(headers.has("ACCEPT"), "Header names have to be case insensitive");
  anch::ut::assertThat(headers.getAll("Accept").size() == 2, "Accept has {} value(s)", headers.getAll("Accept").size());
  anch::ut::assertThat(headers.format("accept") == "application/json, text/plain", "Unexpected joined values '{}'", headers.format("accept").value_or(""));
  headers.set("content-type", "application/json");
  anch::ut::assertThat(headers.get("Content-Type") == "application/json", "set() has to replace values");
  anch::ut::assertThat(headers.remove("CONTENT-TYPE") && !headers.has("Content-Type"), "remove() has to remove header");
  anch::ut::assertThat(!headers.get("X-Unknown").has_value() && headers.getAll("X-Unknown").empty(), "Unknown header has no value");
}

void
testMethodStatus() {
  anch::ut::assertThat(anch::rest::toString(Method::PATCH) == "PATCH", "Unexpected PATCH name");
  anch::ut::assertThat(anch::rest::parseMethod("DELETE") == Method::DELETE, "DELETE has to be parsed");
  anch::ut::assertThat(!anch::rest::parseMethod("get").has_value(), "Method names are case sensitive");
  anch::ut::assertThat(anch::rest::code(Status::NOT_FOUND) == 404, "NOT_FOUND has to be 404");
  anch::ut::assertThat(anch::rest::reasonPhrase(Status::IM_USED) == "IM Used" && anch::rest::code(Status::IM_USED) == 226, "IM_USED has to be 226");
}

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
  anch::ut::assertThat(anch::rest::negotiate(MediaType::parseAccept("text/*;q=0.3, text/html;q=0.7, */*;q=0.5"), available) == 2u, "text/html expected");
  anch::ut::assertThat(anch::rest::negotiate({}, available) == 0u, "First media type expected without Accept");
  anch::ut::assertThat(anch::rest::negotiate(MediaType::parseAccept("*/*, application/json;q=0"), available) == 1u, "q=0 has to exclude application/json");
  anch::ut::assertThat(!anch::rest::negotiate(MediaType::parseAccept("image/png"), available).has_value(), "No media type expected");
}

void
testRequestTarget() {
  Request request(Method::GET, "/pets/a%20b?name=Rex+le%20chien&flag&empty=&bad=%zz#fragment");
  anch::ut::assertThat(request.getPath() == "/pets/a%20b", "Path has to be kept encoded: '{}'", request.getPath());
  anch::ut::assertThat(request.getQueryParameter("name") == "Rex le chien", "Unexpected name '{}'", request.getQueryParameter("name").value_or(""));
  anch::ut::assertThat(request.getQueryParameter("flag") == "" && request.getQueryParameter("empty") == "", "Parameters without value have to be empty");
  anch::ut::assertThat(request.getQueryParameter("bad") == "%zz", "Invalid encoding has to be kept");
  anch::ut::assertThat(request.getQueryParameters().size() == 4, "Expected 4 query parameters");
  anch::ut::assertThat(anch::rest::percentEncode("a b/é~") == "a%20b%2F%C3%A9~", "Unexpected encoding '{}'", anch::rest::percentEncode("a b/é~"));
  anch::ut::assertThat(!anch::rest::percentDecode("%4").has_value(), "Truncated escape is invalid");
}

void
testResponse() {
  Response response = Response(Status::CREATED).header("Location", "/pets/1").contentType("text/plain").body("created");
  std::ostringstream out;
  response.writeBody(out);
  anch::ut::assertThat(response.getStatus() == Status::CREATED && response.getHeaders().get("location") == "/pets/1", "Unexpected status or headers");
  anch::ut::assertThat(out.str() == "created", "Unexpected body '{}'", out.str());
  Response copy = response;
  std::ostringstream copyOut;
  copy.writeBody(copyOut);
  anch::ut::assertThat(copyOut.str() == "created", "Copied response has to keep its body");
  response.clearBody();
  anch::ut::assertThat(!response.hasBody() && response.getHeaders().has("Content-Type"), "clearBody() has to keep headers");
}

void
testForm() {
  Form form;
  form.add("name", "Rex").add("tags", "dog").add("tags", "brown")
    .addFile("photo", "rex.png", MediaType("image", "png"), "PNG...");
  anch::ut::assertThat(form.size() == 4 && !form.empty() && form.has("photo") && !form.has("Name"), "Unexpected parts");
  anch::ut::assertThat(form.field("name") == "Rex" && !form.field("age"), "Unexpected field");
  anch::ut::assertThat(form.fields("tags") == std::vector<std::string_view>({"dog", "brown"}), "Repeated fields have to be kept in order");
  const anch::rest::Part* name = form.part("name");
  anch::ut::assertThat(name != nullptr && !name->isFile() && name->getMediaType().essence() == "text/plain" && !name->getFilename(),
		       "Field part has to be text/plain without file name");
  const anch::rest::Part* photo = form.part("photo");
  anch::ut::assertThat(photo != nullptr && photo->isFile() && photo->getFilename() == "rex.png" && photo->getMediaType().essence() == "image/png"
		       && photo->size() == 6, "Unexpected file part");
  std::unique_ptr<std::istream> input = photo->open();
  anch::ut::assertThat(std::string(std::istreambuf_iterator<char>(*input), std::istreambuf_iterator<char>()) == "PNG...", "open() has to read content");
  anch::ut::assertThat(form.parts("tags").size() == 2 && form.part("unknown") == nullptr, "Unexpected parts by name");
}

void
testFormUrlEncoded() {
  // Serialization
  std::ostringstream out;
  FormUrlEncoded::serialize(Form().add("name", "Médor le chien").add("q", "a&b=c+d*-._~").add("empty", ""), out);
  anch::ut::assertThat(out.str() == "name=M%C3%A9dor+le+chien&q=a%26b%3Dc%2Bd*-._%7E&empty=", "Unexpected body '{}'", out.str());
  std::ostringstream file;
  bool refused = false;
  try {
    FormUrlEncoded::serialize(Form().addFile("photo", "rex.png", MediaType("image", "png"), "PNG"), file);
  } catch(const std::invalid_argument&) {
    refused = true;
  }
  anch::ut::assertThat(refused, "File parts can not be URL-encoded");

  // Deserialization
  auto read = [](std::string_view body, std::string_view contentType = "application/x-www-form-urlencoded") {
    Form form;
    std::istringstream input{std::string(body)};
    FormUrlEncoded::deserialize(form, input, *MediaType::parse(contentType), anch::rest::Side::SERVER);
    return form;
  };
  Form form = read("name=M%C3%A9dor+le+chien&&tags=a&tags=b&flag&=anonymous&e=");
  anch::ut::assertThat(form.size() == 6, "Unexpected size {}", form.size());
  anch::ut::assertThat(form.field("name") == "Médor le chien", "Unexpected name '{}'", form.field("name").value_or(""));
  anch::ut::assertThat(form.fields("tags") == std::vector<std::string_view>({"a", "b"}), "Unexpected tags");
  anch::ut::assertThat(form.field("flag") == "" && form.field("") == "anonymous" && form.field("e") == "", "Unexpected empty values");
  anch::ut::assertThat(read("").empty(), "Empty body is an empty form");
  anch::ut::assertThat(read("a=1", "application/x-www-form-urlencoded; charset=UTF-8").field("a") == "1", "UTF-8 charset has to be accepted");
  auto invalid = [&read](std::string_view body, std::string_view contentType) {
    try {
      read(body, contentType);
    } catch(const std::invalid_argument&) {
      return true;
    }
    return false;
  };
  anch::ut::assertThat(invalid("a=%zz", "application/x-www-form-urlencoded"), "Invalid encoding has to be refused");
  anch::ut::assertThat(invalid("a=%", "application/x-www-form-urlencoded"), "Truncated encoding has to be refused");
  anch::ut::assertThat(invalid("a=1", "application/x-www-form-urlencoded; charset=iso-8859-1"), "Other charsets have to be refused");

  // Round trip
  std::ostringstream again;
  FormUrlEncoded::serialize(form, again);
  Form back = read(again.str());
  anch::ut::assertThat(back.size() == form.size() && back.field("name") == "Médor le chien" && back.field("") == "anonymous", "Round trip has to keep fields");
}

namespace {

  Form
  readMultipart(std::string_view body, std::string_view contentType = "multipart/form-data; boundary=XyZ") {
    Form form;
    std::istringstream input{std::string(body)};
    anch::rest::Multipart::deserialize(form, input, *MediaType::parse(contentType), anch::rest::Side::SERVER);
    return form;
  }

  template<typename E>
  std::string
  multipartError(std::string_view body, std::string_view contentType = "multipart/form-data; boundary=XyZ") {
    try {
      readMultipart(body, contentType);
    } catch(const E& e) {
      return e.what();
    } catch(const std::exception& e) {
      return std::string("unexpected exception: ") + e.what();
    }
    return "no error";
  }

}

void
testMultipart() {
  using namespace std::string_literals;
  // Body as written by browsers and curl, with preamble and epilogue
  const std::string body =
    "preamble\r\n"
    "--XyZ\r\n"
    "Content-Disposition: form-data; name=\"description\"\r\n"
    "\r\n"
    "Rex, le \"chien\"\r\n"
    "--XyZ\r\n"
    "content-disposition: FORM-DATA; name=photo; filename=\"rex.png\"\r\n"
    "Content-Type: image/png\r\n"
    "X-Custom: value\r\n"
    "\r\n"
    "\x89PNG\r\n\x1a\n\0--Xy\r\n"
    "--XyZ  \r\n"
    "Content-Disposition: form-data; name=\"doc\"; filename=\"a.txt\"; filename*=UTF-8''%E2%82%AC.txt\r\n"
    "\r\n"
    "\r\n"
    "--XyZ\r\n"
    "Content-Disposition: form-data; name=\"json\"\r\n"
    "Content-Type: application/json; charset=utf-8\r\n"
    "\r\n"
    "{\"a\":1}\r\n"
    "--XyZ--\r\n"
    "epilogue"s;
  Form form = readMultipart(body, "multipart/form-data; boundary=\"XyZ\"");
  anch::ut::assertThat(form.size() == 4, "Unexpected size {}", form.size());
  anch::ut::assertThat(form.field("description") == "Rex, le \"chien\"", "Unexpected description '{}'", form.field("description").value_or(""));
  const anch::rest::Part* photo = form.part("photo");
  anch::ut::assertThat(photo != nullptr && photo->isFile() && photo->getFilename() == "rex.png" && photo->getMediaType().essence() == "image/png",
		       "Unexpected photo part");
  anch::ut::assertThat(photo->content() == std::string_view("\x89PNG\r\n\x1a\n\0--Xy", 13), "Binary content has to be kept ({} bytes)", photo->size());
  anch::ut::assertThat(photo->getHeaders().get("X-Custom") == "value", "Other part headers have to be kept");
  const anch::rest::Part* doc = form.part("doc");
  anch::ut::assertThat(doc->getFilename() == "€.txt" && doc->getMediaType().essence() == "application/octet-stream" && doc->size() == 0,
		       "filename* has priority, files are application/octet-stream by default");
  anch::ut::assertThat(form.part("json")->getMediaType().getParameter("charset") == "utf-8", "Field media type has to be kept");
  anch::ut::assertThat(form.part("description")->getMediaType().essence() == "text/plain", "Fields are text/plain by default");

  // Round trip with a random boundary
  Form written;
  written.add("na\"me\r\n", "value").add("json", "{}");
  anch::rest::Part json("typed", "{}");
  json.setMediaType(MediaType("application", "json"));
  json.getHeaders().add("X-Custom", "value");
  written.add(std::move(json));
  written.addFile("file", "r\"ex.bin", MediaType("application", "octet-stream"), std::string("\0\r\n--anch-\r\n", 12));
  MediaType type = anch::rest::Multipart::contentType(written);
  MediaType other = anch::rest::Multipart::contentType(written);
  anch::ut::assertThat(type.essence() == "multipart/form-data" && type.getParameter("boundary")->size() == 37
		       && type.getParameter("boundary") != other.getParameter("boundary"), "Unexpected boundary '{}'", type.toString());
  std::ostringstream out;
  anch::rest::Multipart::serialize(written, out, type, anch::rest::Side::CLIENT);
  const std::string delimiter = "--" + std::string(*type.getParameter("boundary"));
  anch::ut::assertThat(out.str().starts_with(delimiter + "\r\nContent-Disposition: form-data; name=\"na%22me%0D%0A\"\r\n\r\nvalue\r\n")
		       && out.str().ends_with("\r\n" + delimiter + "--\r\n"), "Unexpected body '{}'", out.str());
  anch::ut::assertThat(out.str().find("name=\"json\"\r\n\r\n") != std::string::npos, "text/plain fields have no Content-Type");
  anch::ut::assertThat(out.str().find("name=\"typed\"\r\nContent-Type: application/json\r\nX-Custom: value\r\n\r\n{}") != std::string::npos,
		       "Typed fields have a Content-Type");
  Form back = readMultipart(out.str(), type.toString());
  anch::ut::assertThat(back.size() == 4 && back.field("na%22me%0D%0A") == "value" && back.part("typed")->getMediaType().essence() == "application/json"
		       && back.part("typed")->getHeaders().get("X-Custom") == "value", "Round trip has to keep parts");
  anch::ut::assertThat(back.part("file")->getFilename() == "r%22ex.bin" && back.part("file")->content() == std::string_view("\0\r\n--anch-\r\n", 12),
		       "Round trip has to keep files");

  // Writing errors
  auto writeError = [](const Form& value, const MediaType& contentType) {
    std::ostringstream ignored;
    try {
      anch::rest::Multipart::serialize(value, ignored, contentType, anch::rest::Side::CLIENT);
    } catch(const std::invalid_argument&) {
      return true;
    }
    return false;
  };
  anch::ut::assertThat(writeError(written, MediaType("multipart", "form-data")), "Boundary is required");
  MediaType fixed("multipart", "form-data");
  fixed.setParameter("boundary", "abc");
  anch::ut::assertThat(writeError(Form().add("a", "x\r\n--abc\r\n"), fixed), "Boundary must not appear in parts");
  anch::rest::Part injected("a", "x");
  injected.getHeaders().add("X-Injected", "a\r\nContent-Type: text/html");
  anch::ut::assertThat(writeError(Form().add(std::move(injected)), fixed), "CR / LF in part headers have to be refused");
  anch::ut::assertThat(readMultipart("--XyZ--").empty(), "Empty multipart body");
}

void
testMultipartMalformed() {
  const std::string part = "--XyZ\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\nvalue\r\n";
  auto expect = [](std::string_view body, std::string_view message, std::string_view contentType = "multipart/form-data; boundary=XyZ") {
    std::string error = multipartError<std::invalid_argument>(body, contentType);
    anch::ut::assertThat(error.find(message) != std::string::npos, "Expected '{}', found '{}'", message, error);
  };
  expect(part + "--XyZ--", "Missing multipart boundary", "multipart/form-data");
  expect(part + "--XyZ--", "Invalid multipart boundary", "multipart/form-data; boundary=\"" + std::string(71, 'a') + "\"");
  expect("no delimiter", "Missing multipart delimiter");
  expect(part, "Missing multipart close delimiter");
  expect(part + "--XyZ", "Missing multipart close delimiter");
  expect("--XyZ\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\nvalue", "Missing multipart close delimiter");
  expect("--XyZ\r\nContent-Disposition: form-data; name=\"a\"\r\n", "Unterminated multipart part headers");
  expect("--XyZgarbage\r\n" + part + "--XyZ--", "Invalid multipart delimiter line");
  expect("--XyZ\nContent-Disposition: form-data; name=\"a\"\n\nvalue\n--XyZ--", "Invalid multipart delimiter line");
  expect("--XyZ\r\n\r\nvalue\r\n--XyZ--", "Missing multipart Content-Disposition header");
  expect("--XyZ\r\nContent-Disposition: attachment; name=\"a\"\r\n\r\nv\r\n--XyZ--", "Invalid multipart Content-Disposition");
  expect("--XyZ\r\nContent-Disposition: form-data; name=\"a\r\n\r\nv\r\n--XyZ--", "Invalid multipart");
  expect("--XyZ\r\nContent-Disposition: form-data; filename=\"a\"\r\n\r\nv\r\n--XyZ--", "Missing multipart part name");
  expect("--XyZ\r\nContent-Disposition: form-data; name=\"a\"\r\nContent-Disposition: form-data; name=\"b\"\r\n\r\nv\r\n--XyZ--",
	 "Duplicate multipart Content-Disposition header");
  expect("--XyZ\r\nContent-Disposition: form-data; name=\"a\"\r\nContent-Type: nonsense\r\n\r\nv\r\n--XyZ--", "Invalid multipart part Content-Type");
  expect("--XyZ\r\nContent-Disposition: form-data; name=\"a\"\r\nNo colon\r\n\r\nv\r\n--XyZ--", "Invalid multipart part header");
  expect("--XyZ\r\nContent-Disposition: form-data; name=\"a\"\r\n folded: header\r\n\r\nv\r\n--XyZ--", "Invalid multipart part header");
}

void
testMultipartLimits() {
  anch::rest::FormLimits& limits = anch::rest::Multipart::limits(anch::rest::Side::SERVER);
  const anch::rest::FormLimits saved = limits;
  const std::string part = "--XyZ\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\nvalue\r\n";
  auto expect = [](std::string_view body, std::string_view message) {
    std::string error = multipartError<anch::rest::ContentTooLarge>(body);
    anch::ut::assertThat(error.find(message) != std::string::npos, "Expected '{}', found '{}'", message, error);
  };
  limits.maxParts = 2;
  anch::ut::assertThat(readMultipart(part + part + "--XyZ--").size() == 2, "Parts at limit have to be accepted");
  expect(part + part + part + "--XyZ--", "more than 2 parts");
  limits = saved;
  limits.maxPartSize = 5;
  anch::ut::assertThat(readMultipart(part + "--XyZ--").field("a") == "value", "Part at limit has to be accepted");
  expect("--XyZ\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\nvalue!\r\n--XyZ--", "part is larger than 5 bytes");
  expect("--XyZ\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n" + std::string(100, 'x'), "part is larger than 5 bytes");
  limits = saved;
  limits.maxHeadersSize = 48; // headers below are 48 bytes
  anch::ut::assertThat(readMultipart("--XyZ\r\nContent-Disposition: form-data; name=\"a\"\r\nX: y\r\n\r\nv\r\n--XyZ--").size() == 1,
		       "Headers at limit have to be accepted");
  expect("--XyZ\r\nContent-Disposition: form-data; name=\"a\"\r\nX: yz\r\n\r\nv\r\n--XyZ--", "headers are larger than 48 bytes");
  expect("--XyZ\r\nContent-Disposition: form-data; name=\"a\"" + std::string(100, ' '), "headers are larger than 48 bytes");
  limits = saved;
  limits.maxSize = 32;
  expect(part + "--XyZ--", "Content is larger than 32 bytes");
  limits = saved;

  // URL-encoded forms
  anch::rest::FormLimits& url = FormUrlEncoded::limits(anch::rest::Side::SERVER);
  const anch::rest::FormLimits urlSaved = url;
  auto readUrl = [](std::string_view body) {
    Form form;
    std::istringstream input{std::string(body)};
    try {
      FormUrlEncoded::deserialize(form, input, *MediaType::parse("application/x-www-form-urlencoded"), anch::rest::Side::SERVER);
    } catch(const anch::rest::ContentTooLarge& e) {
      return std::string(e.what());
    }
    return std::string("no error");
  };
  url.maxParts = 2;
  anch::ut::assertThat(readUrl("a=1&b=2") == "no error" && readUrl("a=1&b=2&c=3") == "Form has more than 2 fields", "Unexpected fields limit");
  url = urlSaved;
  url.maxPartSize = 3;
  anch::ut::assertThat(readUrl("a=1") == "no error" && readUrl("a=12") == "Form field is larger than 3 bytes", "Unexpected field size limit");
  url = urlSaved;
  url.maxSize = 4;
  anch::ut::assertThat(readUrl("a=12&") == "Content is larger than 4 bytes", "Unexpected size limit");
  url = urlSaved;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH REST HTTP model unit tests")
    .description("Test headers, media types, requests and responses")
    .add("headers", testHeaders)
    .add("method-status", testMethodStatus)
    .add("media-type", testMediaType)
    .add("accept", testAccept)
    .add("request-target", testRequestTarget)
    .add("response", testResponse)
    .add("form", testForm)
    .add("form-url-encoded", testFormUrlEncoded)
    .add("multipart", testMultipart)
    .add("multipart-malformed", testMultipartMalformed)
    .add("multipart-limits", testMultipartLimits)
    ;
}
