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
#include <string>

namespace anch::csv {

  /*!
   * CSV format options. Default values follow RFC 4180 (\c , separator, \c " quote, \c CRLF line endings, header).
   * Objects mapping options are used by \ref anch::csv::ObjectReader and \ref anch::csv::ObjectWriter .
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  struct Options {
    /*! Fields separator (\c ; , \c \\t , ...) */
    char separator = ',';

    /*! Quote character */
    char quote = '"';

    /*! Written line ending: \c CRLF ( \c true ) or \c LF . Both are read. */
    bool crlf = true;

    /*! The first record is the header (column names) */
    bool header = true;

    /*! Write an UTF-8 BOM (spreadsheets) ; a BOM is always skipped when it is read */
    bool bom = false;

    /*! Maximum size of a read record in bytes, \c 0 for no limit */
    std::size_t maxRecordSize = 1024 * 1024;

    /*! Maximum size of the read input in bytes, \c 0 for no limit (server side bodies) */
    std::size_t maxSize = 0;

    /*! Maximum number of columns of a read record, \c 0 for no limit */
    std::size_t maxColumns = 1024;

    /*! Every read record has the number of columns of the first one (header or first record) */
    bool sameColumnCount = true;

    /*! Objects mapping: columns which are not fields are ignored ( \c false : \c UNKNOWN_COLUMN error) */
    bool ignoreUnknownColumns = true;

    /*! Objects mapping: fields constraints (anch-val) are checked when objects are read */
    bool readCheckConstraints = true;

    /*! Objects mapping: fields constraints (anch-val) are checked when objects are written */
    bool writeCheckConstraints = false;

    /*! Objects mapping: text of \c true booleans */
    std::string trueValue = "true";

    /*! Objects mapping: text of \c false booleans */
    std::string falseValue = "false";
  };

}
