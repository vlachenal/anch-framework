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

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "codec/thrift.hpp"

#include "rest/codec.hpp"

/*!
 * \file rest/thriftCodec.hpp
 * \deprecated removed in 0.4: Thrift codecs have moved to anch-codec-thrift, use \c codec/thrift.hpp
 */

namespace anch::rest {

  /*! \deprecated removed in 0.4, use \ref anch::codec::ThriftStruct */
  template<typename T>
  concept ThriftStruct [[deprecated("removed in 0.4, use anch::codec::ThriftStruct")]] = anch::codec::ThriftStruct<T>;

  /*! \deprecated removed in 0.4, use \ref anch::codec::ThriftProtocol */
  using ThriftProtocol [[deprecated("removed in 0.4, use anch::codec::ThriftProtocol")]] = anch::codec::ThriftProtocol;

  /*! \deprecated removed in 0.4, use \ref anch::codec::ThriftCodec */
  template<anch::codec::ThriftProtocol P>
  using ThriftCodec [[deprecated("removed in 0.4, use anch::codec::ThriftCodec")]] = anch::codec::ThriftCodec<P>;

  /*! \deprecated removed in 0.4, use \ref anch::codec::ThriftBinary */
  using ThriftBinary [[deprecated("removed in 0.4, use anch::codec::ThriftBinary")]] = anch::codec::ThriftBinary;

  /*! \deprecated removed in 0.4, use \ref anch::codec::ThriftCompact */
  using ThriftCompact [[deprecated("removed in 0.4, use anch::codec::ThriftCompact")]] = anch::codec::ThriftCompact;

  /*! \deprecated removed in 0.4, use \ref anch::codec::ThriftJson */
  using ThriftJson [[deprecated("removed in 0.4, use anch::codec::ThriftJson")]] = anch::codec::ThriftJson;

  /*!
   * \deprecated removed in 0.4, use \ref anch::codec::thriftMediaType
   *
   * \param protocol the protocol
   *
   * \return the media type
   */
  [[deprecated("removed in 0.4, use anch::codec::thriftMediaType")]]
  inline std::string_view
  thriftMediaType(anch::codec::ThriftProtocol protocol) noexcept {
    return anch::codec::thriftMediaType(protocol);
  }

  /*!
   * \deprecated removed in 0.4, use \ref anch::codec::thriftProtocolOf
   *
   * \param mediaType the media type
   *
   * \return the protocol, empty when media type is not a Thrift one
   */
  [[deprecated("removed in 0.4, use anch::codec::thriftProtocolOf")]]
  inline std::optional<anch::codec::ThriftProtocol>
  thriftProtocolOf(const anch::codec::MediaType& mediaType) {
    return anch::codec::thriftProtocolOf(mediaType);
  }

  /*!
   * \deprecated removed in 0.4, use \ref anch::codec::thriftProtocol
   *
   * \param protocol the protocol
   * \param transport the transport
   *
   * \return the protocol
   */
  [[deprecated("removed in 0.4, use anch::codec::thriftProtocol")]]
  inline std::shared_ptr<apache::thrift::protocol::TProtocol>
  thriftProtocol(anch::codec::ThriftProtocol protocol, std::shared_ptr<apache::thrift::transport::TTransport> transport) {
    return anch::codec::thriftProtocol(protocol, std::move(transport));
  }

  /*!
   * \deprecated removed in 0.4, use \ref anch::codec::thriftWrite
   *
   * \param protocol the protocol
   * \param write the write function
   *
   * \return the content
   */
  [[deprecated("removed in 0.4, use anch::codec::thriftWrite")]]
  inline std::string
  thriftWrite(anch::codec::ThriftProtocol protocol, const std::function<void(apache::thrift::protocol::TProtocol*)>& write) {
    return anch::codec::thriftWrite(protocol, write);
  }

  /*!
   * \deprecated removed in 0.4, use \ref anch::codec::thriftRead
   *
   * \param protocol the protocol
   * \param content the content
   * \param read the read function
   */
  [[deprecated("removed in 0.4, use anch::codec::thriftRead")]]
  inline void
  thriftRead(anch::codec::ThriftProtocol protocol, std::string_view content,
	     const std::function<void(apache::thrift::protocol::TProtocol*)>& read) {
    anch::codec::thriftRead(protocol, content, read);
  }

}
