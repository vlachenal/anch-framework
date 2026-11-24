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

#include "anch-codec-csv-export.hpp"

#include <exception>

#include "csv/format.hpp"

#include "codec/codec.hpp"
#include "codec/formatCodec.hpp"

namespace anch::codec {

  /*!
   * \brief Policy of the CSV codec (anch-codec-csv plugin, option \c ANCH_CODEC_CSV )
   *
   * Options of each side are RFC 4180 default ones, then:
   * - \c SERVER : constraints are checked on read objects ( \c readCheckConstraints ), not on written ones, and read
   *   values are limited to 2 MiB ( \c maxSize , as JSON bodies): larger ones raise \ref ContentTooLarge ;
   * - \c CLIENT : constraints are checked on written objects ( \c writeCheckConstraints ), read values are trusted
   *   and read with tolerance (unknown columns are ignored).
   *
   * \since 0.3
   */
  template<>
  struct ANCH_CODEC_CSV_EXPORT FormatPolicy<anch::csv::CsvFormat> {
    /*!
     * Configure the options of a side
     *
     * \param options the options
     * \param side the side
     */
    static void configure(anch::csv::Options& options, anch::codec::Side side);

    /*!
     * Translate \c TOO_LARGE CSV errors into \ref ContentTooLarge
     *
     * \param error the error
     */
    static void translate(const std::exception& error);
  };

  /*!
   * \c text/csv codec based on anch-csv (anch-codec-csv plugin, option \c ANCH_CODEC_CSV ): values are lists of
   * described objects ( \c anch::Mapping , anch-utils), one record per object. Options of each side: see
   * \ref FormatPolicy<anch::csv::CsvFormat> "FormatPolicy".
   *
   * \since 0.3
   */
  using Csv = anch::codec::FormatCodec<anch::csv::CsvFormat>;

}

// Options of each side are instantiated in anch-codec-csv (unique in the program)
extern template struct ANCH_CODEC_CSV_EXPORT anch::codec::FormatCodec<anch::csv::CsvFormat>;
