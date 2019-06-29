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

#ifdef ANCH_STD_OTP

#include <optional>
#include <functional>

namespace anch {

  /*!
   * Apply treatment if optional has value
   *
   * \param value the optional value
   * \param func the treatment to apply when optional has value
   */
  template<typename T>
  void applyIfValue(std::optional<T>& value, std::function<void(const T&)> func);

  /*!
   * Apply treatment if optional has value or apply fallback treatment
   *
   * \param value the optional value
   * \param func the treatment to apply when optional has value
   * \param fallback the treatment to apply when optional has no value
   */
  template<typename T>
  void applyIfValueOrElse(std::optional<T>& value, std::function<void(const T&)> func, std::function<void()> fallback);

  /*!
   * \brief \c std::optional usefull methods.
   *
   * Provides some missing (from my point of view) features to \c std::optional .
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename T>
  class Optional: std::optional<T> {
    // Constructors +
  public:
    /*!
     * \ref Optional default constructor
     */
    constexpr Optional() noexcept;

    /*!
     * \ref Optional constructor
     *
     * \param null the \c NULL value
     */
    constexpr Optional(std::nullopt_t null) noexcept;

    /*!
     * \ref Optional 'copy' construtor
     *
     * \param other the \c std::optional to copy
     */
    constexpr Optional(const std::optional<T>& other);

    /*!
     * \ref Optional 'move' constructor
     *
     * \param other the \c std::optional to move
     */
    constexpr Optional(std::optional<T>&& other) noexcept;

    /*!
     * \ref Optional converting 'copy' construtor
     *
     * \param other the \c std::optional to copy
     */
    template<typename U>
    Optional(const std::optional<U>& other);

    /*!
     * \ref Optional converting 'move' constructor
     *
     * \param other the \c std::optional to move
     */
    template<typename U>
    Optional(std::optional<U>&& other);

    /*!
     * \ref Optional constructor.\n
     * This constructor will initialize containing object with args.
     *
     * \param inplace parameters to force \c T construction
     * \param args the \c T type constructor arguments
     */
    template<typename... Args>
    constexpr explicit Optional(std::in_place_t inplace, Args&&... args);

    /*!
     * \ref Optional constructor.\n
     * This constructor will initialize containing object with args.
     *
     * \param inplace parameters to force \c T construction
     * \param ilist the \c std::list initializer
     * \param args the \c T type constructor arguments
     */
    template<typename U, typename... Args>
    constexpr explicit Optional(std::in_place_t inplace, std::initializer_list<U> ilist, Args&&... args);

    /*!
     * \ref Optional constructor.
     *
     * \param value the value to convert
     */
    template<typename U = T>
    constexpr Optional(U&& value);
    // Constructors -


    // Destructor +
  public:
    /*!
     * \ref Optional destructor
     */
    virtual ~Optional();
    // Destructor -


    // Methods +
  public:
    /*!
     * Apply treatment if optional has value
     *
     * \param func the treatment to apply when optional has value
     */
    void ifValue(std::function<void(const T&)> func);

    /*!
     * Apply treatment if optional has value or apply fallback treatment
     *
     * \param func the treatment to apply when optional has value
     * \param fallback the treatment to apply when optional has no value
     */
    void ifValueOrElse(std::function<void(const T&)> func, std::function<void()> fallback);

    /*!
     * Transform optional type to another.\n
     * It value is not set, return an empty optional.
     *
     * \param mapper the mapping function
     *
     * \return the new optional
     */
    template<typename U>
    Optional<U> map(std::function<U(const T&)> mapper) const;
    // Methods -

  };

  template<typename T>
  inline void
  applyIfValue(std::optional<T>& value, std::function<void(const T&)> func) {
    if(value) {
      func(*value);
    }
  }

  template<typename T>
  inline void
  applyIfValueOrElse(std::optional<T>& value, std::function<void(const T&)> func, std::function<void()> fallback) {
    if(value) {
      func(*value);
    } else {
      fallback();
    }
  }

  template<typename T>
  constexpr Optional<T>::Optional() noexcept: std::optional<T>() {
    // Nothing to do
  }

  template<typename T>
  constexpr Optional<T>::Optional(std::nullopt_t null) noexcept: std::optional<T>(null) {
    // Nothing to do
  }

  template<typename T>
  constexpr Optional<T>::Optional(const std::optional<T>& other): std::optional<T>(other) {
    // Nothing to do
  }

  template<typename T>
  constexpr Optional<T>::Optional(std::optional<T>&& other) noexcept: std::optional<T>(other) {
    // Nothing to do
  }

  template<typename T>
  template<typename U>
  Optional<T>::Optional(const std::optional<U>& other): std::optional<T>(other) {
    // Nothing to do
  }

  template<typename T>
  template<typename U>
  Optional<T>::Optional(std::optional<U>&& other): std::optional<T>(other) {
    // Nothing to do
  }

  template<typename T>
  template<typename... Args>
  constexpr Optional<T>::Optional(std::in_place_t inplace, Args&&... args): std::optional<T>(inplace, args...) {
    // Nothing to do
  }

  template<typename T>
  template<typename U, typename... Args>
  constexpr Optional<T>::Optional(std::in_place_t inplace, std::initializer_list<U> ilist, Args&&... args)
    : std::optional<T>(inplace, ilist, args...) {
    // Nothing to do
  }

  template<typename T>
  template<typename U>
  constexpr Optional<T>::Optional(U&& value): std::optional<T>(value) {
    // Nothing to do
  }

  template<typename T>
  Optional<T>::~Optional() {
    // Nothing to do
  }

  template<typename T>
  inline void
  Optional<T>::ifValue(std::function<void(const T&)> func) {
    if(this->has_value()) {
      func(this->value());
    }
  }

  template<typename T>
  inline void
  Optional<T>::ifValueOrElse(std::function<void(const T&)> func, std::function<void()> fallback) {
    if(this->has_value()) {
      func(this->value());
    } else {
      fallback();
    }
  }

  template<typename T>
  template<typename U>
  inline Optional<U>
  Optional<T>::map(std::function<U(const T&)> mapper) const {
    Optional<U> val;
    if(this->has_value()) {
      val = mapper(this->value());
    }
    return val;
  }

}
#endif // ANCH_STD_OPT
