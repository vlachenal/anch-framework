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
#include <chrono>
#include <cstdint>
#include <iostream>
#include <istream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "json/json.hpp"
#include "rest/api.hpp"
#include "rest/cors.hpp"
#include "rest/form.hpp"
#include "rest/formCodec.hpp"
#include "rest/formMapper.hpp"
#include "rest/jsonCodec.hpp"
#include "rest/multipartCodec.hpp"
#include "rest/request.hpp"
#include "rest/response.hpp"

// Application model and services used by the documentation fragments +
struct Pet {
  uint64_t id = 0;
  std::string name;
};

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Pet>& mapper) {
  mapper
    .registerField("id", &Pet::id)
    .registerField("name", &Pet::name, anch::json::minLength(1));
}

struct UploadPhoto {
  std::string description;
  anch::rest::Part photo;
};

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<UploadPhoto>& mapper) {
  mapper.registerField("description", &UploadPhoto::description, anch::json::minLength(1));
}

std::optional<Pet>
findPet(uint64_t id) {
  if(id != 1) {
    return std::nullopt;
  }
  return Pet{1, "Rex"};
}

Pet
savePet(Pet pet) {
  pet.id = 2;
  return pet;
}

void
saveImage(uint64_t, std::string_view, std::istream& image) {
  image.ignore(std::numeric_limits<std::streamsize>::max());
}

anch::rest::Response
login(const std::string& user, bool remember) {
  return anch::rest::Response(anch::rest::Status::OK).body(user + (remember ? " (remembered)" : ""));
}

anch::rest::Response
savePhoto(uint64_t, const anch::rest::Part*) {
  return anch::rest::Response(anch::rest::Status::NO_CONTENT);
}
// Application model and services used by the documentation fragments -

//! [form mapper]
template<>
struct anch::rest::FormMapper<UploadPhoto> {
  static void toForm(const UploadPhoto& value, anch::rest::Form& form, anch::rest::Side) {
    anch::rest::addFormField(form, "description", value.description);
    anch::rest::addFormFile(form, "photo", value.photo);                  // anch::rest::Part
  }
  static void fromForm(UploadPhoto& value, const anch::rest::Form& form, anch::rest::Side side) {
    value.description = anch::rest::requiredFormField<std::string>(form, "description");
    value.photo = anch::rest::requiredFormFile(form, "photo");
    anch::rest::checkFormConstraints(value, side);
  }
};
//! [form mapper]

anch::rest::Response
created() {
  //! [response]
  return anch::rest::Response(anch::rest::Status::CREATED).header(anch::rest::header::LOCATION, "/pets/1");
  //! [response]
}

anch::rest::Response
api() {
  //! [api]
  anch::rest::Api api;
  api.get("/pets/{id}", [](uint64_t id) -> std::optional<Pet> { return findPet(id); }) // 404 when empty
    .produces<anch::rest::Json>();
  api.post("/pets", [](anch::rest::Body<Pet> pet) {
      Pet saved = savePet(*pet);
      return anch::rest::Response(anch::rest::Status::CREATED)
        .header(anch::rest::header::LOCATION, "/pets/" + std::to_string(saved.id));
    })
    .consumes<anch::rest::Json>();
  api.filter([](anch::rest::Request& request, const anch::rest::Next& next) {
      if(!request.getHeaders().has(anch::rest::header::AUTHORIZATION)) {
        return anch::rest::Response(anch::rest::Status::UNAUTHORIZED);
      }
      return next(request);
    });
  api.validate(); // every endpoint has its codecs
  anch::rest::Request request(anch::rest::Method::GET, "/pets/1");
  anch::rest::Response response = api.handle(request); // done by HTTP server adapter
  //! [api]
  return response;
}

bool
bodies() {
  anch::rest::Api api;
  //! [stream server]
  api.put("/pets/{id}/image", [](const anch::rest::Request& request, uint64_t id, std::istream& image) {
      saveImage(id, request.getHeaders().get(anch::rest::header::CONTENT_TYPE).value_or(""), image);
    });
  //! [stream server]
  //! [form server]
  api.post("/login", [](anch::rest::Body<anch::rest::Form> form) {
      std::string user = anch::rest::requiredFormField<std::string>(*form, "user"); // 400 when missing
      std::optional<bool> remember = anch::rest::formField<bool>(*form, "remember"); // 400 when invalid
      return login(user, remember.value_or(false));
    })
    .consumes<anch::rest::FormUrlEncoded>();
  //! [form server]
  //! [multipart server]
  api.post("/pets/{id}/photo", [](uint64_t id, anch::rest::Body<anch::rest::Form> form) {
      const anch::rest::Part* photo = form->part("photo"); // file name, media type, content
      return savePhoto(id, photo);
    })
    .consumes<anch::rest::Multipart>();
  //! [multipart server]
  //! [cors]
  api.cors({
    .origins = {"https://app.example.com"},           // "*" for every origin
    .headers = {"Content-Type", "Authorization"},     // "*" for every requested header
    .exposedHeaders = {"Location"},
    .credentials = true,                              // not allowed with "*" origin
    .maxAge = std::chrono::hours(1)
  });
  //! [cors]
  api.validate();
  anch::rest::Request request(anch::rest::Method::POST, "/login");
  request.getHeaders().set(anch::rest::header::CONTENT_TYPE, "application/x-www-form-urlencoded");
  request.setBody("user=rex&remember=true");
  anch::rest::Response response = api.handle(request);
  return anch::rest::code(response.getStatus()) == 200 && response.content() == "rex (remembered)";
}

void
uploads() {
  anch::rest::Api api;
  //! [form mapper api]
  api.post("/pets/{id}/photo", [](uint64_t id, anch::rest::Body<UploadPhoto> upload) { return savePhoto(id, &upload->photo); })
    .consumes<anch::rest::Multipart>();
  //! [form mapper api]
  api.validate();
}

void
configuration() {
  //! [multipart limits]
  anch::rest::FormLimits& limits = anch::rest::Multipart::limits(anch::rest::Side::SERVER);
  limits.maxSize = limits.maxPartSize = 1024 * 1024 * 1024; // 1 GiB uploads, read in temporary files
  limits.temporaryDirectory = "/var/tmp/uploads";
  //! [multipart limits]
  //! [json options]
  anch::rest::Json::options(anch::rest::Side::SERVER).serialize_check_constraints = true; // also check responses
  anch::rest::Json::options(anch::rest::Side::CLIENT).deserialize_ignore_unknown_field = false; // strict client
  //! [json options]
}

bool
forms() {
  //! [form]
  anch::rest::Form form;
  form.add("name", "Rex").add("tags", "dog").add("tags", "brown");
  std::optional<std::string_view> name = form.field("name");
  std::vector<std::string_view> tags = form.fields("tags");
  //! [form]
  return name == "Rex" && tags.size() == 2;
}

int
main() {
  bool ok = anch::rest::code(api().getStatus()) == 401 // no Authorization header
    && anch::rest::code(created().getStatus()) == 201
    && bodies()
    && forms();
  uploads();
  configuration();
  std::cout << (ok ? "OK" : "KO") << std::endl;
  return ok ? 0 : 1;
}
