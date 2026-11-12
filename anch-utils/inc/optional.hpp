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

#include <functional>
#include <optional>
#include <type_traits>
#include <utility>

namespace anch {

  /*!
   * Apply treatment if optional has value
   *
   * \param value the optional value
   * \param func the treatment to apply when optional has value
   */
  template<typename T>
  void applyIfValue(const std::optional<T>& value, std::function<void(const T&)> func);

  /*!
   * Apply treatment if optional has value or apply fallback treatment
   *
   * \param value the optional value
   * \param func the treatment to apply when optional has value
   * \param fallback the treatment to apply when optional has no value
   */
  template<typename T>
  void applyIfValueOrElse(const std::optional<T>& value, std::function<void(const T&)> func, std::function<void()> fallback);

  /*!
   * \brief \c std::optional usefull methods.
   *
   * Provides some missing (from my point of view) features to \c std::optional .\n
   * Every \c std::optional constructor and method is available. Assignments use these constructors.\n
   * Since C++23, \c std::optional provides \c transform , \c and_then and \c or_else which cover \ref map
   * and \ref ifValueOrElse use cases.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename T>
  class Optional: public std::optional<T> {

    // Constructors +
  public:
    /*! Inherits every \c std::optional constructor */
    using std::optional<T>::optional;

    /*!
     * \ref Optional constructor from \c std::optional copy
     *
     * \param other the \c std::optional to copy
     */
    constexpr Optional(const std::optional<T>& other);

    /*!
     * \ref Optional constructor from \c std::optional move
     *
     * \param other the \c std::optional to move
     */
    constexpr Optional(std::optional<T>&& other) noexcept(std::is_nothrow_move_constructible_v<T>);
    // Constructors -

    // Methods +
  public:
    /*!
     * Apply treatment if optional has value
     *
     * \param func the treatment to apply when optional has value
     */
    void ifValue(std::function<void(const T&)> func) const;

    /*!
     * Apply treatment if optional has value or apply fallback treatment
     *
     * \param func the treatment to apply when optional has value
     * \param fallback the treatment to apply when optional has no value
     */
    void ifValueOrElse(std::function<void(const T&)> func, std::function<void()> fallback) const;

    /*!
     * Map optional value into another type
     *
     * \param mapper the mapping function
     *
     * \return the mapped value when optional has value, an empty optional otherwise
     */
    template<typename U>
    Optional<U> map(std::function<U(const T&)> mapper) const;
    // Methods -

  };

}

#include "impl/optional.hpp"
