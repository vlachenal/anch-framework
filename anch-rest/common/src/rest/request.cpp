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
#include "rest/request.hpp"

#include <sstream>
#include <stdexcept>

using anch::rest::Headers;
using anch::codec::MediaType;
using anch::rest::Method;
using anch::rest::Request;

namespace {

  int
  hexValue(char c) noexcept {
    if(c >= '0' && c <= '9') {
      return c - '0';
    }
    if(c >= 'a' && c <= 'f') {
      return c - 'a' + 10;
    }
    if(c >= 'A' && c <= 'F') {
      return c - 'A' + 10;
    }
    return -1;
  }

}

// Request +
Request::Request(): Request(Method::GET, "/") {
  // Nothing to do
}

Request::Request(Method method, std::string_view target):
  _method(method),
  _path(),
  _query(),
  _headers(),
  _body(),
  _pathParameters() {
  setTarget(target);
}

void
Request::setTarget(std::string_view target) {
  const std::size_t fragment = target.find('#');
  if(fragment != std::string_view::npos) {
    target = target.substr(0, fragment);
  }
  _query.clear();
  const std::size_t question = target.find('?');
  _path = target.substr(0, question);
  if(question == std::string_view::npos) {
    return;
  }
  std::string_view query = target.substr(question + 1);
  while(!query.empty()) {
    const std::size_t amp = query.find('&');
    std::string_view param = query.substr(0, amp);
    query = (amp == std::string_view::npos) ? std::string_view() : query.substr(amp + 1);
    if(param.empty()) {
      continue;
    }
    const std::size_t equal = param.find('=');
    std::string_view name = param.substr(0, equal);
    std::string_view value = (equal == std::string_view::npos) ? std::string_view() : param.substr(equal + 1);
    // Invalid encoding is kept as is
    _query.emplace_back(anch::rest::percentDecode(name, true).value_or(std::string(name)),
			anch::rest::percentDecode(value, true).value_or(std::string(value)));
  }
}

std::optional<std::string_view>
Request::getQueryParameter(std::string_view name) const {
  for(const auto& [key, value] : _query) {
    if(key == name) {
      return std::string_view(value);
    }
  }
  return std::nullopt;
}

void
Request::addQueryParameter(std::string_view name, std::string_view value) {
  _query.emplace_back(name, value);
}

std::optional<std::string_view>
Request::getPathParameter(std::string_view name) const {
  auto iter = _pathParameters.find(name);
  if(iter == _pathParameters.end()) {
    return std::nullopt;
  }
  return std::string_view(iter->second);
}

std::optional<MediaType>
Request::getContentType() const {
  std::optional<std::string_view> header = _headers.get(anch::rest::header::CONTENT_TYPE);
  if(!header) {
    return std::nullopt;
  }
  return MediaType::parse(*header);
}

std::vector<MediaType>
Request::getAccept() const {
  std::optional<std::string> header = _headers.format(anch::rest::header::ACCEPT);
  if(!header) {
    return std::vector<MediaType>();
  }
  return MediaType::parseAccept(*header);
}

bool
Request::hasBody() const noexcept {
  return static_cast<bool>(_body);
}

std::istream&
Request::getBody() const {
  if(!_body) {
    throw std::logic_error("Request has no body");
  }
  return *_body;
}

void
Request::setBody(std::unique_ptr<std::istream> body) {
  _body = std::move(body);
}

void
Request::setBody(std::string body) {
  _body = std::make_unique<std::istringstream>(std::move(body));
}

Method
Request::getMethod() const noexcept {
  return _method;
}

void
Request::setMethod(Method method) noexcept {
  _method = method;
}

const std::string&
Request::getPath() const noexcept {
  return _path;
}

void
Request::setPath(std::string_view path) {
  _path = path;
}

const std::vector<std::pair<std::string, std::string>>&
Request::getQueryParameters() const noexcept {
  return _query;
}

Headers&
Request::getHeaders() noexcept {
  return _headers;
}

const Headers&
Request::getHeaders() const noexcept {
  return _headers;
}

const std::map<std::string, std::string, std::less<>>&
Request::getPathParameters() const noexcept {
  return _pathParameters;
}

void
Request::setPathParameters(std::map<std::string, std::string, std::less<>> parameters) {
  _pathParameters = std::move(parameters);
}
// Request -

std::optional<std::string>
anch::rest::percentDecode(std::string_view value, bool plusAsSpace) {
  std::string res;
  res.reserve(value.size());
  for(std::size_t i = 0 ; i < value.size() ; ++i) {
    const char c = value[i];
    if(c == '%') {
      if(i + 2 >= value.size()) {
	return std::nullopt;
      }
      const int high = hexValue(value[i + 1]);
      const int low = hexValue(value[i + 2]);
      if(high < 0 || low < 0) {
	return std::nullopt;
      }
      res += static_cast<char>(high * 16 + low);
      i += 2;
    } else if(plusAsSpace && c == '+') {
      res += ' ';
    } else {
      res += c;
    }
  }
  return res;
}

std::string
anch::rest::percentEncode(std::string_view value) {
  static constexpr std::string_view HEX = "0123456789ABCDEF";
  std::string res;
  res.reserve(value.size());
  for(char c : value) {
    const bool unreserved = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')
      || c == '-' || c == '.' || c == '_' || c == '~';
    if(unreserved) {
      res += c;
    } else {
      const auto byte = static_cast<unsigned char>(c);
      res += '%';
      res += HEX[byte >> 4];
      res += HEX[byte & 0x0F];
    }
  }
  return res;
}
