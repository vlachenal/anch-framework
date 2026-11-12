/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2012 Vincent Lachenal

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

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace anch::rest {

  /*!
   * Case insensitive (ASCII) string comparison for header names
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct CaseInsensitiveLess {
    /*! Allow lookup with \c std::string_view */
    using is_transparent = void;

    /*!
     * Compare strings ignoring case
     *
     * \param left the left string
     * \param right the right string
     *
     * \return \c true when \c left is lower than \c right
     */
    bool operator()(std::string_view left, std::string_view right) const noexcept;
  };

  /*!
   * HTTP headers.\n
   * Header names are case insensitive. A header can have several values.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class Headers {

  public:
    /*! Header values by name */
    using Map = std::map<std::string, std::vector<std::string>, anch::rest::CaseInsensitiveLess>;

    // Attributes +
  private:
    /*! Header values by name */
    Map _values;
    // Attributes -

    // Methods +
  public:
    /*!
     * Add a value to header
     *
     * \param name the header name
     * \param value the value to add
     */
    void add(std::string_view name, std::string_view value);

    /*!
     * Set header value (replace existing values)
     *
     * \param name the header name
     * \param value the value
     */
    void set(std::string_view name, std::string_view value);

    /*!
     * Remove header
     *
     * \param name the header name
     *
     * \return \c true when header has been removed
     */
    bool remove(std::string_view name);

    /*!
     * Check if header is set
     *
     * \param name the header name
     *
     * \return \c true when header is set
     */
    bool has(std::string_view name) const;

    /*!
     * Get header first value
     *
     * \param name the header name
     *
     * \return the first value, empty when header is not set
     */
    std::optional<std::string_view> get(std::string_view name) const;

    /*!
     * Get header values
     *
     * \param name the header name
     *
     * \return the values (empty when header is not set)
     */
    const std::vector<std::string>& getAll(std::string_view name) const;

    /*!
     * Get header values joined with \c ", "
     *
     * \param name the header name
     *
     * \return the values, empty when header is not set
     */
    std::optional<std::string> format(std::string_view name) const;

    /*!
     * Get the number of headers
     *
     * \return the number of headers
     */
    std::size_t size() const noexcept;

    /*!
     * Check if there is no header
     *
     * \return \c true when there is no header
     */
    bool empty() const noexcept;

    /*!
     * Iterate over headers (name, values)
     *
     * \return the first header iterator
     */
    Map::const_iterator begin() const noexcept;

    /*!
     * Iterate over headers (name, values)
     *
     * \return the end iterator
     */
    Map::const_iterator end() const noexcept;
    // Methods -

  };

}

namespace anch::rest::header {

  /*! \c Accept header */
  inline constexpr std::string_view ACCEPT = "Accept";
  /*! \c Allow header */
  inline constexpr std::string_view ALLOW = "Allow";
  /*! \c Authorization header */
  inline constexpr std::string_view AUTHORIZATION = "Authorization";
  /*! \c Cache-Control header */
  inline constexpr std::string_view CACHE_CONTROL = "Cache-Control";
  /*! \c Content-Length header */
  inline constexpr std::string_view CONTENT_LENGTH = "Content-Length";
  /*! \c Content-Type header */
  inline constexpr std::string_view CONTENT_TYPE = "Content-Type";
  /*! \c ETag header */
  inline constexpr std::string_view ETAG = "ETag";
  /*! \c Location header */
  inline constexpr std::string_view LOCATION = "Location";
  /*! \c User-Agent header */
  inline constexpr std::string_view USER_AGENT = "User-Agent";
  /*! \c WWW-Authenticate header */
  inline constexpr std::string_view WWW_AUTHENTICATE = "WWW-Authenticate";
  /*! \c X-Correlation-ID header */
  inline constexpr std::string_view X_CORRELATION_ID = "X-Correlation-ID";
  /*! \c X-Request-ID header */
  inline constexpr std::string_view X_REQUEST_ID = "X-Request-ID";

}
