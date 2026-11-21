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
#include "codec/thrift.hpp"

#include <stdexcept>
#include <vector>

#include <thrift/Thrift.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/transport/TBufferTransports.h>

#include "codec/format.hpp"

using anch::codec::ThriftProtocol;
using apache::thrift::protocol::TProtocol;
using apache::thrift::transport::TMemoryBuffer;
using apache::thrift::transport::TTransport;

namespace {

  // Registered when the library is loaded: a program which uses Thrift codecs links this file (thriftWrite, ...)
  [[maybe_unused]] const bool REGISTERED = anch::codec::registerFormat(anch::codec::Format{"thrift", {
	std::string(anch::codec::thriftMediaType(ThriftProtocol::BINARY)),
	std::string(anch::codec::thriftMediaType(ThriftProtocol::COMPACT)),
	std::string(anch::codec::thriftMediaType(ThriftProtocol::JSON))
      }});

}

std::string_view
anch::codec::thriftMediaType(ThriftProtocol protocol) noexcept {
  switch(protocol) {
  case ThriftProtocol::BINARY: return "application/vnd.apache.thrift.binary";
  case ThriftProtocol::COMPACT: return "application/vnd.apache.thrift.compact";
  case ThriftProtocol::JSON: return "application/vnd.apache.thrift.json";
  }
  return "application/vnd.apache.thrift.binary";
}

std::optional<ThriftProtocol>
anch::codec::thriftProtocolOf(const anch::codec::MediaType& mediaType) {
  const std::string essence = mediaType.essence();
  if(essence == "application/vnd.apache.thrift.binary" || essence == "application/x-thrift") {
    return ThriftProtocol::BINARY;
  }
  if(essence == "application/vnd.apache.thrift.compact") {
    return ThriftProtocol::COMPACT;
  }
  if(essence == "application/vnd.apache.thrift.json") {
    return ThriftProtocol::JSON;
  }
  return std::nullopt;
}

std::shared_ptr<TProtocol>
anch::codec::thriftProtocol(ThriftProtocol protocol, std::shared_ptr<TTransport> transport) {
  switch(protocol) {
  case ThriftProtocol::COMPACT:
    return std::make_shared<apache::thrift::protocol::TCompactProtocol>(std::move(transport));
  case ThriftProtocol::JSON:
    return std::make_shared<apache::thrift::protocol::TJSONProtocol>(std::move(transport));
  case ThriftProtocol::BINARY:
    break;
  }
  return std::make_shared<apache::thrift::protocol::TBinaryProtocol>(std::move(transport));
}

std::string
anch::codec::thriftWrite(ThriftProtocol protocol, const std::function<void(TProtocol*)>& write) {
  auto buffer = std::make_shared<TMemoryBuffer>();
  std::shared_ptr<TProtocol> encoder = anch::codec::thriftProtocol(protocol, buffer);
  try {
    write(encoder.get());
  } catch(const apache::thrift::TException& error) {
    // Unset required field, ...
    throw std::invalid_argument(std::string("Invalid Thrift structure: ") + error.what());
  }
  return buffer->getBufferAsString();
}

void
anch::codec::thriftRead(ThriftProtocol protocol, std::string_view content, const std::function<void(TProtocol*)>& read) {
  std::vector<uint8_t> data(content.begin(), content.end());
  auto buffer = std::make_shared<TMemoryBuffer>(data.data(), static_cast<uint32_t>(data.size()), TMemoryBuffer::COPY);
  std::shared_ptr<TProtocol> decoder = anch::codec::thriftProtocol(protocol, buffer);
  try {
    read(decoder.get());
  } catch(const apache::thrift::TException& error) {
    throw std::invalid_argument(std::string("Invalid Thrift structure: ") + error.what());
  }
}
