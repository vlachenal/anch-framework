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
#include "rest/protobufCodec.hpp"

#include <limits>
#include <stdexcept>

#include <google/protobuf/util/json_util.h>

using anch::rest::Protobuf;
using anch::rest::ProtobufJson;

std::string_view
Protobuf::mediaType() noexcept {
  return "application/x-protobuf";
}

void
Protobuf::write(const google::protobuf::Message& message, std::ostream& out) {
  if(!message.SerializeToOstream(&out)) {
    throw std::runtime_error("Unable to write protobuf message " + std::string(message.GetTypeName()));
  }
}

void
Protobuf::read(google::protobuf::Message& message, std::istream& input) {
  if(!message.ParseFromIstream(&input)) {
    throw std::invalid_argument("Invalid protobuf message " + std::string(message.GetTypeName()));
  }
}

void
Protobuf::read(google::protobuf::Message& message, std::string_view data) {
  if(data.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())
     || !message.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
    throw std::invalid_argument("Invalid protobuf message " + std::string(message.GetTypeName()));
  }
}

std::string_view
ProtobufJson::mediaType() noexcept {
  return "application/json";
}

std::string
ProtobufJson::toJson(const google::protobuf::Message& message) {
  std::string json;
  const absl::Status status = google::protobuf::util::MessageToJsonString(message, &json);
  if(!status.ok()) {
    throw std::runtime_error("Unable to write protobuf message " + std::string(message.GetTypeName()) + " in JSON: "
			     + std::string(status.message()));
  }
  return json;
}

void
ProtobufJson::fromJson(google::protobuf::Message& message, std::string_view json, bool ignoreUnknownFields) {
  google::protobuf::util::JsonParseOptions options;
  options.ignore_unknown_fields = ignoreUnknownFields;
  const absl::Status status = google::protobuf::util::JsonStringToMessage(json, &message, options);
  if(!status.ok()) {
    throw std::invalid_argument("Invalid " + std::string(message.GetTypeName()) + " JSON message: " + std::string(status.message()));
  }
}
