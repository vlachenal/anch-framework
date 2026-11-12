/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "rest/response.hpp"

#include <memory>
#include <sstream>

using anch::rest::BodyWriter;
using anch::rest::Headers;
using anch::rest::Response;
using anch::rest::Status;

Response::Response(Status status): _status(status), _headers(), _body() {
  // Nothing to do
}

Response&
Response::header(std::string_view name, std::string_view value) & {
  _headers.set(name, value);
  return *this;
}

Response&&
Response::header(std::string_view name, std::string_view value) && {
  return std::move(header(name, value));
}

Response&
Response::contentType(std::string_view mediaType) & {
  return header(anch::rest::header::CONTENT_TYPE, mediaType);
}

Response&&
Response::contentType(std::string_view mediaType) && {
  return std::move(contentType(mediaType));
}

Response&
Response::body(std::string content) & {
  // std::function has to be copyable: content is shared between copies
  auto shared = std::make_shared<const std::string>(std::move(content));
  _body = [shared](std::ostream& out) {
    out.write(shared->data(), static_cast<std::streamsize>(shared->size()));
  };
  return *this;
}

Response&&
Response::body(std::string content) && {
  return std::move(body(std::move(content)));
}

Response&
Response::stream(BodyWriter writer) & {
  _body = std::move(writer);
  return *this;
}

Response&&
Response::stream(BodyWriter writer) && {
  return std::move(stream(std::move(writer)));
}

bool
Response::hasBody() const noexcept {
  return static_cast<bool>(_body);
}

void
Response::writeBody(std::ostream& out) const {
  if(_body) {
    _body(out);
  }
}

std::string
Response::content() const {
  std::ostringstream out;
  writeBody(out);
  return out.str();
}

void
Response::clearBody() noexcept {
  _body = nullptr;
}

Status
Response::getStatus() const noexcept {
  return _status;
}

void
Response::setStatus(Status status) noexcept {
  _status = status;
}

Headers&
Response::getHeaders() noexcept {
  return _headers;
}

const Headers&
Response::getHeaders() const noexcept {
  return _headers;
}
