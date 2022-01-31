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

#include <vector>
#include <string>
#include <memory>
#include <istream>
#include <map>

#include "rest/headers.hpp"
#include "rest/mapperRegistry.hpp"


namespace anch::rest {

  /*!
   * \brief HTTP request
   *
   * HTTP request representation
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class Request {
    class Builder;

    // Attributes +
  private:
    /*! HTTP verb */
    std::string _verb;

    /*! HTTP headers */
    anch::rest::Headers _headers;

    /*! Request URL */
    std::string _path;

    /*! Request path parameters */
    std::vector<std::string> _parameters;

    /*! Optional body input stream */
    std::shared_ptr<std::istream> _input;

    /*! Mapper registry */
    anch::rest::MapperRegistry* _registry;
    // Attributes -

  public:
    // Constructors +
    /*!
     * \ref Request default constructor
     */
    Request();

    /*!
     * \ref Request copy constructor
     *
     * \param res the \ref Request to move
     */
    Request(const Request& res) = delete;

    /*!
     * \ref Request move construtor
     */
    Request(Request&& other);
    // Constructors -

  public:
    // Destructor +
    /*!
     * \ref Request destructor
     */
    virtual ~Request();
    // Destructor -

    // Methods +
    /*!
     * Instanciate a new request builder for fluent API usage
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
     * \param other the \ref Request to move
     *
     * \return the new request
     */
    Request& operator=(Request&& other);
    // Operators -

  public:
    // Methods +
    /*!
     * Get body from input stream
     *
     * \return the body
     */
    template<typename T>
    T getBody();
    // Methods -

  public:
    // Accessors +
    /*!
     * HTTP verb getter
     *
     * \return the verb
     */
    const std::string& getVerb() const;

    /*!
     * HTTP verb getter
     *
     * \param verb the HTTP verb to set
     */
    void setVerb(const std::string& verb);

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
     * Media type setter
     *
     * \param content the content type to set
     */
    void setContentType(const std::string& contentType);

    /*!
     * Accepted media types getter
     *
     * \return the media types
     */
    const std::optional<const std::vector<std::string>> getAccept() const;

    /*!
     * Accepted media types setter
     *
     * \param accept the media types to set
     */
    void setAccept(const std::vector<std::string>& accept);

    /*!
     * Request path getter
     *
     * \return the path
     */
    const std::string& getPath() const;

    /*!
     * Request path setter
     *
     * \param path the path to set
     */
    void setPath(const std::string& path);

    /*!
     * Path parameters getter
     *
     * \return the parameters
     */
    const std::vector<std::string>& getParameters() const;

    /*!
     * Path parameters setter
     *
     * \param parameters the parameters to set
     */
    void setParameters(const std::vector<std::string>& parameters);
    // Accessors -

  };


  /*!
   * \brief HTTP request builder
   *
   * HTTP \ref Request builder
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class Request::Builder {

    // Attributes +
  private:
    /*! Current request */
    Request _request;
    // Attributes -

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


  public:
    // Methods +
    Builder path(const std::string& path);
    Builder verb(const std::string& verb);

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
     * Acceptable media type setter
     *
     * \param mediaType the media type to set
     *
     * \return \c this
     */
    Builder accept(const std::string& mediaType);

    /*!
     * Acceptable media types setter
     *
     * \param mediaTypes the media types to set
     *
     * \return \c this
     */
    Builder accept(const std::vector<std::string>& mediaTypes);

    /*!
     * Finalize builder and return the HTTP \ref Request
     *
     * \return the HTTP request
     */
    Request build();
    // Methods -

  };

}  // anch::rest

#include "rest/impl/request.hpp"
