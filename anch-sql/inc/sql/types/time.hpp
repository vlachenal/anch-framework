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

#include "anch-sql-export.hpp"

#include "date/dateTime.hpp"


namespace anch::sql {

  /*!
   * \brief SQL time type
   *
   * SQL time type: time of day (local time) of a point in time, on 1970-01-01 (it is replaced by a time of day type
   * in a next version: see \c anch-date/date.todo ).
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class ANCH_SQL_EXPORT Time: public anch::date::DateTime {

    // Constructors +
  public:
    /*!
     * \ref Time default constructor.
     *
     * \param init Initialize the \ref Time to the current time of day (default true), to the time of day of epoch otherwise
     */
    explicit Time(bool init = true);

    /*!
     * \ref Time private copy constructor
     *
     * \param date The point in time ( \ref anch::date::DateTime ) whose time is kept
     */
    Time(const anch::date::DateTime& date);

    /*!
     * \ref Time constructor.
     *
     * \param time The time to set
     */
    Time(const std::time_t& time);

    /*!
     * \ref Time constructor.
     *
     * \param time The time to set
     */
    Time(const std::tm* const time);
    // Constructors -

  };

}  // anch::sql
