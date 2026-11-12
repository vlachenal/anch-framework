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

namespace anch {

  template<typename T>
  inline void
  applyIfValue(const std::optional<T>& value, std::function<void(const T&)> func) {
    if(value) {
      func(*value);
    }
  }

  template<typename T>
  inline void
  applyIfValueOrElse(const std::optional<T>& value, std::function<void(const T&)> func, std::function<void()> fallback) {
    if(value) {
      func(*value);
    } else {
      fallback();
    }
  }

  template<typename T>
  constexpr Optional<T>::Optional(const std::optional<T>& other): std::optional<T>(other) {
    // Nothing to do
  }

  template<typename T>
  constexpr Optional<T>::Optional(std::optional<T>&& other) noexcept(std::is_nothrow_move_constructible_v<T>):
    std::optional<T>(std::move(other)) {
    // Nothing to do
  }

  template<typename T>
  inline void
  Optional<T>::ifValue(std::function<void(const T&)> func) const {
    if(this->has_value()) {
      func(**this);
    }
  }

  template<typename T>
  inline void
  Optional<T>::ifValueOrElse(std::function<void(const T&)> func, std::function<void()> fallback) const {
    if(this->has_value()) {
      func(**this);
    } else {
      fallback();
    }
  }

  template<typename T>
  template<typename U>
  inline Optional<U>
  Optional<T>::map(std::function<U(const T&)> mapper) const {
    if(this->has_value()) {
      return Optional<U>(std::in_place, mapper(**this));
    }
    return Optional<U>();
  }

}
