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

#include "inc/rest/clientIface.hpp"

#include <curl/curl.h>

namespace anch::rest {

  /*!
   * \brief cURL HTTP client implementation
   *
   * cURL HTTP client implementation
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class CURLClient: public anch::rest::ClientIface {

    // Attributes +
  private:
    /*! cURL instance */
    CURL* _curl;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref CURLClient default constructor
     */
    CURLClient();
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref CURLClient destructor
     */
    virtual ~CURLClient();
    // Destructor -

    // Methods +
  public:
    /*!
     * Map path parameters and execute GET HTTP method
     *
     * \param request the request
     *
     * \return return the response
     */
    virtual anch::rest::Response GET(const anch::rest::Request& request) override;

    /*!
     * Execute GET HTTP method
     *
     * \param path the path
     *
     * \return return the response
     */
    virtual anch::rest::Response GET(const std::string& path) override;

    /*!
     * Map path parameters and execute POST HTTP method
     *
     * \param request the request
     *
     * \return return the response
     */
    virtual anch::rest::Response POST(const anch::rest::Request& request) override;

    /*!
     * Map path parameters and execute PUT HTTP method
     *
     * \param request the request
     *
     * \return return the response
     */
    virtual anch::rest::Response PUT(const anch::rest::Request& request) override;

    /*!
     * Map path parameters and execute PATCH HTTP method
     *
     * \param request the request
     *
     * \return return the response
     */
    virtual anch::rest::Response PATCH(const anch::rest::Request& request) override;

    /*!
     * Map path parameters and execute DELETE HTTP method
     *
     * \param request the request
     *
     * \return return the response
     */
    virtual anch::rest::Response DELETE(const anch::rest::Request& request) override;

    /*!
     * Map path parameters and execute DELETE HTTP method
     *
     * \param path the path
     *
     * \return return the response
     */
    virtual anch::rest::Response DELETE(const std::string& path) override;

    /*!
     * Map path parameters and execute HEAD HTTP method
     *
     * \param params the path parameters
     *
     * \return return the response
     */
    virtual anch::rest::Response HEAD(const anch::rest::Request& request) override;

    /*!
     * Map path parameters and execute HEAD HTTP method
     *
     * \param path the path
     *
     * \return return the response
     */
    virtual anch::rest::Response HEAD(const std::string& path) override;

    /*!
     * Map path parameters and execute OPTIONS HTTP method
     *
     * \param request the request
     *
     * \return return the response
     */
    virtual anch::rest::Response OPTIONS(const anch::rest::Request& request) override;

    /*!
     * Map path parameters and execute OPTIONS HTTP method
     *
     * \param path the path
     *
     * \return return the response
     */
    virtual anch::rest::Response OPTIONS(const std::string& path) override;

    /*!
     * Map path parameters and execute CONNECT HTTP method
     *
     * \param request the request
     *
     * \return return the response
     */
    virtual anch::rest::Response CONNECT(const anch::rest::Request& request) override;

    /*!
     * Map path parameters and execute CONNECT HTTP method
     *
     * \param path the path
     *
     * \return return the response
     */
    virtual anch::rest::Response CONNECT(const std::string& path) override;


    /*!
     * Map path parameters and execute TRACE HTTP method
     *
     * \param path the path
     *
     * \return return the response
     */
    virtual anch::rest::Response TRACE(const std::string& path) override;
    // Methods -

  };

}  // anch::rest
