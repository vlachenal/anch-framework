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

#include <istream>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>

#include "json/json.hpp"
#include "json/mappingOptions.hpp"
#include "mapping.hpp"

namespace anch::json {

  /*!
   * \brief Type which the JSON format maps
   *
   * Its value type (the type itself, or the value type of \c std::optional , smart pointers, collections and maps,
   * as for fields) is a primitive ( \c anch::json::isPrimitive ), an enumeration or a type
   * described by an \c anch::Mapping<T> specialization ( \c anch::Described ). Types described by an external
   * schema (Protobuf messages, Thrift structures) are not mapped: lists of codecs (anch-codec) select the codecs of
   * their format.
   *
   * \tparam T the type
   *
   * \since 0.3
   */
  template<typename T>
  concept Mappable = anch::json::isPrimitive<anch::json::detail::MapperType<T>>()
    || std::is_enum_v<anch::json::detail::MapperType<T>>
    || anch::Described<anch::json::detail::MapperType<T>>;

  /*!
   * \brief JSON format: serialization and deserialization of values with options
   *
   * It is the format type used by generic codecs ( \c anch::codec::FormatCodec , anch-codec): it does not depend
   * on them (its members follow the \c anch::codec::DataFormat concept).
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  struct JsonFormat {

    /*! Format options */
    using Options = anch::json::MappingOptions;

    /*! Media type */
    static constexpr std::string_view MEDIA_TYPE = "application/json";

    /*!
     * Default options
     *
     * \return \c anch::json::DEFAULT_MAPPING_OPTIONS
     */
    static const Options& defaultOptions() noexcept;

    /*!
     * Serialize value
     *
     * \param value the value (described object, collection, map, primitive, ...)
     * \param out the output stream
     * \param options the options
     *
     * \throw anch::json::MappingError on constraint violation (when constraints are checked on serialization)
     */
    template<anch::json::Mappable T>
    static void serialize(const T& value, std::ostream& out, const Options& options);

    /*!
     * Deserialize value
     *
     * \param value the value to fill
     * \param input the input stream
     * \param options the options
     *
     * \throw anch::json::MappingError on invalid JSON or constraint violation
     */
    template<anch::json::Mappable T>
    static void deserialize(T& value, std::istream& input, const Options& options);

  };

}

namespace anch::json {

  inline const JsonFormat::Options&
  JsonFormat::defaultOptions() noexcept {
    return anch::json::DEFAULT_MAPPING_OPTIONS;
  }

  template<anch::json::Mappable T>
  void
  JsonFormat::serialize(const T& value, std::ostream& out, const Options& options) {
    if constexpr (std::is_same_v<T, std::string>) {
      // std::string would match anch-json container overload (C<T> with T = char)
      anch::json::serialize<std::string>(value, out, options);
    } else {
      anch::json::serialize(value, out, options);
    }
  }

  template<anch::json::Mappable T>
  void
  JsonFormat::deserialize(T& value, std::istream& input, const Options& options) {
    if constexpr (std::is_same_v<T, std::string>) {
      anch::json::deserialize<std::string>(value, input, options);
    } else {
      anch::json::deserialize(value, input, options);
    }
  }

}
