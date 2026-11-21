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
#include <fstream>
#include <memory>
#include <string>

#include "codec/json.hpp"

#include "json/json.hpp"
#include "rest/auth.hpp"
#include "rest/client.hpp"
#include "rest/curlTransport.hpp"
#include "rest/form.hpp"
#include "rest/formCodec.hpp"
#include "rest/multipartCodec.hpp"

// Client examples are built, not run: the server of the examples does not exist

struct Pet {
  uint64_t id = 0;
  std::string name;
};

template<>
struct anch::Mapping<Pet> {
  static void
  registerObject(anch::ObjectMapper<Pet> auto& mapper) {
    mapper
      .registerField("id", &Pet::id)
      .registerField("name", &Pet::name);
  }
};

void
client(const std::string& token) {
  //! [client]
  anch::rest::TransportConfig config;
  config.timeout = std::chrono::seconds(5);
  anch::rest::Client client(std::make_unique<anch::rest::CurlTransport>(config), "https://api.example.com/v1");
  client.filter(anch::rest::bearerAuth(token));
  //! [get]
  Pet pet = client.get("/pets/{id}", 42).accept<anch::codec::Json>().as<Pet>(); // HttpError when status >= 400
  //! [get]
  anch::rest::Response created = client.post("/pets").body<anch::codec::Json>(pet).send();
  client.del("/pets/{id}", 42).execute();
  //! [client]
}

void
bodies(anch::rest::Client& client, std::uint64_t size, const std::string& pwd, const std::string& content) {
  //! [stream client]
  client.put("/pets/{id}/image", 42).body(std::make_unique<std::ifstream>("rex.png", std::ios::binary), "image/png", size).execute();
  //! [stream client]
  //! [form client]
  client.post("/login").body<anch::rest::FormUrlEncoded>(anch::rest::Form().add("user", "rex").add("password", pwd)).send();
  //! [form client]
  //! [multipart client]
  anch::rest::Form upload;
  upload.add("description", "Rex").addFile("photo", "rex.png", anch::codec::MediaType("image", "png"), content);
  client.post("/pets/{id}/photo", 42).body<anch::rest::Multipart>(upload).send();
  //! [multipart client]
}

void
largeUpload(anch::rest::Client& client) {
  //! [upload file]
  anch::rest::Form upload;
  upload.add(anch::rest::Part::fromFile("video", "/data/video.mp4", anch::codec::MediaType("video", "mp4")));
  client.post("/videos").body<anch::rest::Multipart>(upload).send(); // read from the file while it is sent
  //! [upload file]
}

int
main() {
  return 0;
}
