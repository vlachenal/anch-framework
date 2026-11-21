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
#pragma once

namespace anch::rest {

  inline
  ThriftTransport::ThriftTransport(anch::rest::Client& client, std::string path, anch::codec::ThriftProtocol protocol):
    _client(client), _path(std::move(path)), _contentType(anch::codec::thriftMediaType(protocol)), _request(), _response(), _position(0) {
    // Nothing to do
  }

  inline bool
  ThriftTransport::isOpen() const {
    return true;
  }

  inline void
  ThriftTransport::open() {
    // Nothing to do
  }

  inline void
  ThriftTransport::close() {
    // Nothing to do
  }

  inline uint32_t
  ThriftTransport::read(uint8_t* buffer, uint32_t length) {
    const std::size_t count = std::min<std::size_t>(length, _response.size() - _position);
    std::memcpy(buffer, _response.data() + _position, count);
    _position += count;
    return static_cast<uint32_t>(count);
  }

  inline void
  ThriftTransport::write(const uint8_t* buffer, uint32_t length) {
    _request.append(reinterpret_cast<const char*>(buffer), length);
  }

  inline void
  ThriftTransport::flush() {
    std::string request = std::move(_request);
    _request.clear();
    anch::rest::Response response = _client.post(_path).body(std::move(request), _contentType).send();
    if(anch::rest::code(response.getStatus()) >= 400) {
      throw anch::rest::HttpError(std::move(response));
    }
    _response = response.content();
    _position = 0;
  }

}
