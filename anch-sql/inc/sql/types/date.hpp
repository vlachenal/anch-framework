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
     * \brief SQL date type
     *
     * SQL date type.\n
     * It will only take into account from years to days.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    class Date: public anch::date::Date {

      // Constructors +
    public:
      /*!
       * \ref Date default constructor.
       *
       * \param init Initialize the \ref Date to now (default true)
       */
      Date(bool init = true);

      /*!
       * \ref Date private copy constructor
       *
       * \param date The \ref Date to copy
       */
      Date(const anch::date::Date& date);

      /*!
       * \ref Date constructor.
       *
       * \param time The time to set
       */
      Date(const std::time_t& time);

      /*!
       * \ref Date constructor.
       *
       * \param time The time to set
       */
      Date(const std::tm* const time);
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref Date destructor
       */
      virtual ~Date();
      // Destructor -

    };

  }
}
