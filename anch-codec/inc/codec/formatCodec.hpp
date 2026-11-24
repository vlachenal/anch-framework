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

#include "anch-codec-export.hpp"

#include <concepts>
#include <exception>
#include <istream>
#include <ostream>
#include <string_view>

#include "codec/codec.hpp"

namespace anch::codec {

  /*!
   * \brief Data format which serializes values with options (anch-json \c anch::json::JsonFormat , anch-csv
   * \c anch::csv::CsvFormat , ...)
   *
   * A format defines \c Options , \c MEDIA_TYPE and \c defaultOptions() ; it serializes values with
   * <tt>serialize(value, out, options)</tt> and deserializes them with <tt>deserialize(value, input, options)</tt>
   * ( \ref anch::codec::FormatSerializer , \ref anch::codec::FormatDeserializer ). Formats do not depend on anch-codec: \ref anch::codec::FormatCodec makes a
   * codec of them.
   *
   * \tparam F the format type
   *
   * \since 0.3
   */
  template<typename F>
  concept DataFormat = requires {
    typename F::Options;
    { F::MEDIA_TYPE } -> std::convertible_to<std::string_view>;
    { F::defaultOptions() } -> std::convertible_to<const typename F::Options&>;
  };

  /*!
   * \brief Format which serializes \c T values
   *
   * \tparam F the format type
   * \tparam T the value type
   *
   * \since 0.3
   */
  template<typename F, typename T>
  concept FormatSerializer = anch::codec::DataFormat<F>
    && requires(const T& value, std::ostream& out, const typename F::Options& options) { F::serialize(value, out, options); };

  /*!
   * \brief Format which deserializes \c T values
   *
   * \tparam F the format type
   * \tparam T the value type
   *
   * \since 0.3
   */
  template<typename F, typename T>
  concept FormatDeserializer = anch::codec::DataFormat<F>
    && requires(T& value, std::istream& input, const typename F::Options& options) { F::deserialize(value, input, options); };

  /*!
   * \brief Policy of a format codec: options of each side and errors translation
   *
   * Codec plugins specialize it for their format (a format does not know \ref Side ): anch-codec-json sets the
   * constraints checks and limits of each side and translates payload size errors into \ref ContentTooLarge .
   * The default policy uses the default options for both sides and does not translate errors.
   *
   * \tparam F the format type
   *
   * \since 0.3
   */
  template<anch::codec::DataFormat F>
  struct FormatPolicy {
    /*!
     * Configure the options of a side (they are copied from the format default options)
     *
     * \param options the options
     * \param side the side
     */
    static void
    configure([[maybe_unused]] typename F::Options& options, [[maybe_unused]] anch::codec::Side side) {
      // Default options for both sides
    }

    /*!
     * Translate a deserialization error: throw another exception, or return to rethrow the error
     *
     * \param error the error
     */
    static void
    translate([[maybe_unused]] const std::exception& error) {
      // Error is rethrown
    }
  };

  /*!
   * \brief Codec of a data format: media type, options of each side ( \ref FormatPolicy ), serialization and
   * deserialization ( \ref Serializer , \ref Deserializer concepts)
   *
   * Values are handled when the format handles them ( \ref FormatSerializer , \ref FormatDeserializer ): formats
   * constrain their types (described objects for anch-json and anch-csv), so that values described by an external
   * schema (Protobuf messages, Thrift structures) are left to the codecs of their format. Options of each side are shared by the program: a codec plugin instantiates its \c FormatCodec in its
   * library ( \c anch::codec::Json is \c FormatCodec<anch::json::JsonFormat> , \c anch::codec::Csv is
   * \c FormatCodec<anch::csv::CsvFormat> ).
   *
   * \tparam F the format type
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  template<anch::codec::DataFormat F>
  struct ANCH_CODEC_EXPORT FormatCodec {

    /*! Format options type */
    using Options = typename F::Options;

    /*!
     * Media type
     *
     * \return \c F::MEDIA_TYPE
     */
    static std::string_view mediaType() noexcept;

    /*!
     * Options of a side: copied from \c F::defaultOptions() on first use, then configured by \ref FormatPolicy .
     * They can be changed at application initialization (they are not protected against concurrent changes).
     *
     * \param side the side
     *
     * \return the options of the side
     */
    static Options& options(anch::codec::Side side);

    /*!
     * Serialize value with the format default options
     *
     * \param value the value
     * \param out the output stream
     */
    template<typename T>
    requires anch::codec::FormatSerializer<F, T>
    static void serialize(const T& value, std::ostream& out);

    /*!
     * Serialize value with the options of a side
     *
     * \param value the value
     * \param out the output stream
     * \param side the side
     */
    template<typename T>
    requires anch::codec::FormatSerializer<F, T>
    static void serialize(const T& value, std::ostream& out, anch::codec::Side side);

    /*!
     * Deserialize value with the format default options
     *
     * \param value the value to fill
     * \param input the input stream
     */
    template<typename T>
    requires anch::codec::FormatDeserializer<F, T>
    static void deserialize(T& value, std::istream& input);

    /*!
     * Deserialize value with the options of a side
     *
     * \param value the value to fill
     * \param input the input stream
     * \param side the side
     *
     * \throw ContentTooLarge input is larger than a limit (translated by \ref FormatPolicy )
     */
    template<typename T>
    requires anch::codec::FormatDeserializer<F, T>
    static void deserialize(T& value, std::istream& input, anch::codec::Side side);

  private:
    /*!
     * Deserialize value with options, errors are translated
     *
     * \param value the value to fill
     * \param input the input stream
     * \param options the options
     */
    template<typename T>
    static void read(T& value, std::istream& input, const Options& options);

  };

}

#include "codec/impl/formatCodec.hpp"
