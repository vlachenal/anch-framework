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

  inline
  uint16_t
  Response::getStatus() const {
    return _code;
  }

  inline
  void
  Response::setStatus(uint16_t code) {
    _code = code;
  }

  inline const anch::rest::Headers&
  Response::getHeaders() const {
    return _headers;
  }

  inline void
  Response::setHeaders(const anch::rest::Headers& headers) {
    _headers = headers;
  }

  inline
  std::string
  Response::getContentType() const {
    auto res = _headers.get(anch::rest::CommonHeaders::CONTENT_TYPE);
    return (!res.has_value() || res.value().empty()) ? anch::rest::EMPTY_VALUE : res.value()[0];
  }

  inline
  void
  Response::setContentType(const std::string& contentType) {
    _headers.put(anch::rest::CommonHeaders::CONTENT_TYPE, contentType);
  }

  template<typename T>
  void
  Response::setBody(T body) {
    if constexpr (std::is_same<std::string, T>::value
		  || std::is_same<std::string&, T>::value
		  || std::is_same<const std::string&, T>::value
		  || std::is_same<const char*, T>::value
		  || std::is_same<const char* const, T>::value
		  || std::is_same<char*, T>::value
		  || std::is_same<const uint8_t*, T>::value
		  || std::is_same<const uint8_t* const, T>::value
		  || std::is_same<uint8_t*, T>::value) {
      _bodyWriter = std::function<void(std::ostream&)>([body](std::ostream& out) {
	out << body;
      });
    } else if constexpr (std::is_base_of<std::istream, T>::value) {
      _input = std::make_shared<T>(std::move(body));
      _bodyWriter = std::function<void(std::ostream&)>([this](std::ostream& out) {
	int read = _input->get();
	while(*_input) {
	  out << static_cast<uint8_t>(read);
	  read = _input->get();
	}
      });
    } else {
      MapperRegistry* reg = _registry;
      if(reg == NULL) {
	reg = &anch::rest::GlobalMapperRegistry::getInstance();
      }
      _bodyWriter = std::bind(&anch::rest::MapperRegistry::serialize<T>,
			      *reg, getContentType(), body,
			      std::placeholders::_1);
    }
  }

  template<typename T>
  Response::Builder
  Response::Builder::body(T body) {
    if constexpr (std::is_base_of<std::istream, T>::value) {
      _response.setBody(std::move(body));
    } else {
      _response.setBody(body);
    }
    return std::move(*this);
  }

}
