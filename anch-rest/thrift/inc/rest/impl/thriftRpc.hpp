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

  inline void
  installThrift(anch::rest::Api& api, std::string_view path, std::shared_ptr<apache::thrift::TProcessor> processor,
		std::size_t maxMessageSize) {
    api.post(path, [processor = std::move(processor), maxMessageSize](const anch::rest::Request& request, std::istream& body) {
      std::optional<anch::codec::MediaType> type = request.getContentType();
      std::optional<anch::codec::ThriftProtocol> protocol = type ? anch::codec::thriftProtocolOf(*type) : std::nullopt;
      if(!protocol) {
	throw anch::rest::HttpException(anch::rest::Status::UNSUPPORTED_MEDIA_TYPE);
      }
      std::string content = anch::codec::readContent(body, maxMessageSize);
      std::vector<uint8_t> data(content.begin(), content.end());
      auto input = std::make_shared<apache::thrift::transport::TMemoryBuffer>(data.data(), static_cast<uint32_t>(data.size()),
									      apache::thrift::transport::TMemoryBuffer::COPY);
      auto output = std::make_shared<apache::thrift::transport::TMemoryBuffer>();
      try {
	if(!processor->process(anch::codec::thriftProtocol(*protocol, input), anch::codec::thriftProtocol(*protocol, output), nullptr)) {
	  throw anch::rest::HttpException(anch::rest::Status::BAD_REQUEST, "Invalid Thrift message");
	}
      } catch(const apache::thrift::TException& error) {
	throw anch::rest::HttpException(anch::rest::Status::BAD_REQUEST, std::string("Invalid Thrift message: ") + error.what());
      }
      return anch::rest::Response(anch::rest::Status::OK).contentType(type->essence()).body(output->getBufferAsString());
    });
  }

}
