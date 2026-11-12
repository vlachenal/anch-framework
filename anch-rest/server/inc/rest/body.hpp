/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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

#include <utility>

namespace anch::rest {

  /*!
   * Request body endpoint argument.\n
   * It has to be the last endpoint argument. The body is deserialized with the codec matching the
   * request \c Content-Type (see \ref Endpoint::consumes ).
   *
   * \tparam T the body type (default constructible)
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  template<typename T>
  class Body {

    // Attributes +
  private:
    /*! The body value */
    T _value;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Body constructor
     *
     * \param value the body value
     */
    explicit Body(T value);
    // Constructors -

    // Methods +
  public:
    /*!
     * Access body value
     *
     * \return the body value
     */
    T& operator*() noexcept;

    /*!
     * Access body value
     *
     * \return the body value
     */
    const T& operator*() const noexcept;

    /*!
     * Access body value members
     *
     * \return the body value address
     */
    T* operator->() noexcept;

    /*!
     * Access body value members
     *
     * \return the body value address
     */
    const T* operator->() const noexcept;

    /*!
     * Get body value
     *
     * \return the body value
     */
    T& get() noexcept;
    // Methods -

  };

}

#include "rest/impl/body.hpp"
