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

#include <functional>


namespace anch {

  /*!
   * \brief Manage collection as stream
   *
   * \tparam T the collection's type
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename T>
  class Flux {

    // Attributes +
  private:
    /*! Consume object function */
    std::function<void(const T&)> _consume;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Forbids \ref Flux default constructor
     */
    Flux() = delete;

    /*!
     * \ref Flux constructor
     *
     * \param consume the consume object function to use
     */
    Flux(std::function<void(const T&)> consume);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Flux destructor
     */
    ~Flux();
    // Destructor -

    // Methods +
  public:
    /*!
     * Push object in stream
     *
     * \param object the object to push in stream
     */
    void push(const T& object);

    /*!
     * \brief Handle error
     *
     * This method has to be implemented for each \c T implementation when \c push method can raise error.\n
     * Default implementation will only rethrow the error.
     */
    void handleError();
    // Methods -

  };

}

#include "impl/flux.hpp"
