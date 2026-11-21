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

#include "codec/protobuf.hpp"

#include "rest/codec.hpp"

/*!
 * \file rest/protobufCodec.hpp
 * \deprecated removed in 0.4: Protocol Buffers codecs have moved to anch-codec-protobuf, use \c codec/protobuf.hpp
 */

namespace anch::rest {

  /*! \deprecated removed in 0.4, use \ref anch::codec::ProtobufMessage */
  template<typename T>
  concept ProtobufMessage [[deprecated("removed in 0.4, use anch::codec::ProtobufMessage")]] = anch::codec::ProtobufMessage<T>;

  /*! \deprecated removed in 0.4, use \ref anch::codec::Protobuf */
  using Protobuf [[deprecated("removed in 0.4, use anch::codec::Protobuf")]] = anch::codec::Protobuf;

  /*! \deprecated removed in 0.4, use \ref anch::codec::ProtobufJson */
  using ProtobufJson [[deprecated("removed in 0.4, use anch::codec::ProtobufJson")]] = anch::codec::ProtobufJson;

}
