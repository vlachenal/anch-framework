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
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "photos-client.hpp"
#include "photos-server.hpp"

#include "rest/api.hpp"
#include "rest/client.hpp"
#include "rest/form.hpp"
#include "rest/httpError.hpp"
#include "rest/mediaType.hpp"
#include "rest/request.hpp"
#include "rest/response.hpp"
#include "rest/transport.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::rest::MediaType;
using anch::rest::Method;
using anch::rest::Part;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Status;
using photos::model::Owner;
using photos::model::Photo;
using photos::model::PetPhotosClient;
using photos::model::PetPhotosService;
using photos::model::UploadPhotoRequest;

// Service +
namespace {

  /*! Service which describes what it receives */
  class Photos: public PetPhotosService {
  public:
    std::optional<anch::rest::Form> raw_;

    virtual std::string createOwner(const Owner& body) override {
      return body.name + ' ' + body.email.value_or("-");
    }

    virtual Photo uploadPhoto(int64_t id, const UploadPhotoRequest& body) override {
      Photo photo;
      photo.id = id;
      photo.description = body.description;
      photo.filename = std::string(body.photo.getFilename().value_or("-"));
      photo.mediaType = body.photo.getMediaType().essence();
      photo.size = static_cast<int64_t>(body.photo.size());
      photo.thumbnails = static_cast<int32_t>(body.thumbnails.size());
      photo.tags = body.tags;
      photo.owner = body.owner ? body.owner->name : std::string("-");
      return photo;
    }

    virtual void raw(const anch::rest::Form& body) override {
      raw_ = body;
    }
  };

  /*! Transport which calls the API directly (no network) */
  class ApiTransport: public anch::rest::Transport {
  private:
    anch::rest::Api& _api;
    std::string _base;

  public:
    ApiTransport(anch::rest::Api& api, std::string base): _api(api), _base(std::move(base)) {
    }

    virtual Response send(const std::string& url, const Request& request) override {
      Request copy(request.getMethod(), std::string_view(url).substr(_base.size()));
      copy.getHeaders() = request.getHeaders();
      if(request.hasBody()) {
	copy.setBody(std::string(std::istreambuf_iterator<char>(request.getBody()), std::istreambuf_iterator<char>()));
      }
      return _api.handle(copy);
    }
  };

  Response
  call(anch::rest::Api& api, std::string_view target, std::string_view contentType, const std::string& body) {
    Request request(Method::POST, target);
    request.getHeaders().set("Accept", "application/json, text/plain");
    request.getHeaders().set("Content-Type", contentType);
    request.setBody(body);
    return api.handle(request);
  }

}
// Service -

void
testFormsServer() {
  Photos service;
  anch::rest::Api api;
  photos::model::registerEndpoints(api, service);
  api.validate();
  // Body written as browsers do
  const std::string body = "--b\r\nContent-Disposition: form-data; name=\"description\"\r\n\r\nRex\r\n"
    "--b\r\nContent-Disposition: form-data; name=\"tags\"\r\n\r\ndog\r\n"
    "--b\r\nContent-Disposition: form-data; name=\"tags\"\r\n\r\nbrown\r\n"
    "--b\r\nContent-Disposition: form-data; name=\"owner\"\r\nContent-Type: application/json\r\n\r\n{\"name\":\"Vincent\"}\r\n"
    "--b\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"rex.png\"\r\nContent-Type: image/png\r\n\r\nPNG...\r\n"
    "--b\r\nContent-Disposition: form-data; name=\"thumbnails\"; filename=\"a.png\"\r\n\r\nA\r\n"
    "--b\r\nContent-Disposition: form-data; name=\"thumbnails\"; filename=\"b.png\"\r\n\r\nB\r\n--b--\r\n";
  Response uploaded = call(api, "/pets/7/photo", "multipart/form-data; boundary=b", body);
  anch::ut::assertThat(uploaded.getStatus() == Status::OK, "Unexpected status {}: {}", anch::rest::code(uploaded.getStatus()), uploaded.content());
  const std::string json = uploaded.content();
  for(const char* expected : {"\"id\":7", "\"description\":\"Rex\"", "\"filename\":\"rex.png\"", "\"mediaType\":\"image/png\"", "\"size\":6",
			      "\"thumbnails\":2", "\"tags\":[\"dog\",\"brown\"]", "\"owner\":\"Vincent\""}) {
    anch::ut::assertThat(json.find(expected) != std::string::npos, "'{}' not found in {}", expected, json);
  }
  // Missing required file, constraints
  Response missing = call(api, "/pets/7/photo", "multipart/form-data; boundary=b",
			  "--b\r\nContent-Disposition: form-data; name=\"description\"\r\n\r\nRex\r\n--b--\r\n");
  anch::ut::assertThat(missing.getStatus() == Status::BAD_REQUEST && missing.content() == "Missing form part 'photo'", "Unexpected response '{}'",
		       missing.content());
  Response empty = call(api, "/pets/7/photo", "multipart/form-data; boundary=b",
			"--b\r\nContent-Disposition: form-data; name=\"description\"\r\n\r\n\r\n"
			"--b\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"rex.png\"\r\n\r\nP\r\n--b--\r\n");
  anch::ut::assertThat(empty.getStatus() == Status::BAD_REQUEST && empty.content().starts_with("/description: "), "Unexpected response '{}'",
		       empty.content());
  // Same model in JSON and URL-encoded form
  Response json1 = call(api, "/owners", "application/json", R"({"name":"Vincent","email":"v@example.com"})");
  Response form1 = call(api, "/owners", "application/x-www-form-urlencoded", "name=Vincent&email=v%40example.com");
  anch::ut::assertThat(json1.getStatus() == Status::CREATED && json1.content() == "Vincent v@example.com" && form1.content() == json1.content(),
		       "Unexpected responses '{}' and '{}'", json1.content(), form1.content());
  Response tooLong = call(api, "/owners", "application/x-www-form-urlencoded", "name=Vincent+Lachenal");
  anch::ut::assertThat(tooLong.getStatus() == Status::BAD_REQUEST && tooLong.content().starts_with("/name: "), "Unexpected response '{}'",
		       tooLong.content());
  // Raw form
  Response raw = call(api, "/raw", "multipart/form-data; boundary=b", "--b\r\nContent-Disposition: form-data; name=\"x\"\r\n\r\n1\r\n--b--\r\n");
  anch::ut::assertThat(raw.getStatus() == Status::NO_CONTENT && service.raw_ && service.raw_->field("x") == "1", "Raw form has to be received");
}

void
testFormsClient() {
  Photos service;
  anch::rest::Api api;
  photos::model::registerEndpoints(api, service);
  anch::rest::Client rest(std::make_unique<ApiTransport>(api, "http://localhost/api"), "http://localhost/api");
  PetPhotosClient client(rest);
  UploadPhotoRequest upload;
  upload.description = "Médor";
  upload.tags = {"dog"};
  upload.photo = Part("ignored", "médor.png", MediaType("image", "png"), std::string("\0PNG\r\n--", 8));
  upload.thumbnails = {Part("t", "t.png", MediaType("image", "png"), "T")};
  upload.owner = Owner{.email = std::nullopt, .name = "Vincent"};
  Photo photo = client.uploadPhoto(3, upload);
  anch::ut::assertThat(photo.id == 3 && photo.description == "Médor" && photo.filename == "médor.png" && photo.mediaType == "image/png"
		       && photo.size == 8 && photo.thumbnails == 1 && photo.tags == std::vector<std::string>({"dog"}) && photo.owner == "Vincent",
		       "Unexpected photo");
  anch::ut::assertThat(client.createOwner(Owner{.email = "rex@example.com", .name = "Rex"}) == "Rex rex@example.com", "Unexpected owner");
  anch::rest::Form form;
  form.add("y", "2");
  client.raw(form);
  anch::ut::assertThat(service.raw_ && service.raw_->field("y") == "2", "Raw form has to be sent");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH OAS forms tests")
    .description("Test form bodies endpoints generated from forms.json (anch-rest server and client)")
    .add("forms-server", testFormsServer)
    .add("forms-client", testFormsClient)
    ;
}
