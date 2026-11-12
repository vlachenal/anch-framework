/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2020 Vincent Lachenal

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

#include <exception>
#include <string>
#include <source_location>

namespace anch::ut {

  /*!
   * \brief Unit test assertion error
   *
   * Extends \c std::exception to provides assertion utility functions
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class AssertException: public std::exception {

    // Attributes +
  private:
    /*! Error message */
    std::string _msg;

    /*! Location of the failed assertion (line \c 0 when unknown) */
    std::source_location _location;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref AssertException constructor
     *
     * \param msg error message
     */
    AssertException(const std::string& msg);

    /*!
     * \ref AssertException constructor
     *
     * \param msg error message
     * \param location the failed assertion location
     *
     * \since 0.2
     */
    AssertException(const std::string& msg, const std::source_location& location);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref AssertException destructor
     */
    virtual ~AssertException();
    // Destructor -

    // Methods +
  public:
    /*!
     * Returns error message
     *
     * \return the message
     */
    virtual const char* what() const noexcept override;

    /*!
     * Failed assertion location getter
     *
     * \return the location (its line is \c 0 when location is unknown)
     *
     * \since 0.2
     */
    const std::source_location& location() const noexcept;
    // Methods -

  };

} // anch::ut
