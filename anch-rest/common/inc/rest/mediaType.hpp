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

#include "anch-rest-common-export.hpp"

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace anch::rest {

  /*!
   * Media type (RFC 9110): \c type/subtype with parameters.\n
   * Type, subtype and parameter names are stored in lower case. In \c Accept header, the \c q
   * parameter is the quality and is not kept in parameters.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class ANCH_REST_COMMON_EXPORT MediaType {

    // Attributes +
  private:
    /*! Type (\c application , \c * , ...) */
    std::string _type;

    /*! Subtype (\c json , \c * , ...) */
    std::string _subtype;

    /*! Parameters (\c charset , ...) */
    std::map<std::string, std::string> _parameters;

    /*! Quality (\c Accept header), between \c 0 and \c 1 */
    double _quality;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref MediaType constructor
     *
     * \param type the type
     * \param subtype the subtype
     */
    MediaType(std::string_view type, std::string_view subtype);
    // Constructors -

    // Methods +
  public:
    /*!
     * Parse media type (\c Content-Type header value)
     *
     * \param value the value to parse
     *
     * \return the media type, empty when value is invalid
     */
    static std::optional<anch::rest::MediaType> parse(std::string_view value);

    /*!
     * Parse \c Accept header value.\n
     * Media ranges are sorted by quality (highest first, stable). Invalid media ranges are ignored.
     * Media ranges with \c q=0 are kept: they exclude the media types they include (see \ref anch::rest::negotiate "negotiate").
     *
     * \param value the value to parse
     *
     * \return the media ranges
     */
    static std::vector<anch::rest::MediaType> parseAccept(std::string_view value);

    /*!
     * Check if this media range includes a media type (wildcards are supported, parameters are
     * ignored)
     *
     * \param other the media type
     *
     * \return \c true when \c other is included in this media range
     */
    bool includes(const anch::rest::MediaType& other) const noexcept;

    /*!
     * Get media type specificity: \c 0 for any type, \c 1 for any subtype of a type, \c 2 otherwise
     *
     * \return the specificity
     */
    int specificity() const noexcept;

    /*!
     * Get \c type/subtype without parameters
     *
     * \return the essence
     */
    std::string essence() const;

    /*!
     * Format media type with its parameters (without quality)
     *
     * \return the formatted media type
     */
    std::string toString() const;

    /*!
     * Compare media types (essence and parameters, quality is ignored)
     *
     * \param other the media type to compare
     *
     * \return \c true when media types are equal
     */
    bool operator==(const anch::rest::MediaType& other) const noexcept;
    // Methods -

    // Accessors +
  public:
    /*!
     * Type getter
     *
     * \return the type
     */
    const std::string& getType() const noexcept;

    /*!
     * Subtype getter
     *
     * \return the subtype
     */
    const std::string& getSubtype() const noexcept;

    /*!
     * Parameters getter
     *
     * \return the parameters
     */
    const std::map<std::string, std::string>& getParameters() const noexcept;

    /*!
     * Get parameter value
     *
     * \param name the parameter name (lower case)
     *
     * \return the value, empty when parameter is not set
     */
    std::optional<std::string_view> getParameter(std::string_view name) const;

    /*!
     * Set parameter value
     *
     * \param name the parameter name
     * \param value the parameter value
     */
    void setParameter(std::string_view name, std::string_view value);

    /*!
     * Quality getter
     *
     * \return the quality
     */
    double getQuality() const noexcept;
    // Accessors -

  };

  /*!
   * Select the best available media type for \c Accept media ranges (RFC 9110 proactive
   * negotiation): the quality of an available media type is the quality of the most specific
   * media range which includes it. On equal quality, the first available media type is selected.
   *
   * \param accept the \c Accept media ranges ; empty means that every media type is accepted
   * \param available the available media types
   *
   * \return the selected media type index, empty when no media type is acceptable
   */
  ANCH_REST_COMMON_EXPORT std::optional<std::size_t> negotiate(const std::vector<anch::rest::MediaType>& accept,
				       const std::vector<anch::rest::MediaType>& available);

}

namespace anch::rest::media {

  /*! \c application/json */
  inline constexpr std::string_view APP_JSON = "application/json";
  /*! \c application/octet-stream */
  inline constexpr std::string_view APP_OCTET_STREAM = "application/octet-stream";
  /*! \c application/x-www-form-urlencoded */
  inline constexpr std::string_view APP_FORM_URLENCODED = "application/x-www-form-urlencoded";
  /*! \c application/xml */
  inline constexpr std::string_view APP_XML = "application/xml";
  /*! \c multipart/form-data */
  inline constexpr std::string_view MULTIPART_FORM_DATA = "multipart/form-data";
  /*! \c text/csv */
  inline constexpr std::string_view TEXT_CSV = "text/csv";
  /*! \c text/html */
  inline constexpr std::string_view TEXT_HTML = "text/html";
  /*! \c text/plain */
  inline constexpr std::string_view TEXT_PLAIN = "text/plain";

}
