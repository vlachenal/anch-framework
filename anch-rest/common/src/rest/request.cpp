/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2020 Vincent Lachenal

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
#include "rest/request.hpp"

using anch::rest::Request;

// HTTP request +
// Constructors +
Request::Request():
  _verb(),
  _headers(),
  _path(),
  _parameters(),
  _input(),
  _registry(NULL) {
  // nothing to do
}

Request::Request(Request&& other):
  _verb(std::move(other._verb)),
  _headers(std::move(other._headers)),
  _path(std::move(other._path)),
  _parameters(std::move(other._parameters)),
  _input(other._input),
  _registry(other._registry) {
  // Nothing to do
}
// Constructors -

// Destructor +
Request::~Request() {
  // nothing to do
}
// Destructor -

// Methods +
Request::Builder
Request::builder() {
  return std::move(Request::Builder());
}
// Methods -

// Operators +
Request&
Request::operator=(Request&& other) {
  _verb = std::move(other._verb);
  _headers = std::move(other._headers);
  _path = std::move(other._path);
  _input = other._input;
  _registry = other._registry;
  return *this;
}
// Operators -
// HTTP request -


// HTTP request builder +
// Constructors +
Request::Builder::Builder(): _request() {
  // Nothing to do
}

Request::Builder::Builder(Builder&& builder): _request(std::move(builder._request)) {
  // Nothing to do
}
// Constructors -

// Destructor +
Request::Builder::~Builder() {
  // Nothing to do
}
// Destructor -

// Methods +
Request::Builder
Request::Builder::path(const std::string& path) {
  _request._path = path;
  return std::move(*this);
}

Request::Builder
Request::Builder::verb(const std::string& verb) {
  _request._verb = verb;
  return std::move(*this);
}

Request::Builder
Request::Builder::header(const std::string& name, const std::string& value) {
  _request._headers.add(name, value);
  return std::move(*this);
}

Request::Builder
Request::Builder::header(const std::string& name, const std::vector<std::string>& values) {
  _request._headers.add(name, values);
  return std::move(*this);
}

Request::Builder
Request::Builder::headers(const anch::rest::Headers& headers) {
  _request._headers = headers;
  return std::move(*this);
}

Request::Builder
Request::Builder::contentType(const std::string& contentType) {
  _request.setContentType(contentType);
  return std::move(*this);
}

Request::Builder
Request::Builder::accept(const std::string& mediaType) {
  _request.setAccept({mediaType});
  return std::move(*this);
}

Request::Builder
Request::Builder::accept(const std::vector<std::string>& mediaTypes) {
  _request.setAccept(mediaTypes);
  return std::move(*this);
}

Request
Request::Builder::build() {
  return std::move(_request);
}
// Methods -
// HTTP request builder -
