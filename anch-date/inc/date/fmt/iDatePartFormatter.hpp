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

#include "anch-date-export.hpp"

#include <cstdint>
#include <ostream>
#include <string>

#include "date/dateTime.hpp"


namespace anch::date {

  /*!
   * \brief Date and time part formatter interface
   *
   * Implement this interface to add a pattern to \ref DateFormatter (\ref DateFormatter::registerFormatterPart ):
   * parts format and parse the broken down local time ( \ref DateFields , computed once by \ref DateFormatter ).\n
   * A part has a fixed size: \ref setValue receives exactly \ref getSize characters. When a \ref DateTime is parsed, its
   * fields are 1970-01-01 00:00:00.000 before parts are called, and the point in time is built afterwards.
   *
   * \author Vincent Lachenal
   */
  class ANCH_DATE_EXPORT IDatePartFormatter {
  public:
    /*!
     * \ref IDatePartFormatter destructor
     */
    virtual ~IDatePartFormatter() {
      // Nothing to do
    }

    /*!
     * Format date
     *
     * \param fields the local time of the \ref DateTime to format
     * \param output The output stream to write in
     */
    virtual void format(const anch::date::DateFields& fields, std::ostream& output) const noexcept = 0;

    /*!
     * Get size of formatter part
     *
     * \return The size of formatter part
     */
    virtual std::size_t getSize() const noexcept = 0;

    /*!
     * Set parsed value
     *
     * \param fields the local time to modify
     * \param value The value to set
     *
     * \return \c true when value is valid, \c false otherwise
     */
    virtual bool setValue(anch::date::DateFields& fields, const std::string& value) const noexcept = 0;

    /*!
     * Get formatter part pattern
     *
     * \return The formatter part pattern
     */
    virtual const std::string& getPattern() const noexcept = 0;

  };

}

