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

#include <cstdint>
#include <array>
#include <optional>
#include <ostream>

namespace anch::cli {

  /*!
   * Console color definition
   *
   * \author Vincent Lachenal
   */
  enum class Color {

    /*! Black */
    BLACK = 0,

    /*! Red */
    RED = 1,

    /*! Green */
    GREEN = 2,

    /*! Yellow */
    YELLOW = 3,

    /*! Blue */
    BLUE = 4,

    /*! Magenta */
    MAGENTA = 5,

    /*! Cyan */
    CYAN = 6,

    /*! White */
    WHITE = 7,

    /*! Default color console */
    DEFAULT = 9

  };

  /*!
   * \brief Console formatter
   *
   * Console formatter utility in fluent API
   *
   * \author Vincent Lachenal
   */
  struct Formatter {

    /*! Disable console formatter globally */
    static bool DISABLED;

    /*! Text color (with predefined colors) */
    std::optional<Color> _fg_color;

    /*! Text color (with RGB colors) */
    std::optional<std::array<uint8_t,3>> _rgb_fg_color;

    /*! Background color (with predefined colors) */
    std::optional<Color> _bg_color;

    /*! Background color (with RGB colors) */
    std::optional<std::array<uint8_t,3>> _rgb_bg_color;

    /*! Bold text flag */
    std::optional<bool> _bold = false;

    /*! Underline text flag */
    std::optional<bool> _underline = false;

    /*! Blink text flag */
    std::optional<bool> _blink = false;

    /*! Reset formatting flag */
    bool _reset = false;

    /*!
     * \c Formatter default constructor
     */
    Formatter();

    /*!
     * \c Formatter destructor
     */
    virtual ~Formatter();

    /*!
     * Initialize formatter for fluent API usage
     *
     * \return the new \c Formatter
     */
    static Formatter format();

    /*!
     * Set bold/unbold
     *
     * \param state the flag (default to \c true )
     *
     * \return \c this
     */
    Formatter& bold(bool state = true);

    /*!
     * Set underline/ununderline
     *
     * \param state the flag (default to \c true )
     *
     * \return \c this
     */
    Formatter& underline(bool state = true);

    /*!
     * Set blink/unblink
     *
     * \param state the flag (default to \c true )
     *
     * \return \c this
     */
    Formatter& blink(bool state = true);

    /*!
     * Set reset flag to \c true . Every other flag will be ignored.
     *
     * \return \c this
     */
    Formatter& reset();

    /*!
     * Set text color
     *
     * \param color the predefined color to use
     *
     * \return \c this
     */
    Formatter& fgColor(const Color& color);

    /*!
     * Set text color
     *
     * \param color the RGB color to use
     *
     * \return \c this
     */
    Formatter& fgColor(const std::array<uint8_t,3>& color);

    /*!
     * Set background color
     *
     * \param color the predefined color to use
     *
     * \return \c this
     */
    Formatter& bgColor(const Color& color);

    /*!
     * Set background color
     *
     * \param color the RGB color to use
     *
     * \return \c this
     */
    Formatter& bgColor(const std::array<uint8_t,3>& color);

  };

  /*! Predefined bold formatter */
  extern anch::cli::Formatter BOLD;

  /*! Predefined unbold formatter */
  extern anch::cli::Formatter UNBOLD;

  /*! Predefined underline formatter */
  extern anch::cli::Formatter UNDERLINE;

  /*! Predefined ununderline formatter */
  extern anch::cli::Formatter UNUNDERLINE;

  /*! Predefined blink formatter */
  extern anch::cli::Formatter BLINK;

  /*! Predefined unblink formatter */
  extern anch::cli::Formatter UNBLINK;

  /*! Predefined reset formatter */
  extern anch::cli::Formatter RESET;

}  // anch::cli

/*!
 * \c Formatter output stream operator
 *
 * \param stream the output stream to use
 * \param fmt the formatter to use
 *
 * \return the output stream
 */
std::ostream&
operator<<(std::ostream& stream, const anch::cli::Formatter& fmt);
