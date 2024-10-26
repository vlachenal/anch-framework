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
#include "cli/formatter.hpp"

using anch::cli::Formatter;
using anch::cli::Color;

bool Formatter::DISABLED(false);

Formatter anch::cli::BOLD = Formatter::format().bold();
Formatter anch::cli::UNBOLD = Formatter::format().bold(false);
Formatter anch::cli::UNDERLINE = Formatter::format().underline();
Formatter anch::cli::UNUNDERLINE = Formatter::format().underline(false);
Formatter anch::cli::BLINK = Formatter::format().blink();
Formatter anch::cli::UNBLINK = Formatter::format().blink(false);
Formatter anch::cli::RESET = Formatter::format().reset();

Formatter::Formatter():
  _fg_color(),
  _rgb_fg_color(),
  _bg_color(),
  _rgb_bg_color(),
  _bold(),
  _underline(),
  _blink(),
  _reset(false) {
}

Formatter::~Formatter() {
}

Formatter
Formatter::format() {
  Formatter fmt;
  return fmt;
}

Formatter&
Formatter::bold(bool state) {
  _bold = state;
  return *this;
}

Formatter&
Formatter::underline(bool state) {
  _underline = state;
  return *this;
}

Formatter&
Formatter::blink(bool state) {
  _blink = state;
  return *this;
}

Formatter&
Formatter::reset() {
  _reset = true;
  return *this;
}

Formatter&
Formatter::fgColor(const Color& color) {
  _fg_color = color;
  return *this;
}

Formatter&
Formatter::fgColor(const std::array<uint8_t,3>& color) {
  _rgb_fg_color = color;
  return *this;
}

Formatter&
Formatter::bgColor(const Color& color) {
  _bg_color = color;
  return *this;
}

Formatter&
Formatter::bgColor(const std::array<uint8_t,3>& color) {
  _rgb_bg_color = color;
  return *this;
}

void
addSeparator(std::ostream& stream, bool first) {
  if(!first) {
    stream << ';';
  } else {
    stream << "\033[";
  }
}

void
addColor(std::ostream& stream, const Formatter& fmt, bool background, bool& first) {
  int offset = 30;
  std::optional<Color> color;
  std::optional<std::array<uint8_t,3>> rgb;
  if(background) {
    offset += 10;
    color = fmt._bg_color;
    rgb = fmt._rgb_bg_color;
  } else {
    color = fmt._fg_color;
    rgb = fmt._rgb_fg_color;
  }
  if(color.has_value()) {
    addSeparator(stream, first);
    stream << (offset + static_cast<int>(color.value()));
    first = false;
  } else if(rgb.has_value()) {
    addSeparator(stream, first);
    stream << (offset + 8) << ";2;" << static_cast<uint16_t>(rgb.value()[0]) << ';'
	   << static_cast<uint16_t>(rgb.value()[1]) << ';'
	   << static_cast<uint16_t>(rgb.value()[2]);
    first = false;
  }
}

void
addEffects(std::ostream& stream, const Formatter& fmt, bool& first) {
  if(fmt._bold.has_value()) {
    addSeparator(stream, first);
    stream << (fmt._bold.value() ? 1 : 22);
    first = false;
  }
  if(fmt._underline.has_value()) {
    addSeparator(stream, first);
    stream << (fmt._underline.value() ? 4 : 24);
    first = false;
  }
  if(fmt._blink.has_value()) {
    addSeparator(stream, first);
    stream << (fmt._blink.value() ? 5 : 25);
    first = false;
  }
}

std::ostream&
operator<<(std::ostream& stream, const Formatter& fmt) {
  if(Formatter::DISABLED) {
    return stream;
  }
  if(fmt._reset) {
    stream << "\033[0m\033[K";
    return stream;
  }
  bool first = true;
  addColor(stream, fmt, false, first); // Foreground
  addColor(stream, fmt, true, first); // Background
  addEffects(stream, fmt, first);
  if(!first) {
    stream << "m";
  }
  return stream;
}
