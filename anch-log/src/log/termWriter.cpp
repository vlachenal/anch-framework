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
#include "log/termWriter.hpp"

#include <iostream>

#include "log/mdc.hpp"
#include "log/levels.hpp"
#include "cli/formatter.hpp"

using anch::log::ConsoleWriter;
using anch::log::MDC;


ConsoleWriter::ConsoleWriter(const anch::conf::Section& conf): anch::log::Writer(conf), _mutex() {
  _color = conf.getValue<bool>("color", false);
}

ConsoleWriter::~ConsoleWriter() {
  // Nothing to do
}

void
ConsoleWriter::write(const std::string& message) {
  std::lock_guard<std::mutex> lock(_mutex);
  // No color +
  if(!_color) {
    std::cout << format(message) << std::endl;
    return;
  }
  // No color -
  // Log with color +
  switch(anch::log::LABEL_LEVEL.at(MDC.get().at("level"))) {
  case Level::TRACE:
    std::cout << anch::cli::Formatter::format().fgColor({127,127,127}) // Light/mid gray
	      << format(message) << anch::cli::RESET << std::endl;
    break;
  case Level::INFO:
    std::cout << anch::cli::Formatter::format().fgColor(anch::cli::Color::BLUE)
	      << format(message) << anch::cli::RESET << std::endl;
    break;
  case Level::WARN:
    std::cout << anch::cli::Formatter::format().fgColor(anch::cli::Color::YELLOW)
	      << format(message) << anch::cli::RESET << std::endl;
    break;
  case Level::ERROR:
    std::cout << anch::cli::Formatter::format().fgColor(anch::cli::Color::RED)
	      << format(message) << anch::cli::RESET << std::endl;
    break;
  case Level::FATAL:
    std::cout << anch::cli::Formatter::format().bgColor(anch::cli::Color::RED).blink()
	      << format(message) << anch::cli::RESET << std::endl;
    break;
  default:
    std::cout << format(message) << std::endl;
  }
  // Log with color -
}
// Methods -
