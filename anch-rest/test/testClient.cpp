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
#include "rest/auth.hpp"

#include "codec/json.hpp"
#include "codec/text.hpp"

#include "rest/client.hpp"
#include "rest/formCodec.hpp"
#include "rest/multipartCodec.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <optional>
#include <initializer_list>
#include <utility>
#include <iostream>
#include <string>
#include <vector>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::rest::Client;
using anch::rest::HttpError;
using anch::codec::Json;
using anch::rest::Method;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Status;
using anch::codec::Text;

// Codec with media type parameters (as a multipart codec with its boundary) +
struct Bounded {
  static std::string_view
  mediaType() noexcept {
    return "multipart/x-test";
  }

  static anch::codec::MediaType
  contentType(const std::string& value) {
    anch::codec::MediaType type("multipart", "x-test");
    type.setParameter("boundary", "b" + std::to_string(value.size()));
    return type;
  }

  static void
  serialize(const std::string& value, std::ostream& out, const anch::codec::MediaType& contentType, anch::codec::Side side) {
    out << "--" << contentType.getParameter("boundary").value_or("") << ' ' << (side == anch::codec::Side::SERVER ? "server" : "client")
	<< ' ' << value;
  }

  static void
  deserialize(std::string& value, std::istream& input, const anch::codec::MediaType& contentType, anch::codec::Side side) {
    std::optional<std::string_view> boundary = contentType.getParameter("boundary");
    if(!boundary) {
      throw std::invalid_argument("missing boundary");
    }
    value = std::string(*boundary) + ' ' + (side == anch::codec::Side::SERVER ? "server" : "client") + ' '
      + std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
  }
};
static_assert(!anch::codec::Serializer<Bounded, int> && !anch::codec::Deserializer<Bounded, int>);
// Codec with media type parameters -

// Fake transport +
namespace {

  /*! Transport which records requests and answers a prepared response */
  class FakeTransport: public anch::rest::Transport {
  public:
    std::string url;
    Method method = Method::GET;
    anch::rest::Headers headers;
    std::string body;
    Response response;

    virtual Response send(const std::string& requestUrl, const Request& request) override {
      url = requestUrl;
      method = request.getMethod();
      headers = request.getHeaders();
      body = request.hasBody() ? std::string(std::istreambuf_iterator<char>(request.getBody()), std::istreambuf_iterator<char>()) : std::string();
      return response;
    }
  };

  /*! Client and its transport */
  struct Fixture {
    FakeTransport* transport;
    Client client;

    explicit Fixture(std::string_view baseUrl = "http://localhost:8080/api/"):
      transport(new FakeTransport()),
      client(std::unique_ptr<anch::rest::Transport>(transport), baseUrl) {
    }
  };

  template<typename E, typename F>
  bool
  throws(F function) {
    try {
      function();
    } catch(const E&) {
      return true;
    }
    return false;
  }

}
// Fake transport -

struct Pet {
  uint64_t id;
  std::string name;
};

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Pet>& mapper) {
  mapper
    .registerField("id", &Pet::id)
    .registerField("name", &Pet::name)
    ;
}

struct Owner {
  std::string name;
  std::optional<std::string> email;
};

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Owner>& mapper) {
  mapper
    .registerField("name", &Owner::name, anch::val::required, anch::val::minLength(1), anch::val::maxLength(10))
    .registerField<std::string>("email", &Owner::email, anch::val::format("email"))
    ;
}

// Form mapping +
struct UploadPhoto {
  std::string description;
  std::optional<uint32_t> age;
  std::vector<std::string> tags;
  anch::rest::Part photo;
  std::optional<Pet> pet;
};

template<>
struct anch::rest::FormMapper<UploadPhoto> {
  static void
  toForm(const UploadPhoto& value, anch::rest::Form& form, anch::codec::Side side) {
    anch::rest::addFormField(form, "description", value.description);
    anch::rest::addFormField(form, "age", value.age);
    anch::rest::addFormField(form, "tags", value.tags);
    anch::rest::addFormFile(form, "photo", value.photo);
    if(value.pet) {
      anch::rest::addJsonFormField(form, "pet", *value.pet, side);
    }
  }

  static void
  fromForm(UploadPhoto& value, const anch::rest::Form& form, anch::codec::Side side) {
    value.description = anch::rest::requiredFormField<std::string>(form, "description");
    value.age = anch::rest::formField<uint32_t>(form, "age");
    value.tags = anch::rest::formFields<std::string>(form, "tags");
    value.photo = anch::rest::requiredFormFile(form, "photo");
    value.pet = anch::rest::jsonFormField<Pet>(form, "pet", side);
  }
};
// Form mapping -

void
testFormatPath() {
  anch::ut::assertThat(Client::formatPath("/owners/{owner}/pets/{id}", {"a b/c", "42"}) == "/owners/a%20b%2Fc/pets/42",
		       "Unexpected path '{}'", Client::formatPath("/owners/{owner}/pets/{id}", {"a b/c", "42"}));
  anch::ut::assertThat(throws<std::invalid_argument>([]() { Client::formatPath("/pets/{id}", {}); }), "Missing parameter has to be refused");
  anch::ut::assertThat(throws<std::invalid_argument>([]() { Client::formatPath("/pets", {"1"}); }), "Extra parameter has to be refused");
  anch::ut::assertThat(throws<std::invalid_argument>([]() { Client::formatPath("/pets/{id", {"1"}); }), "Unclosed placeholder has to be refused");
}

void
testUrl() {
  Fixture fixture;
  fixture.client.get("/pets/{id}", 42).query("name", "Rex & co").query("limit", 10).query("all", true).send();
  anch::ut::assertThat(fixture.transport->url == "http://localhost:8080/api/pets/42?name=Rex%20%26%20co&limit=10&all=true",
		       "Unexpected URL '{}'", fixture.transport->url);
  fixture.client.del("pets").send();
  anch::ut::assertThat(fixture.transport->url == "http://localhost:8080/api/pets" && fixture.transport->method == Method::DELETE,
		       "Unexpected URL '{}'", fixture.transport->url);
}

void
testHeaders() {
  Fixture fixture;
  fixture.client.header("X-Tenant", "anch");
  fixture.client.get("/pets").send();
  anch::ut::assertThat(fixture.transport->headers.get("User-Agent") == "anch-rest" && fixture.transport->headers.get("X-Tenant") == "anch",
		       "Default headers have to be sent");
  fixture.client.get("/pets").header("user-agent", "test").accept<Json>().accept<Text>().send();
  anch::ut::assertThat(fixture.transport->headers.get("User-Agent") == "test", "Request header has to override default header");
  anch::ut::assertThat(fixture.transport->headers.format("Accept") == "application/json, text/plain", "Unexpected Accept '{}'",
		       fixture.transport->headers.format("Accept").value_or(""));
}

void
testBody() {
  Fixture fixture;
  fixture.client.post("/pets").body<Json>(Pet{7, "Rex"}).send();
  anch::ut::assertThat(fixture.transport->body == "{\"id\":7,\"name\":\"Rex\"}" && fixture.transport->headers.get("Content-Type") == "application/json",
		       "Unexpected body '{}'", fixture.transport->body);
}

void
testCodecMediaType() {
  Fixture fixture;
  fixture.transport->response = Response().contentType("multipart/x-test; boundary=srv").body("data");
  std::string result = fixture.client.post("/echo").body<Bounded>(std::string("hi")).accept<Bounded>().as<std::string>();
  std::optional<anch::codec::MediaType> type = anch::codec::MediaType::parse(fixture.transport->headers.get("Content-Type").value_or(""));
  anch::ut::assertThat(type && type->essence() == "multipart/x-test" && type->getParameter("boundary") == "b2",
		       "Unexpected request content type '{}'", fixture.transport->headers.get("Content-Type").value_or(""));
  anch::ut::assertThat(fixture.transport->body == "--b2 client hi", "Unexpected request body '{}'", fixture.transport->body);
  anch::ut::assertThat(result == "srv client data", "Response has to be read with its content type parameters, found '{}'", result);
}

void
testForm() {
  Fixture fixture;
  fixture.client.post("/login").body<anch::rest::FormUrlEncoded>(anch::rest::Form().add("user", "rex").add("password", "s3cr&t")).send();
  anch::ut::assertThat(fixture.transport->headers.get("Content-Type") == "application/x-www-form-urlencoded"
		       && fixture.transport->body == "user=rex&password=s3cr%26t", "Unexpected request body '{}'", fixture.transport->body);
  fixture.transport->response = Response().contentType("application/x-www-form-urlencoded").body("token=abc+def");
  anch::rest::Form form = fixture.client.get("/token").accept<anch::rest::FormUrlEncoded>().as<anch::rest::Form>();
  anch::ut::assertThat(form.field("token") == "abc def", "Unexpected token '{}'", form.field("token").value_or(""));
}

void
testMultipart() {
  Fixture fixture;
  anch::rest::Form form;
  form.add("description", "Rex").addFile("photo", "rex.png", anch::codec::MediaType("image", "png"), "PNG...");
  fixture.client.post("/pets/{id}/photo", 1).body<anch::rest::Multipart>(form).send();
  std::optional<anch::codec::MediaType> type = anch::codec::MediaType::parse(fixture.transport->headers.get("Content-Type").value_or(""));
  anch::ut::assertThat(type && type->essence() == "multipart/form-data" && type->getParameter("boundary"), "Unexpected content type '{}'",
		       fixture.transport->headers.get("Content-Type").value_or(""));
  const std::string delimiter = "--" + std::string(*type->getParameter("boundary"));
  anch::ut::assertThat(fixture.transport->body == delimiter + "\r\nContent-Disposition: form-data; name=\"description\"\r\n\r\nRex\r\n"
		       + delimiter + "\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"rex.png\"\r\nContent-Type: image/png\r\n\r\nPNG...\r\n"
		       + delimiter + "--\r\n", "Unexpected body '{}'", fixture.transport->body);
  fixture.transport->response = Response().contentType("multipart/form-data; boundary=srv")
    .body("--srv\r\nContent-Disposition: form-data; name=\"id\"\r\n\r\n42\r\n--srv--\r\n");
  anch::rest::Form result = fixture.client.get("/card").accept<anch::rest::Multipart>().as<anch::rest::Form>();
  anch::ut::assertThat(result.field("id") == "42", "Unexpected response form");
}

void
testMultipartStream() {
  // File parts are streamed by the transport with Content-Length
  const std::filesystem::path path = std::filesystem::current_path() / "client-stream.bin";
  {
    std::ofstream out(path, std::ios::binary);
    out << std::string(100000, 'z');
  }
  Fixture fixture;
  anch::rest::Form form;
  form.add("description", "Rex").add(anch::rest::Part::fromFile("photo", path, anch::codec::MediaType("image", "png")));
  fixture.client.post("/photo").body<anch::rest::Multipart>(form).send();
  std::filesystem::remove(path);
  anch::ut::assertThat(fixture.transport->headers.get("Content-Length") == std::to_string(fixture.transport->body.size()),
		       "Content-Length {} expected, found '{}'", fixture.transport->body.size(),
		       fixture.transport->headers.get("Content-Length").value_or(""));
  anch::ut::assertThat(fixture.transport->body.find(std::string(100000, 'z')) != std::string::npos
		       && fixture.transport->body.find("filename=\"client-stream.bin\"") != std::string::npos, "File has to be sent");
}

void
testRawBody() {
  Fixture fixture;
  fixture.client.put("/blobs/{id}", 7).body(std::make_unique<std::istringstream>(std::string(1000, 'b')), "image/png", 1000).send();
  anch::ut::assertThat(fixture.transport->method == Method::PUT && fixture.transport->body == std::string(1000, 'b')
		       && fixture.transport->headers.get("Content-Type") == "image/png" && fixture.transport->headers.get("Content-Length") == "1000",
		       "Unexpected raw body request");
  fixture.client.post("/blobs").body(std::make_unique<std::istringstream>("abc"), "application/octet-stream").send();
  anch::ut::assertThat(fixture.transport->body == "abc" && !fixture.transport->headers.has("Content-Length"),
		       "Body without size has no Content-Length");
}

void
testFormMapper() {
  Fixture fixture;
  UploadPhoto upload{"Rex", std::nullopt, {"a"}, anch::rest::Part("x", "rex.png", anch::codec::MediaType("image", "png"), "PNG"), std::nullopt};
  fixture.client.post("/pets/{id}/photo", 1).body<anch::rest::Multipart>(upload).send();
  std::optional<anch::codec::MediaType> type = anch::codec::MediaType::parse(fixture.transport->headers.get("Content-Type").value_or(""));
  anch::ut::assertThat(type && type->essence() == "multipart/form-data" && type->getParameter("boundary"), "Unexpected content type");
  std::istringstream input(fixture.transport->body);
  anch::rest::Form form;
  anch::rest::Multipart::deserialize(form, input, *type, anch::codec::Side::SERVER);
  anch::ut::assertThat(form.size() == 3 && form.field("description") == "Rex" && !form.has("age") && form.field("tags") == "a"
		       && form.part("photo")->getFilename() == "rex.png", "Unexpected sent form");
  bool refused = false;
  try {
    fixture.client.post("/pets/{id}/photo", 1).body<anch::rest::FormUrlEncoded>(upload).send();
  } catch(const std::invalid_argument&) {
    refused = true;
  }
  anch::ut::assertThat(refused, "File can not be URL-encoded");
}

void
testAs() {
  Fixture fixture;
  fixture.transport->response = Response().contentType("application/json; charset=utf-8").body("{\"id\":1,\"name\":\"Rex\"}");
  Pet pet = fixture.client.get("/pets/{id}", 1).accept<Text>().accept<Json>().as<Pet>();
  anch::ut::assertThat(pet.id == 1 && pet.name == "Rex", "Unexpected pet");
  fixture.transport->response = Response().contentType("text/plain").body("plop");
  anch::ut::assertThat(fixture.client.get("/pets/1").accept<Json>().accept<Text>().as<std::string>() == "plop", "text/plain has to be read with Text");
  anch::ut::assertThat(throws<std::runtime_error>([&]() { fixture.client.get("/pets/1").accept<Json>().as<Pet>(); }),
		       "Not accepted content type has to be refused");
}

void
testHttpError() {
  Fixture fixture;
  fixture.transport->response = Response(Status::NOT_FOUND).contentType("text/plain").body("no pet");
  try {
    fixture.client.get("/pets/{id}", 2).accept<Json>().as<Pet>();
  } catch(const HttpError& e) {
    anch::ut::assertThat(e.getStatus() == Status::NOT_FOUND && e.getResponse().content() == "no pet", "Error response has to be kept");
    anch::ut::assertThat(std::string(e.what()) == "HTTP 404 Not Found", "Unexpected message '{}'", e.what());
    anch::ut::assertThat(fixture.client.get("/pets/2").send().getStatus() == Status::NOT_FOUND, "send() must not throw");
    anch::ut::assertThat(throws<HttpError>([&]() { fixture.client.del("/pets/2").execute(); }), "execute() has to throw");
    return;
  }
  anch::ut::assertThat(false, "HttpError expected");
}

void
testFilters() {
  Fixture fixture;
  std::vector<std::string> calls;
  fixture.client.filter([&calls](Request& request, const anch::rest::Next& next) {
    calls.push_back("first");
    Response response = next(request);
    response.header("X-Filtered", "1");
    return response;
  });
  fixture.client.filter(anch::rest::bearerAuth("token"));
  fixture.client.filter([&calls](Request& request, const anch::rest::Next& next) {
    calls.push_back("last " + std::string(request.getHeaders().get("Authorization").value_or("")));
    return next(request);
  });
  Response response = fixture.client.get("/pets").send();
  anch::ut::assertThat(calls == std::vector<std::string>({"first", "last Bearer token"}), "Filters have to be executed in order");
  anch::ut::assertThat(response.getHeaders().has("X-Filtered"), "Filter has to change response");
}

void
testBase64() {
  anch::ut::assertThat(anch::rest::base64Encode("Aladdin:open sesame") == "QWxhZGRpbjpvcGVuIHNlc2FtZQ==", "Unexpected encoding");
  anch::ut::assertThat(anch::rest::base64Encode("") == "" && anch::rest::base64Encode("f") == "Zg==" && anch::rest::base64Encode("fo") == "Zm8="
		       && anch::rest::base64Encode("foo") == "Zm9v" && anch::rest::base64Encode("\xff\xfe") == "//4=", "Unexpected padding");
  Fixture fixture;
  fixture.client.filter(anch::rest::basicAuth("Aladdin", "open sesame"));
  fixture.client.get("/").send();
  anch::ut::assertThat(fixture.transport->headers.get("Authorization") == "Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==", "Unexpected Basic authorization");
}

void
testConstraints() {
  Fixture fixture;
  // Invalid request is not sent
  try {
    fixture.client.post("/owners").body<Json>(Owner{"", std::nullopt}).send();
    anch::ut::fail("Invalid request should not be sent");
  } catch(const anch::json::MappingError& e) {
    std::cout << e.what() << std::endl;
    anch::ut::assertThat(e.getPath() == "/name", "Expected path '/name' ; found '{}'", e.getPath());
  }
  anch::ut::assertThat(fixture.transport->url.empty(), "Transport should not be called");
  fixture.client.post("/owners").body<Json>(Owner{"Vincent", "vincent@anch.org"}).send();
  anch::ut::assertThat(fixture.transport->body == R"({"name":"Vincent","email":"vincent@anch.org"})", "Unexpected body '{}'", fixture.transport->body);
  // Responses are trusted: constraints are not checked
  fixture.transport->response = Response().contentType("application/json").body(R"({"email":"x"})");
  Owner owner = fixture.client.get("/owners/1").accept<Json>().as<Owner>();
  anch::ut::assertThat(owner.name.empty() && owner.email == "x", "Response should be trusted");
  // Responses are read with tolerance: unknown fields and whitespaces
  fixture.transport->response = Response().contentType("application/json").body("{\n  \"name\": \"Rex\",\n  \"age\": 3,\n  \"tags\": [ \"a\", { \"b\": 1 } ]\n}");
  owner = fixture.client.get("/owners/1").accept<Json>().as<Owner>();
  anch::ut::assertThat(owner.name == "Rex", "Unexpected name '{}'", owner.name);
  // Client options can be changed
  Json::options(anch::codec::Side::CLIENT).serialize_check_constraints = false;
  fixture.client.post("/owners").body<Json>(Owner{"", std::nullopt}).send();
  anch::ut::assertThat(fixture.transport->body == R"({"name":""})", "Unexpected body '{}'", fixture.transport->body);
  Json::options(anch::codec::Side::CLIENT).serialize_check_constraints = true;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH REST client unit tests")
    .description("Test client requests building with a fake transport")
    .add("format-path", testFormatPath)
    .add("url", testUrl)
    .add("headers", testHeaders)
    .add("body", testBody)
    .add("as", testAs)
    .add("codec-media-type", testCodecMediaType)
    .add("form", testForm)
    .add("multipart", testMultipart)
    .add("form-mapper", testFormMapper)
    .add("multipart-stream", testMultipartStream)
    .add("raw-body", testRawBody)
    .add("constraints", testConstraints)
    .add("http-error", testHttpError)
    .add("filters", testFilters)
    .add("base64", testBase64)
    ;
}
