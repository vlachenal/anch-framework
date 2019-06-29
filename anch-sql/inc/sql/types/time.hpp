/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "date/date.hpp"


namespace anch {
  namespace sql {

    /*!
     * \brief SQL time type
     *
     * SQL time type.\n
     * It will only take into from hours to microseconds
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    class Time: public anch::date::Date {

      // Constructors +
    public:
      /*!
       * \ref Time default constructor.
       *
       * \param init Initialize the \ref Time to now (default true)
       */
      Time(bool init = true);

      /*!
       * \ref Time private copy constructor
       *
       * \param date The \ref Date to copy
       */
      Time(const anch::date::Date& date);

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

      // Destructor +
    public:
      /*!
       * \ref Time destructor
       */
      virtual ~Time();
      // Destructor -

    };

  }
}
