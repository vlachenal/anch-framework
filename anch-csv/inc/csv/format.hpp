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
#include <ranges>
#include <string_view>
#include <vector>

#include "mapping.hpp"

#include "csv/csv.hpp"
#include "csv/options.hpp"

namespace anch::csv {

  /*!
   * \brief CSV format: serialization and deserialization of lists of described objects with options
   *
   * It is the format type used by generic codecs ( \c anch::codec::FormatCodec , anch-codec): it does not depend
   * on them (its members follow the \c anch::codec::DataFormat concept). Values are lists of described objects
   * ( \c anch::Mapping ): one record per object.
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  struct Csv {

    /*! Format options */
    using Options = anch::csv::Options;

    /*! Media type */
    static constexpr std::string_view MEDIA_TYPE = "text/csv";

    /*!
     * Default options (RFC 4180)
     *
     * \return the default options
     */
    static const Options& defaultOptions() noexcept;

    /*!
     * Serialize objects (header, then one record per object)
     *
     * \param values the objects
     * \param out the output stream
     * \param options the options
     *
     * \throw CsvError on constraint violation (with \c Options::writeCheckConstraints )
     */
    template<std::ranges::input_range R>
    requires anch::Described<std::remove_cvref_t<std::ranges::range_value_t<R>>>
    static void serialize(const R& values, std::ostream& out, const Options& options);

    /*!
     * Deserialize objects
     *
     * \param values the objects (replaced)
     * \param input the input stream
     * \param options the options
     *
     * \throw CsvError on invalid CSV, conversion error or constraint violation
     */
    template<anch::Described T>
    static void deserialize(std::vector<T>& values, std::istream& input, const Options& options);

  };

}

namespace anch::csv {

  inline const Csv::Options&
  Csv::defaultOptions() noexcept {
    static const Options options;
    return options;
  }

  template<std::ranges::input_range R>
  requires anch::Described<std::remove_cvref_t<std::ranges::range_value_t<R>>>
  void
  Csv::serialize(const R& values, std::ostream& out, const Options& options) {
    anch::csv::serialize(values, out, options);
  }

  template<anch::Described T>
  void
  Csv::deserialize(std::vector<T>& values, std::istream& input, const Options& options) {
    anch::csv::deserialize(values, input, options);
  }

}
