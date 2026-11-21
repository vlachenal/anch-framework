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

namespace anch::codec {

  template<anch::codec::ThriftProtocol P>
  std::string_view
  ThriftCodec<P>::mediaType() noexcept {
    return anch::codec::thriftMediaType(P);
  }

  template<anch::codec::ThriftProtocol P>
  template<anch::codec::ThriftStruct T>
  void
  ThriftCodec<P>::serialize(const T& value, std::ostream& out) {
    out << anch::codec::thriftWrite(P, [&value](apache::thrift::protocol::TProtocol* protocol) { value.write(protocol); });
  }

  template<anch::codec::ThriftProtocol P>
  template<anch::codec::ThriftStruct T>
  void
  ThriftCodec<P>::deserialize(T& value, std::istream& input, anch::codec::Side side) {
    const std::string content = anch::codec::readBody(input, side);
    anch::codec::thriftRead(P, content, [&value](apache::thrift::protocol::TProtocol* protocol) { value.read(protocol); });
  }

}
