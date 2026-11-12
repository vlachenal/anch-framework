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

namespace anch::log {

  // NoopStream +
  template<typename T>
  constexpr const NoopStream&
  NoopStream::operator<<(const T&) const noexcept {
    return *this;
  }

  constexpr const NoopStream&
  NoopStream::operator<<(std::ostream& (*)(std::ostream&)) const noexcept {
    return *this;
  }
  // NoopStream -

  // LogLine +
  inline
  LogLine::LogLine(const anch::log::Logger& logger, anch::log::Level level) noexcept:
    _logger(logger),
    _level(level),
    _out() {
    if(_logger.isEnabled(_level)) {
      try {
	_out.emplace();
      } catch(...) {
	// Logging must never stop the application
      }
    }
  }

  inline
  LogLine::~LogLine() noexcept {
    if(_out) {
      try {
	_logger.write(_level, _out->str());
      } catch(...) {
	// Logging must never stop the application
      }
    }
  }

  template<typename T>
  LogLine&
  LogLine::operator<<(const T& value) noexcept {
    if(_out) {
      try {
	*_out << value;
      } catch(...) {
	_out.reset(); // Drop partial message
      }
    }
    return *this;
  }

  inline LogLine&
  LogLine::operator<<(std::ostream& (*manip)(std::ostream&)) noexcept {
    if(_out) {
      try {
	manip(*_out);
      } catch(...) {
	_out.reset(); // Drop partial message
      }
    }
    return *this;
  }
  // LogLine -

  template<anch::log::Level L>
  [[nodiscard]] auto
  operator<<(const anch::log::Logger& logger, anch::log::LevelTag<L>) noexcept {
    if constexpr (L < anch::log::MIN_LEVEL) {
      return anch::log::NoopStream{};
    } else {
      return anch::log::LogLine{logger, L};
    }
  }

}
