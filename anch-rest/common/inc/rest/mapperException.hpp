/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <exception>
#include <string>

namespace anch::rest {

  class MapperException: public std::exception {
    // Attributes +
  private:
    /*! The error message */
    std::string _msg;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref MapperException constructor
     *
     * \param msg the error message
     */
    MapperException(const std::string& msg);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref MapperException destructor
     */
    virtual ~MapperException();
    // Destructor -

    // Methods +
  public:
    /*!
     * Returns error message
     *
     * \return the error message
     */
    virtual const char* what() const noexcept override;
    // Methods -

  };

}  // anch::rest
