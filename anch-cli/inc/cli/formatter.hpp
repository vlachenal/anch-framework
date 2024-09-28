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

  enum class Color {
    BLACK = 0,
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENTA = 5,
    CYAN = 6,
    WHITE = 7,
    DEFAULT = 9
  };

  struct Formatter {
    static bool DISABLED;
    std::optional<Color> _fg_color;
    std::optional<std::array<uint8_t,3>> _rgb_fg_color;
    std::optional<Color> _bg_color;
    std::optional<std::array<uint8_t,3>> _rgb_bg_color;
    std::optional<bool> _bold = false;
    std::optional<bool> _underline = false;
    std::optional<bool> _blink = false;
    bool _reset = false;
    Formatter();
    //Formatter(const Formatter& formatter);
    virtual ~Formatter();
    static Formatter format();
    Formatter& bold(bool state = true);
    Formatter& underline(bool state = true);
    Formatter& blink(bool state = true);
    Formatter& reset();
    Formatter& fgColor(const Color& color);
    Formatter& fgColor(const std::array<uint8_t,3>& color);
    Formatter& bgColor(const Color& color);
    Formatter& bgColor(const std::array<uint8_t,3>& color);
  };

  extern anch::cli::Formatter BOLD;
  extern anch::cli::Formatter UNBOLD;
  extern anch::cli::Formatter UNDERLINE;
  extern anch::cli::Formatter UNUNDERLINE;
  extern anch::cli::Formatter BLINK;
  extern anch::cli::Formatter UNBLINK;
  extern anch::cli::Formatter RESET;

}  // anch::cli

std::ostream& operator<<(std::ostream& stream, const anch::cli::Formatter& fmt);
