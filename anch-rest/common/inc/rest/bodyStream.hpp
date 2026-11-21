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

#include <cstdint>
#include <istream>
#include <memory>
#include <optional>

namespace anch::rest {

  /*!
   * Body read as a stream: codecs which define
   * <tt>stream(const T&, const anch::codec::MediaType&, anch::codec::Side) -> anch::rest::BodyStream</tt> let the
   * client send large bodies without building them in memory (see \ref Multipart ).
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_REST_COMMON_EXPORT BodyStream {
    /*! Body stream */
    std::unique_ptr<std::istream> input;

    /*! Body size ( \c Content-Length ), empty when unknown */
    std::optional<std::uint64_t> size;
  };

}
