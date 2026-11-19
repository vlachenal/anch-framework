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

#include "date/date.hpp"


namespace anch::sql {

  /*!
   * \brief SQL timestamp type
   *
   * SQL timestamp type.\n
   * It will take into account from year to microseconds.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class ANCH_SQL_EXPORT Timestamp: public anch::date::Date {

    // Constructors +
  public:
    /*!
     * \ref Timestamp default constructor.
     *
     * \param init Initialize the \ref Timestamp to now (default true)
     */
    explicit Timestamp(bool init = true);

    /*!
     * \ref Timestamp private copy constructor
     *
     * \param date The \ref Date to copy
     */
    Timestamp(const anch::date::Date& date);

    /*!
     * \ref Timestamp constructor.
     *
     * \param time The time to set
     */
    Timestamp(const std::time_t& time);

    /*!
     * \ref Timestamp constructor.
     *
     * \param time The time to set
     */
    Timestamp(const std::tm* const time);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Timestamp destructor
     */
    virtual ~Timestamp();
    // Destructor -

  };

}  // anch::sql
