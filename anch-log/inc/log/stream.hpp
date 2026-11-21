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

#include "anch-log-export.hpp"

#include <optional>
#include <ostream>
#include <sstream>
#include <string>

#include "log/levels.hpp"
#include "log/logger.hpp"

namespace anch::log {

  /*!
   * Logging level known at compile time.\n
   * Instances are defined in \c anch::log::lvl namespace and are used to start a log line:
   * \snippet{trimleft} anch-log/examples/log.cpp stream
   *
   * \tparam L The logging level
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  template<anch::log::Level L>
  struct LevelTag {
    /*! The logging level */
    static constexpr anch::log::Level value = L;
  };

  /*!
   * Disabled log line.\n
   * Returned for levels lower than \ref anch::log::MIN_LEVEL "MIN_LEVEL" : every operation does nothing and is removed by the compiler.\n
   * Values are still evaluated.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class ANCH_LOG_EXPORT NoopStream {

    // Methods +
  public:
    /*!
     * Ignore value
     *
     * \return \c this
     */
    template<typename T>
    constexpr const NoopStream& operator<<(const T&) const noexcept;

    /*!
     * Ignore manipulator (\c std::endl , \c std::flush , ...)
     *
     * \return \c this
     */
    constexpr const NoopStream& operator<<(std::ostream& (*)(std::ostream&)) const noexcept;
    // Methods -

  };

  /*!
   * Log line.\n
   * Returned for levels greater or equal to \ref anch::log::MIN_LEVEL "MIN_LEVEL" . The logger level is checked once at construction:
   * when level is disabled, values are not formatted.\n
   * The message is written when the line is destroyed, i.e. at the end of the full expression.\n
   * \n
   * A formatting error drops the message. Logging never throws.\n
   * \n
   * Line is neither copyable nor movable. It must not be kept by reference:
   * <tt>auto& line = LOG << anch::log::lvl::debug;</tt> is a dangling reference.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class ANCH_LOG_EXPORT LogLine {

    // Attributes +
  private:
    /*! The logger */
    const anch::log::Logger& _logger;

    /*! The message level */
    anch::log::Level _level;

    /*! Message stream ; empty when level is disabled or when formatting failed */
    std::optional<std::ostringstream> _out;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref LogLine constructor
     *
     * \param logger The logger
     * \param level The message level
     */
    LogLine(const anch::log::Logger& logger, anch::log::Level level) noexcept;

    /*! Prohibit \ref LogLine copy */
    LogLine(const LogLine&) = delete;

    /*! Prohibit \ref LogLine move */
    LogLine(LogLine&&) = delete;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref LogLine destructor: write the message when enabled
     */
    ~LogLine() noexcept;
    // Destructor -

    // Methods +
  public:
    /*!
     * Append value to message
     *
     * \param value The value to append
     *
     * \return \c this
     */
    template<typename T>
    LogLine& operator<<(const T& value) noexcept;

    /*!
     * Apply manipulator (\c std::endl , \c std::hex , ...) on message
     *
     * \param manip The manipulator
     *
     * \return \c this
     */
    LogLine& operator<<(std::ostream& (*manip)(std::ostream&)) noexcept;

    /*!
     * Prohibit \ref LogLine copy
     */
    LogLine& operator=(const LogLine&) = delete;

    /*!
     * Prohibit \ref LogLine move
     */
    LogLine& operator=(LogLine&&) = delete;
    // Methods -

  };

  /*!
   * Start a log line.\n
   * Return type depends on level: \ref NoopStream when level is lower than \ref anch::log::MIN_LEVEL "MIN_LEVEL" , \ref LogLine otherwise.
   *
   * \param logger The logger
   * \param level The level (\c anch::log::lvl::debug , ...)
   *
   * \return the log line
   *
   * \since 0.2
   */
  template<anch::log::Level L>
  [[nodiscard]] auto operator<<(const anch::log::Logger& logger, anch::log::LevelTag<L> level) noexcept;

}

namespace anch::log::lvl {

  /*! Trace level */
  inline constexpr anch::log::LevelTag<anch::log::Level::TRACE> trace{};

  /*! Debug level */
  inline constexpr anch::log::LevelTag<anch::log::Level::DEBUG> debug{};

  /*! Information level */
  inline constexpr anch::log::LevelTag<anch::log::Level::INFO> info{};

  /*! Warning level */
  inline constexpr anch::log::LevelTag<anch::log::Level::WARN> warn{};

  /*! Error level */
  inline constexpr anch::log::LevelTag<anch::log::Level::ERROR> error{};

  /*! Fatal level */
  inline constexpr anch::log::LevelTag<anch::log::Level::FATAL> fatal{};

}

#include "log/impl/stream.hpp"
