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

#include <string>
#include <sstream>
#include <functional>
#include <ostream>
#include <istream>
#include <memory>
#include <map>

#include "rest/headers.hpp"
#include "rest/mapperRegistry.hpp"


namespace anch::rest {

  /*!
   * \brief HTTP response
   *
   * HTTP response representation
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class Response {
    class Builder;

    // Attributes +
  private:
    /*! HTTP return code */
    uint16_t _code;

    /*! HTTP headers */
    anch::rest::Headers _headers;

    /*! Mapper registry */
    anch::rest::MapperRegistry* _registry;

    /*! Response body */
    std::function<void(std::ostream&)> _bodyWriter;

    /*! Body input stream */
    std::shared_ptr<std::istream> _input;
    // Attributes -

  public:
    // Constructors +
    /*!
     * \ref Response default constructor
     */
    Response();

    /*!
     * \ref Response constructor
     *
     * \param code the HTTP return code
     */
    Response(uint16_t code);

    /*!
     * \ref Response copy constructor
     *
     * \param res the \ref Response to move
     */
    Response(const Response& res) = delete;

    /*!
     * \ref Response move constructor
     *
     * \param res the \ref Response to move
     */
    Response(Response&& res);
    // Constructors -

  public:
    // Destructor +
    /*!
     * \ref Response destructor
     */
    virtual ~Response();
    // Destructor -

    // Methods +
    /*!
     * Instanciate a new response builder for fluent API usage
     *
     * \return the new builder
     */
    static Builder builder();
    // Methods -

    // Operators +
  public:
    /*!
     * Move assignement
     *
     * \param other the \ref Response to move
     *
     * \return the new response
     */
    Response& operator=(Response&& other);
    // Operators -

    // Accessors +
    /*!
     * HTTP status code getter
     *
     * \return the code
     */
    uint16_t getStatus() const;

    /*!
     * HTTP status code setter
     *
     * \param code the status code to set
     */
    void setStatus(uint16_t code);

    /*!
     * HTTP headers getter
     *
     * \return the headers
     */
    const anch::rest::Headers& getHeaders() const;

    /*!
     * HTTP headers setter.\n
     * This method will override every headers previously set.
     *
     * \return the headers
     */
    void setHeaders(const anch::rest::Headers& headers);

    /*!
     * Media type getter
     *
     * \return the media type
     */
    std::string getContentType() const;

    /*!
     * Media type getter
     *
     * \param contentType the media type to set
     */
    void setContentType(const std::string& contentType);

    /*!
     * Response body setter
     *
     * \tparam T the object type
     *
     * \param body the body to set
     */
    template<typename T>
    void setBody(T body);

    /*!
     * Write body on output stream
     *
     * \param out the output stream to write in
     */
    void writeBody(std::ostream& out) const;
    // Accessors -

  };

  /*!
   * \brief HTTP response builder
   *
   * HTTP \ref Response builder
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class Response::Builder {
  private:
    /*! HTTP response */
    Response _response;

  public:
    // Constructors +
    /*!
     * \ref Builder default constructor
     */
    Builder();

    /*!
     * \ref Builder move constructor
     *
     * \param builder the builder to move
     */
    Builder(Builder&& builder);
    // Constructors -

  public:
    // Destructor +
    /*!
     * \ref Builder destructor
     */
    virtual ~Builder();
    // Destructor -

    // Methods +
    /*!
     * Status setter
     *
     * \param code the status code to set
     *
     * \return \c this
     */
    Builder status(uint16_t code);

    /*!
     * Add header when it does not exist
     *
     * \param name the header's name
     * \param value the header's value
     *
     * \return \c this
     */
    Builder header(const std::string& name, const std::string& value);

    /*!
     * Add header when it does not exist
     *
     * \param name the header's name
     * \param values the header's values
     *
     * \return \c this
     */
    Builder header(const std::string& name, const std::vector<std::string>& values);

    /*!
     * Set headers.\n
     * This method overrides all headers previsouly defined
     *
     * \param headers the headers to set
     *
     * \return \c this
     */
    Builder headers(const anch::rest::Headers& headers);

    /*!
     * Media type setter
     *
     * \param contentType the media type to set
     *
     * \return \c this
     */
    Builder contentType(const std::string& contentType);

    /*!
     * Body setter
     *
     * \param body the body to set
     */
    Builder body(const std::string& body);

    /*!
     * Body setter
     *
     * \param body the body to set
     */
    Builder body(const char* const body);

    /*!
     * Body setter
     *
     * \param body the body to set
     *
     * \return \c this
     */
    template<typename T>
    Builder body(T body);

    /*!
     * Finalize builder and return the HTTP \ref Response
     *
     * \return the HTTP response
     */
    Response build();
    // Methods -
  };

}

#include "rest/impl/response.hpp"
