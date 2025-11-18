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

#include "log/fmt/iFormatter.hpp"
#include "log/levels.hpp"

namespace anch::log::fmt {

  /*!
   * Logging level formatter
   *
   * \author Vincent Lachenal
   */
  class LevelFormatter: public anch::log::fmt::IFormatter {
  public:
    // Constructors +
    /*!
     * \ref LevelFormatter default constructor
     */
    LevelFormatter();
    // Constructors -

    // Destructor +
    /*!
     * \ref LevelFormatter destructor
     */
    virtual ~LevelFormatter();
    // Destructor -

  public:
    /*!
     * Return the level label
     *
     * \param value The logging level
     * \param out The output stream to write in
     */
    virtual void formatValue(const void* const value,
			     std::ostream& out)
      const noexcept;

    /*!
     * Get the formatter type
     *
     * \return The formatter type
     */
    virtual anch::log::fmt::FormatterType getType() const noexcept;
  };

}
