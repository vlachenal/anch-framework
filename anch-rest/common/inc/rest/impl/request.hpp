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
#pragma once

namespace anch::rest {

  inline const std::string&
  Request::getVerb() const {
    return _verb;
  }

  inline void
  Request::setVerb(const std::string& verb) {
    _verb = verb;
  }

  inline const anch::rest::Headers&
  Request::getHeaders() const {
    return _headers;
  }

  inline void
  Request::setHeaders(const anch::rest::Headers& headers) {
    _headers = headers;
  }

  inline std::string
  Request::getContentType() const {
    auto res = _headers.get(anch::rest::CommonHeaders::CONTENT_TYPE);
    return (!res.has_value() || res.value().empty()) ? anch::rest::EMPTY_VALUE : res.value()[0];
  }

  inline void
  Request::setContentType(const std::string& contentType) {
    _headers.put(anch::rest::CommonHeaders::CONTENT_TYPE, contentType);
  }

  inline const std::optional<const std::vector<std::string>>
  Request::getAccept() const {
    return _headers.get(anch::rest::CommonHeaders::ACCEPT);
  }

  inline void
  Request::setAccept(const std::vector<std::string>& accept) {
    _headers.put(anch::rest::CommonHeaders::ACCEPT, accept);
  }

  inline const std::string&
  Request::getPath() const {
    return _path;
  }

  inline void
  Request::setPath(const std::string& path) {
    _path = path;
  }

  inline const std::vector<std::string>&
  Request::getParameters() const {
    return _parameters;
  }

  inline void
  Request::setParameters(const std::vector<std::string>& parameters) {
    _parameters = parameters;
  }

  template<typename T>
  T
  Request::getBody() {
    if(!_input) {
      throw 500; // \todo raise error
    }
    if constexpr (std::is_same<std::string, T>::value) {
      std::ostringstream oss;
      char read = _input->get();
      while(*_input) {
	oss << read;
	read = _input->get();
      }
      return oss.str();
    } else if constexpr (std::is_base_of<std::istream, T>::value) {
      return *_input;
    } else {
      MapperRegistry* reg = _registry;
      if(reg == NULL) {
	reg = &anch::rest::GlobalMapperRegistry::getInstance();
      }
      return reg->deserialize<T>(*_input);
    }
  }

}  // anch::rest
