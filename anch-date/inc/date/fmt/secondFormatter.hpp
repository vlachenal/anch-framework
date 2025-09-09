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

#include "date/fmt/iDatePartFormatter.hpp"

namespace anch::date {

  /*!
   * Seconds formatter part
   *
   * \author Vincent Lachenal
   */
  class SecondFormatter: public IDatePartFormatter {
  public:
    /*! The pattern */
    static const std::string PATTERN;

  public:
    /*!
     * \ref SecondFormatter default constructor
     */
    SecondFormatter();

    /*!
     * \ref SecondFormatter destructor
     */
    virtual ~SecondFormatter();

    /*!
     * Format date
     *
     * \param date The \ref Date to format
     * \param output The output stream to write in
     */
    virtual void format(const anch::date::Date& date, std::ostream& output) const noexcept override;

    /*!
     * Get size of formatter part
     *
     * \return The size of formatter part
     */
    virtual std::size_t getSize() const noexcept override;

    /*!
     * Set value to \ref Date
     *
     * \param date The \ref Date to modify
     * \param value The value to set
     */
    virtual bool setValue(anch::date::Date& date, const std::string& value) const noexcept override;

    /*!
     * Get formatter part pattern
     *
     * \return The formatter part pattern
     */
    virtual const std::string& getPattern() const noexcept override;

    /*!
     * Return a new instance of \ref SecondFormatter
     *
     * \return A new instance of \ref SecondFormatter
     */
    static IDatePartFormatter* getInstance();

  };

}
