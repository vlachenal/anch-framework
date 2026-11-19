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

#include "log/mdc.hpp"

namespace anch::log {

  template<typename T, typename... Q>
  void
  Logger::trace(const T& value, const Q&... values) const noexcept {
    if constexpr (Level::TRACE >= MIN_LEVEL) {
      log(Level::TRACE, value, values...);
    }
  }

  template<typename T, typename... Q>
  void
  Logger::debug(const T& value, const Q&... values) const noexcept {
    if constexpr (Level::DEBUG >= MIN_LEVEL) {
      log(Level::DEBUG, value, values...);
    }
  }

  template<typename T, typename... Q>
  void
  Logger::info(const T& value, const Q&... values) const noexcept {
    if constexpr (Level::INFO >= MIN_LEVEL) {
      log(Level::INFO, value, values...);
    }
  }

  template<typename T, typename... Q>
  void
  Logger::warn(const T& value, const Q&... values) const noexcept {
    if constexpr (Level::WARN >= MIN_LEVEL) {
      log(Level::WARN, value, values...);
    }
  }

  template<typename T, typename... Q>
  void
  Logger::error(const T& value, const Q&... values) const noexcept {
    if constexpr (Level::ERROR >= MIN_LEVEL) {
      log(Level::ERROR, value, values...);
    }
  }

  template<typename T, typename... Q>
  void
  Logger::fatal(const T& value, const Q&... values) const noexcept {
    if constexpr (Level::FATAL >= MIN_LEVEL) {
      log(Level::FATAL, value, values...);
    }
  }

  template<typename T>
  void
  Logger::log(const Level& level, std::ostringstream& out, const T& value) const noexcept {
    try {
      out << value;
      write(level, out.str());
    } catch(...) {
      // Logging must never stop the application (this method is noexcept)
    }
  }

  template<typename T, typename... Q>
  void
  Logger::log(const Level& level, const T& value, const Q&... values) const noexcept {
    if(isEnabled(level)) {
      std::ostringstream out;
      log(level, out, value, values...);
    }
  }

  template<typename T, typename... Q>
  void
  Logger::log(const Level& level, std::ostringstream& out, const T& value, const Q&... values) const noexcept {
    out << value;
    log(level, out, values...);
  }

}
