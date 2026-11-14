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
#include "rest/grpcServer.hpp"

#include <cctype>
#include <stdexcept>
#include <vector>

#include <grpcpp/generic/callback_generic_service.h>
#include <grpcpp/support/byte_buffer.h>
#include <grpcpp/support/slice.h>

using anch::rest::GrpcServer;
using anch::rest::RpcCode;
using anch::rest::RpcContext;
using anch::rest::RpcError;
using anch::rest::RpcFormat;
using anch::rest::RpcService;

namespace {

  /*! Unary call: one request message, one response message */
  class UnaryReactor: public grpc::ServerGenericBidiReactor {
  private:
    grpc::GenericCallbackServerContext* _context;
    const std::map<std::string, const RpcService*, std::less<>>& _services;
    grpc::ByteBuffer _request;
    grpc::ByteBuffer _response;

  public:
    UnaryReactor(grpc::GenericCallbackServerContext* context, const std::map<std::string, const RpcService*, std::less<>>& services):
      _context(context), _services(services) {
      StartRead(&_request);
    }

    virtual void
    OnReadDone(bool ok) override {
      if(!ok) {
	end(grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Missing request message"));
	return;
      }
      try {
	process();
      } catch(const RpcError& error) {
	end(grpc::Status(static_cast<grpc::StatusCode>(error.getCode()), error.what()));
      } catch(...) {
	end(grpc::Status(grpc::StatusCode::INTERNAL, ""));
      }
    }

    virtual void
    OnWriteDone(bool) override {
      // Finished with the write
    }

    virtual void
    OnDone() override {
      delete this;
    }

  private:
    /*! Process call: response is written and call is finished (errors are raised before) */
    void
    process() {
      // Method: /package.Service/Method
      const std::string& path = _context->method();
      const std::size_t slash = path.rfind('/');
      if(path.empty() || path.front() != '/' || slash == 0 || slash == std::string::npos) {
	throw RpcError(RpcCode::UNIMPLEMENTED, "Unknown method " + path);
      }
      auto service = _services.find(std::string_view(path).substr(1, slash - 1));
      if(service == _services.end()) {
	throw RpcError(RpcCode::UNIMPLEMENTED, "Unknown method " + path);
      }
      // Request message and metadata
      std::vector<grpc::Slice> slices;
      if(!_request.Dump(&slices).ok()) {
	throw RpcError(RpcCode::INTERNAL, "");
      }
      std::string request;
      for(const grpc::Slice& slice : slices) {
	request.append(reinterpret_cast<const char*>(slice.begin()), slice.size());
      }
      anch::rest::Headers metadata;
      for(const auto& [name, value] : _context->client_metadata()) {
	metadata.add(std::string_view(name.data(), name.size()), std::string_view(value.data(), value.size()));
      }
      RpcContext context(std::move(metadata));
      std::string response = service->second->call(std::string_view(path).substr(slash + 1), request, RpcFormat::BINARY, context);
      for(const auto& [name, values] : context.getResponseMetadata()) {
	std::string key(name);
	for(char& c : key) {
	  c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}
	for(const std::string& value : values) {
	  _context->AddInitialMetadata(key, value);
	}
      }
      grpc::Slice slice(response);
      _response = grpc::ByteBuffer(&slice, 1);
      StartWriteAndFinish(&_response, grpc::WriteOptions(), grpc::Status::OK);
    }

    /*! Finish call with error status */
    void
    end(const grpc::Status& status) {
      Finish(status);
    }
  };

}

// Dispatcher +
class GrpcServer::Dispatcher: public grpc::CallbackGenericService {
private:
  const std::map<std::string, const RpcService*, std::less<>>& _services;

public:
  explicit Dispatcher(const std::map<std::string, const RpcService*, std::less<>>& services): _services(services) {
  }

  virtual grpc::ServerGenericBidiReactor*
  CreateReactor(grpc::GenericCallbackServerContext* context) override {
    return new UnaryReactor(context, _services);
  }
};
// Dispatcher -

// Constructors +
GrpcServer::GrpcServer(): _builder(), _services(), _dispatcher(), _server(), _port(0) {
  _dispatcher = std::make_unique<Dispatcher>(_services);
}
// Constructors -

// Destructor +
GrpcServer::~GrpcServer() {
  shutdown();
}
// Destructor -

// Methods +
GrpcServer&
GrpcServer::addService(const RpcService& service) {
  if(!_services.emplace(service.getName(), &service).second) {
    throw std::invalid_argument("RPC service " + service.getName() + " already exists");
  }
  return *this;
}

GrpcServer&
GrpcServer::listen(const std::string& address, std::shared_ptr<grpc::ServerCredentials> credentials) {
  _builder.AddListeningPort(address, std::move(credentials), _port == 0 ? &_port : nullptr);
  return *this;
}

grpc::ServerBuilder&
GrpcServer::builder() noexcept {
  return _builder;
}

int
GrpcServer::start() {
  std::size_t maxMessageSize = 0;
  for(const auto& [name, service] : _services) {
    maxMessageSize = std::max(maxMessageSize, service->getMaxMessageSize());
  }
  if(maxMessageSize != 0) {
    _builder.SetMaxReceiveMessageSize(static_cast<int>(std::min<std::size_t>(maxMessageSize, INT32_MAX)));
  }
  _builder.RegisterCallbackGenericService(_dispatcher.get());
  _server = _builder.BuildAndStart();
  if(!_server) {
    throw std::runtime_error("Unable to start gRPC server");
  }
  return _port;
}

void
GrpcServer::wait() {
  if(_server) {
    _server->Wait();
  }
}

void
GrpcServer::shutdown() {
  if(_server) {
    _server->Shutdown();
    _server.reset();
  }
}

int
GrpcServer::getPort() const noexcept {
  return _port;
}
// Methods -
