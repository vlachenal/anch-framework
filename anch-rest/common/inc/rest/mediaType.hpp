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
#include <optional>
#include <string_view>
#include <vector>

#include "codec/mediaType.hpp"

/*!
 * \file rest/mediaType.hpp
 * \deprecated removed in 0.4: media types have moved to anch-codec, use \c codec/mediaType.hpp
 */

namespace anch::rest {

  /*! \deprecated removed in 0.4, use \ref anch::codec::MediaType */
  using MediaType [[deprecated("removed in 0.4, use anch::codec::MediaType")]] = anch::codec::MediaType;

  /*!
   * \deprecated removed in 0.4, use \ref anch::codec::negotiate
   *
   * \param accept the \c Accept media ranges
   * \param available the available media types
   *
   * \return the selected media type index, empty when no media type is acceptable
   */
  [[deprecated("removed in 0.4, use anch::codec::negotiate")]]
  inline std::optional<std::size_t>
  negotiate(const std::vector<anch::codec::MediaType>& accept, const std::vector<anch::codec::MediaType>& available) {
    return anch::codec::negotiate(accept, available);
  }

}

namespace anch::rest::media {

  /*! \deprecated removed in 0.4, use \c anch::codec::media::APP_JSON */
  [[deprecated("removed in 0.4, use anch::codec::media::APP_JSON")]]
  inline constexpr std::string_view APP_JSON = anch::codec::media::APP_JSON;
  /*! \deprecated removed in 0.4, use \c anch::codec::media::APP_OCTET_STREAM */
  [[deprecated("removed in 0.4, use anch::codec::media::APP_OCTET_STREAM")]]
  inline constexpr std::string_view APP_OCTET_STREAM = anch::codec::media::APP_OCTET_STREAM;
  /*! \deprecated removed in 0.4, use \c anch::codec::media::APP_FORM_URLENCODED */
  [[deprecated("removed in 0.4, use anch::codec::media::APP_FORM_URLENCODED")]]
  inline constexpr std::string_view APP_FORM_URLENCODED = anch::codec::media::APP_FORM_URLENCODED;
  /*! \deprecated removed in 0.4, use \c anch::codec::media::APP_XML */
  [[deprecated("removed in 0.4, use anch::codec::media::APP_XML")]]
  inline constexpr std::string_view APP_XML = anch::codec::media::APP_XML;
  /*! \deprecated removed in 0.4, use \c anch::codec::media::MULTIPART_FORM_DATA */
  [[deprecated("removed in 0.4, use anch::codec::media::MULTIPART_FORM_DATA")]]
  inline constexpr std::string_view MULTIPART_FORM_DATA = anch::codec::media::MULTIPART_FORM_DATA;
  /*! \deprecated removed in 0.4, use \c anch::codec::media::TEXT_CSV */
  [[deprecated("removed in 0.4, use anch::codec::media::TEXT_CSV")]]
  inline constexpr std::string_view TEXT_CSV = anch::codec::media::TEXT_CSV;
  /*! \deprecated removed in 0.4, use \c anch::codec::media::TEXT_HTML */
  [[deprecated("removed in 0.4, use anch::codec::media::TEXT_HTML")]]
  inline constexpr std::string_view TEXT_HTML = anch::codec::media::TEXT_HTML;
  /*! \deprecated removed in 0.4, use \c anch::codec::media::TEXT_PLAIN */
  [[deprecated("removed in 0.4, use anch::codec::media::TEXT_PLAIN")]]
  inline constexpr std::string_view TEXT_PLAIN = anch::codec::media::TEXT_PLAIN;

}
