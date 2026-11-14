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

#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "rest/headers.hpp"
#include "rest/protobufCodec.hpp"
#include "rest/status.hpp"

namespace anch::rest {

  /*!
   * RPC status codes (gRPC codes, shared by gRPC, gRPC-Web and Connect protocols)
   *
   * \since 0.2
   */
  enum class RpcCode: uint8_t {
    /*! Success */
    OK = 0,
    /*! Call cancelled */
    CANCELLED = 1,
    /*! Unknown error */
    UNKNOWN = 2,
    /*! Invalid argument */
    INVALID_ARGUMENT = 3,
    /*! Deadline exceeded */
    DEADLINE_EXCEEDED = 4,
    /*! Not found */
    NOT_FOUND = 5,
    /*! Already exists */
    ALREADY_EXISTS = 6,
    /*! Permission denied */
    PERMISSION_DENIED = 7,
    /*! Resource exhausted */
    RESOURCE_EXHAUSTED = 8,
    /*! Failed precondition */
    FAILED_PRECONDITION = 9,
    /*! Aborted */
    ABORTED = 10,
    /*! Out of range */
    OUT_OF_RANGE = 11,
    /*! Not implemented */
    UNIMPLEMENTED = 12,
    /*! Internal error */
    INTERNAL = 13,
    /*! Unavailable */
    UNAVAILABLE = 14,
    /*! Data loss */
    DATA_LOSS = 15,
    /*! Not authenticated */
    UNAUTHENTICATED = 16
  };

  /*!
   * RPC error: raised by RPC handlers (sent to the client with its code and message) and by RPC
   * clients
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class RpcError: public std::runtime_error {

    // Attributes +
  private:
    /*! Status code */
    anch::rest::RpcCode _code;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref RpcError constructor
     *
     * \param code the status code
     * \param message the message (sent to the client)
     */
    RpcError(anch::rest::RpcCode code, const std::string& message = std::string());
    // Constructors -

    // Accessors +
  public:
    /*!
     * Get status code
     *
     * \return the code
     */
    anch::rest::RpcCode getCode() const noexcept;
    // Accessors -

  };

  /*!
   * RPC call context: request metadata (HTTP headers, gRPC metadata) and response metadata
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class RpcContext {

    // Attributes +
  private:
    /*! Request metadata */
    anch::rest::Headers _request;

    /*! Response metadata */
    anch::rest::Headers _response;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref RpcContext constructor
     *
     * \param request the request metadata
     */
    explicit RpcContext(anch::rest::Headers request = anch::rest::Headers());
    // Constructors -

    // Accessors +
  public:
    /*!
     * Get request metadata
     *
     * \return the metadata
     */
    const anch::rest::Headers& getRequestMetadata() const noexcept;

    /*!
     * Get response metadata, to complete (HTTP headers, gRPC initial metadata: names in lower case)
     *
     * \return the metadata
     */
    anch::rest::Headers& getResponseMetadata() noexcept;
    // Accessors -

  };

  /*!
   * Messages encoding of an RPC call
   *
   * \since 0.2
   */
  enum class RpcFormat {
    /*! Protocol Buffers binary encoding */
    BINARY,
    /*! Protocol Buffers JSON mapping */
    JSON
  };

  /*!
   * RPC service: unary methods of a Protocol Buffers service, served by \ref installRpc (Connect and
   * gRPC-Web in \ref Api , HTTP/1.1) and by \ref GrpcServer (gRPC, HTTP/2) with the same handlers.
   * \code{.cpp}
anch::rest::RpcService service("pets.PetStore");
service.unary<pets::GetPetRequest, pets::Pet>("GetPet", [](const pets::GetPetRequest& request, anch::rest::RpcContext&) {
  if(request.id() != 1) {
    throw anch::rest::RpcError(anch::rest::RpcCode::NOT_FOUND, "no pet");
  }
  pets::Pet pet;
  pet.set_id(1);
  return pet;
});
anch::rest::installRpc(api, service); // POST /pets.PetStore/GetPet
   * \endcode
   * Handler errors: \ref RpcError is sent with its code and message, \ref HttpException with the
   * code of its status ( \c 400 is \c INVALID_ARGUMENT , \c 404 is \c NOT_FOUND , ...), other
   * exceptions are \c INTERNAL without message.\n
   * \n
   * Methods have to be declared before calls; calls can then be made from several threads.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class RpcService {

    /*! Method: request bytes to response bytes */
    using Method = std::function<std::string(std::string_view, anch::rest::RpcFormat, anch::rest::RpcContext&)>;

    // Attributes +
  private:
    /*! Service full name ( \c package.Service ) */
    std::string _name;

    /*! Methods by name */
    std::map<std::string, Method, std::less<>> _methods;

    /*! Maximum request message size */
    std::size_t _maxMessageSize;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref RpcService constructor
     *
     * \param name the service full name ( \c package.Service )
     *
     * \throw std::invalid_argument when name is empty or contains \c /
     */
    explicit RpcService(std::string_view name);
    // Constructors -

    // Methods +
  public:
    /*!
     * Declare unary method
     *
     * \tparam Req the request message type
     * \tparam Res the response message type
     *
     * \param method the method name ( \c GetPet )
     * \param handler the handler: <tt>Res(const Req&, RpcContext&)</tt> or <tt>Res(const Req&)</tt>
     *
     * \return \c this
     *
     * \throw std::invalid_argument when method name is invalid or already declared
     */
    template<anch::rest::ProtobufMessage Req, anch::rest::ProtobufMessage Res, typename F>
    RpcService& unary(std::string_view method, F&& handler);

    /*!
     * Call method
     *
     * \param method the method name
     * \param request the request message
     * \param format the messages encoding
     * \param context the call context
     *
     * \return the response message
     *
     * \throw RpcError on error ( \c UNIMPLEMENTED for unknown method, \c INVALID_ARGUMENT for invalid
     *        request, \c RESOURCE_EXHAUSTED for request larger than maximum message size, handler errors)
     */
    std::string call(std::string_view method, std::string_view request, anch::rest::RpcFormat format,
		     anch::rest::RpcContext& context) const;

    /*!
     * Check if service has a method
     *
     * \param method the method name
     *
     * \return \c true when method is declared
     */
    bool has(std::string_view method) const;

    /*!
     * Get methods names
     *
     * \return the methods names, sorted
     */
    std::vector<std::string> getMethods() const;
    // Methods -

    // Accessors +
  public:
    /*!
     * Get service full name
     *
     * \return the name
     */
    const std::string& getName() const noexcept;

    /*!
     * Get maximum request message size (default: 4 MiB, as gRPC)
     *
     * \return the size in bytes
     */
    std::size_t getMaxMessageSize() const noexcept;

    /*!
     * Set maximum request message size
     *
     * \param size the size in bytes
     */
    void setMaxMessageSize(std::size_t size) noexcept;
    // Accessors -

  private:
    /*!
     * Register method
     *
     * \param method the method name
     * \param function the method
     */
    void add(std::string_view method, Method function);
    // Methods -

  };

}

namespace anch::rest::rpc {

  /*!
   * Connect error code name ( \c not_found , ...)
   *
   * \param code the code
   *
   * \return the name
   *
   * \since 0.2
   */
  std::string_view connectCode(anch::rest::RpcCode code) noexcept;

  /*!
   * Parse Connect error code name
   *
   * \param name the name
   *
   * \return the code, empty when name is unknown
   *
   * \since 0.2
   */
  std::optional<anch::rest::RpcCode> parseConnectCode(std::string_view name) noexcept;

  /*!
   * HTTP status of a Connect error
   *
   * \param code the code
   *
   * \return the HTTP status
   *
   * \since 0.2
   */
  anch::rest::Status httpStatus(anch::rest::RpcCode code) noexcept;

  /*!
   * RPC code of an HTTP status (errors raised outside RPC handlers)
   *
   * \param status the HTTP status
   *
   * \return the code
   *
   * \since 0.2
   */
  anch::rest::RpcCode rpcCode(anch::rest::Status status) noexcept;

  /*!
   * Connect error body: <tt>{"code":"not_found","message":"..."}</tt>
   *
   * \param error the error
   *
   * \return the JSON body
   *
   * \since 0.2
   */
  std::string connectError(const anch::rest::RpcError& error);

  /*!
   * Parse Connect error body
   *
   * \param status the HTTP status
   * \param body the response body
   *
   * \return the error (code of the HTTP status when body is not a Connect error)
   *
   * \since 0.2
   */
  anch::rest::RpcError parseConnectError(anch::rest::Status status, std::string_view body);

  /*!
   * gRPC-Web frame: flag byte, big endian length (4 bytes) and data
   *
   * \param data the frame data
   * \param trailers trailers frame (flag \c 0x80 ), data frame otherwise
   *
   * \return the frame
   *
   * \since 0.2
   */
  std::string grpcWebFrame(std::string_view data, bool trailers);

  /*!
   * Read gRPC-Web request message frame
   *
   * \param body the request body
   *
   * \return the message
   *
   * \throw RpcError \c INVALID_ARGUMENT when body is not a single uncompressed message frame
   *
   * \since 0.2
   */
  std::string grpcWebMessage(std::string_view body);

  /*!
   * gRPC-Web trailers frame data: \c grpc-status and \c grpc-message (percent-encoded) lines
   *
   * \param code the status code
   * \param message the status message
   *
   * \return the trailers
   *
   * \since 0.2
   */
  std::string grpcWebTrailers(anch::rest::RpcCode code, std::string_view message);

}

#include "rest/impl/rpcService.hpp"
