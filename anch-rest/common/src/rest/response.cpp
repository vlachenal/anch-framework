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
#include "rest/response.hpp"

using anch::rest::Response;


// HTTP Response +
// Constructors +
Response::Response():
  _code(500),
  _headers(),
  _registry(NULL),
  _bodyWriter(),
  _input() {
  // Nothing to do
}

Response::Response(uint16_t code):
  _code(code),
  _headers(),
  _registry(NULL),
  _bodyWriter(),
  _input() {
  // Nothing to do
}

Response::Response(Response&& res):
  _code(res._code),
  _headers(std::move(res._headers)),
  _registry(res._registry),
  _bodyWriter(res._bodyWriter),
  _input(res._input)  {
  // Nothing to do
}
// Constructors -

// Destructor +
Response::~Response() {
  // Nothing to do
}
// Destructor -

// Operators +
Response&
Response::operator=(Response&& other) {
  _code = other._code;
  _headers = std::move(other._headers);
  _registry = other._registry;
  _bodyWriter = other._bodyWriter;
  _input = other._input;
  return *this;
}
// Operators -

// Methods +
Response::Builder
Response::builder() {
  return std::move(Response::Builder());
}

void
Response::writeBody(std::ostream& out) const {
  _bodyWriter(out);
}
// Methods -
// HTTP Response -


// HTTP reponse builder +
// Constructors +
Response::Builder::Builder(): _response() {
  // Nothing to do
}

Response::Builder::Builder(Builder&& builder): _response(std::move(builder._response)) {
  // Nothing to do
}
// Constructors -

// Destructor +
Response::Builder::~Builder() {
  // Nothing to do
}
// Destructor -

// Methods +
Response::Builder
Response::Builder::status(uint16_t code) {
  _response._code = code;
  return std::move(*this);
}

Response::Builder
Response::Builder::header(const std::string& name, const std::string& value) {
  _response._headers.add(name, value);
  return std::move(*this);
}

Response::Builder
Response::Builder::header(const std::string& name, const std::vector<std::string>& values) {
  _response._headers.add(name, values);
  return std::move(*this);
}

Response::Builder
Response::Builder::headers(const anch::rest::Headers& headers) {
  _response._headers = headers;
  return std::move(*this);
}

Response::Builder
Response::Builder::body(const std::string& body) {
  _response.setBody(body);
  return std::move(*this);
}

Response::Builder
Response::Builder::body(const char* const body) {
  _response.setBody(std::string(body));
  return std::move(*this);
}

Response::Builder
Response::Builder::contentType(const std::string& contentType) {
  _response.setContentType(contentType);
  return std::move(*this);
}

Response
Response::Builder::build() {
  return std::move(_response);
}
// Methods -
// HTTP reponse builder -
