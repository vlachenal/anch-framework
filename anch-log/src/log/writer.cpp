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
#include <fstream>
#include <sstream>
#include <filesystem>

#include "log/writer.hpp"
#include "log/mdc.hpp"
#include "log/levels.hpp"


using anch::log::Writer;
using anch::log::Level;
using anch::log::MDC;
using anch::log::fmt::MessageFormatter;


Writer::Writer(const anch::conf::Section& conf): _formatter() {
  std::optional<std::string> optVal = conf.getValue<std::string>("pattern");
  if(!optVal.has_value()) {
    _formatter.parserPattern("%m");
  } else {
    _formatter.parserPattern(optVal.value());
  }
}

Writer::~Writer() {
  // Nothing to do
}

std::string
Writer::format(const std::string& msg) {
  const std::string& logger = MDC.get().at("logger");
  const std::string& level = MDC.get().at("level");
  return _formatter.formatMessage(logger, anch::log::LABEL_LEVEL.at(level), msg);
}
