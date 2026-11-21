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
#include <istream>
#include <optional>
#include <ostream>
#include <vector>

#include "codec/codec.hpp"
#include "codec/mediaType.hpp"

namespace anch::codec {

  /*!
   * List of codecs, resolved at compilation: selects the codec of a content type (reading) or of an \c Accept
   * negotiation (writing).\n
   * \c anch::codec::Available (\c codec/all.hpp , target \c anch::codec-all ) lists the codecs of the formats built
   * with the framework.
   *
   * \tparam Cs the codecs, by preference order
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  template<typename... Cs>
  struct Codecs {

    /*! \c true when one of the codecs serializes \c T values */
    template<typename T>
    static constexpr bool SERIALIZES = (anch::codec::Serializer<Cs, T> || ...);

    /*! \c true when one of the codecs deserializes \c T values */
    template<typename T>
    static constexpr bool DESERIALIZES = (anch::codec::Deserializer<Cs, T> || ...);

    /*!
     * Get codecs media types
     *
     * \return the media types, in codecs order
     *
     * \throw std::invalid_argument when a codec media type is invalid
     */
    static std::vector<anch::codec::MediaType> mediaTypes();

    /*!
     * Deserialize value with the first codec which deserializes \c T values and whose media type has the
     * \c type/subtype of the content type (parameters are given to the codec)
     *
     * \tparam T the value type
     *
     * \param value the value to fill
     * \param input the input stream
     * \param contentType the content type
     * \param side the side which deserializes value
     *
     * \return \c false when no codec handles the content type (value and input are untouched)
     */
    template<typename T>
    static bool deserialize(T& value, std::istream& input, const anch::codec::MediaType& contentType,
			    anch::codec::Side side);

    /*!
     * Serialize value with the codec selected by \c Accept negotiation (\ref anch::codec::negotiate "negotiate")
     * among the codecs which serialize \c T values
     *
     * \tparam T the value type
     *
     * \param value the value
     * \param out the output stream
     * \param accept the accepted media ranges ; empty means that every media type is accepted (first codec)
     * \param side the side which serializes value
     *
     * \return the content type of the serialized value, empty when no codec is acceptable (nothing is written)
     */
    template<typename T>
    static std::optional<anch::codec::MediaType> serialize(const T& value, std::ostream& out,
							   const std::vector<anch::codec::MediaType>& accept,
							   anch::codec::Side side);

  };

}

#include "codec/impl/codecs.hpp"
