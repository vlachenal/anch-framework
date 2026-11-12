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
#include "rest/client.hpp"
#include "rest/auth.hpp"

using anch::rest::Client;
using anch::rest::Filter;
using anch::rest::HttpError;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Transport;

// Transport +
Transport::~Transport() {
  // Nothing to do
}
// Transport -

// HttpError +
HttpError::HttpError(Response response):
  anch::rest::HttpException(response.getStatus(),
			    "HTTP " + std::to_string(anch::rest::code(response.getStatus())) + ' '
			    + std::string(anch::rest::reasonPhrase(response.getStatus()))),
  _response(std::move(response)) {
  // Nothing to do
}

const Response&
HttpError::getResponse() const noexcept {
  return _response;
}
// HttpError -

// Client +
Client::Client(std::unique_ptr<Transport> transport, std::string_view baseUrl):
  _transport(std::move(transport)),
  _baseUrl(baseUrl),
  _headers(),
  _filters() {
  while(!_baseUrl.empty() && _baseUrl.back() == '/') {
    _baseUrl.pop_back();
  }
  _headers.set(anch::rest::header::USER_AGENT, "anch-rest");
}

Client&
Client::header(std::string_view name, std::string_view value) {
  _headers.set(name, value);
  return *this;
}

Client&
Client::filter(Filter filter) {
  _filters.push_back(std::move(filter));
  return *this;
}

Response
Client::send(Request& request) {
  for(const auto& [name, values] : _headers) {
    if(!request.getHeaders().has(name)) {
      for(const std::string& value : values) {
	request.getHeaders().add(name, value);
      }
    }
  }
  return next(request, 0);
}

Response
Client::next(Request& request, std::size_t index) {
  if(index == _filters.size()) {
    return _transport->send(url(request), request);
  }
  return _filters[index](request, [this, index](Request& req) {
    return next(req, index + 1);
  });
}

std::string
Client::url(const Request& request) const {
  std::string res = _baseUrl;
  if(request.getPath().empty() || request.getPath().front() != '/') {
    res += '/';
  }
  res += request.getPath();
  char separator = '?';
  for(const auto& [name, value] : request.getQueryParameters()) {
    res += separator;
    res += anch::rest::percentEncode(name);
    res += '=';
    res += anch::rest::percentEncode(value);
    separator = '&';
  }
  return res;
}

std::string
Client::formatPath(std::string_view path, const std::vector<std::string>& parameters) {
  std::string res;
  std::size_t index = 0;
  std::size_t pos = 0;
  while(pos < path.size()) {
    const std::size_t open = path.find('{', pos);
    if(open == std::string_view::npos) {
      res += path.substr(pos);
      break;
    }
    const std::size_t close = path.find('}', open);
    if(close == std::string_view::npos) {
      throw std::invalid_argument("Invalid path '" + std::string(path) + "': unclosed placeholder");
    }
    if(index >= parameters.size()) {
      throw std::invalid_argument("Invalid path '" + std::string(path) + "': missing parameter for "
				  + std::string(path.substr(open, close - open + 1)));
    }
    res += path.substr(pos, open - pos);
    res += anch::rest::percentEncode(parameters[index++]);
    pos = close + 1;
  }
  if(index != parameters.size()) {
    throw std::invalid_argument("Invalid path '" + std::string(path) + "': " + std::to_string(parameters.size())
				+ " parameter(s) for " + std::to_string(index) + " placeholder(s)");
  }
  return res;
}
// Client -
