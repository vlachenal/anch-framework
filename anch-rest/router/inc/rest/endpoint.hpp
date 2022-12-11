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
#include <regex>

#include "rest/constants.hpp"
#include "rest/request.hpp"


namespace anch::rest {

  /*!
   * \brief Router endpoint
   *
   * HTTP router endpoint representation
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class EndPoint {
  public:
    /*! HTTP verb */
    std::string verb;

    /*! Path pattern */
    std::string pathPattern;

    /*! Reponse media type */
    std::string contentType;

    /*! Request media type */
    std::string accept;

  private:
    // Attributes +
    /*! Path pattern to remove wild card */
    static std::regex PATH_PATTERN; // \todo replace with common constants

    /*! Path regular expression */
    std::regex _pathRegex;

    /*! Built flag */
    bool _built;
    // Attributes -

  public:
    // Constructors +
    /*!
     * \ref EndPoint default constructor
     */
    EndPoint();

    /*!
     * \ref EndPoint copy constructor
     *
     * \param other the \ref EndPoint to copy
     */
    EndPoint(const EndPoint& other);

    /*!
     * \ref EndPoint move constructor
     *
     * \param other the \ref EndPoint to move
     */
    EndPoint(EndPoint&& other);
    // Constructors -

  public:
    // Destructor +
    /*!
     * \ref EndPoint destructor
     */
    virtual ~EndPoint();
    // Destructor -

  public:
    // Methods +
    /*!
     * Check if \ref Request match this endpoint
     *
     * \param request the request to check
     *
     * \return \c true if request matches, \c false otherwise
     */
    bool match(const anch::rest::Request& request) const;

    /*!
     * Finalize \ref EndPoint construction.\n
     * This will compute number of separator in path and path regular expression.
     */
    void build();
    // Methods -

    // Accessors +
    /*!
     * Path pattern getter
     *
     * \return the path pattern
     */
    const std::string& getPathPattern() const;

    /*!
     * Path pattern setter
     *
     * \param pathPattern the path pattern to set
     */
    void setPathPattern(const std::string& pathPattern);

    /*!
     * Number of '/' getter in path
     *
     * \return the number of '/'
     */
    const std::regex& getPathRegex() const;
    // Accessors -
  };

}  // anch::rest

#include "rest/impl/endpoint.hpp"
