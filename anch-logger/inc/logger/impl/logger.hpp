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

namespace anch::logger {

  template<typename T, typename... Q>
  void
  Logger::trace(const T& value, const Q&... values) const noexcept {
    log(Level::TRACE, value, values...);
  }

  template<typename T, typename... Q>
  void
  Logger::debug(const T& value, const Q&... values) const noexcept {
    log(Level::DEBUG, value, values...);
  }

  template<typename T, typename... Q>
  void
  Logger::info(const T& value, const Q&... values) const noexcept {
    log(Level::INFO, value, values...);
  }

  template<typename T, typename... Q>
  void
  Logger::warn(const T& value, const Q&... values) const noexcept {
    log(Level::WARN, value, values...);
  }

  template<typename T, typename... Q>
  void
  Logger::error(const T& value, const Q&... values) const noexcept {
    log(Level::ERROR, value, values...);
  }

  template<typename T, typename... Q>
  void
  Logger::fatal(const T& value, const Q&... values) const noexcept {
    log(Level::FATAL, value, values...);
  }

  template<typename T>
  void
  Logger::log(const Level& level, std::ostringstream& out, const T& value) const noexcept {
    out << value;
    const std::string& message = out.str();
    for(size_t i = 0 ; i < _writers.size() ; i++) {
      _writers[i]->write(_name, level, message);
    }
  }

  template<typename T, typename... Q>
  void
  Logger::log(const Level& level, const T& value, const Q&... values) const noexcept {
    if(level >= _level && !_writers.empty()) {
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
