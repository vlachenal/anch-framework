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

#include <iterator>

namespace anch::rest {

  template<anch::rest::ThriftProtocol P>
  std::string_view
  ThriftCodec<P>::mediaType() noexcept {
    return anch::rest::thriftMediaType(P);
  }

  template<anch::rest::ThriftProtocol P>
  template<anch::rest::ThriftStruct T>
  void
  ThriftCodec<P>::serialize(const T& value, std::ostream& out) {
    out << anch::rest::thriftWrite(P, [&value](apache::thrift::protocol::TProtocol* protocol) { value.write(protocol); });
  }

  template<anch::rest::ThriftProtocol P>
  template<anch::rest::ThriftStruct T>
  void
  ThriftCodec<P>::deserialize(T& value, std::istream& input) {
    const std::string content((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    anch::rest::thriftRead(P, content, [&value](apache::thrift::protocol::TProtocol* protocol) { value.read(protocol); });
  }

}
