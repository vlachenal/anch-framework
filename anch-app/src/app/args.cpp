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
#include "app/args.hpp"

using anch::app::Arguments;


// Constructors +
Arguments::Arguments(): level(), profiles(), _args() {
  // Nothing to do
}
// Constructors -

// Destructor +
Arguments::~Arguments() {
  // Nothing to do
}
// Destructor -

// Methods +
void
Arguments::verbose() {
  if(!level) {
    level = anch::log::Level::INFO;
  }
  switch(level.value()) {
  case anch::log::Level::DEBUG:
    level = anch::log::Level::TRACE;
    break;
  case anch::log::Level::INFO:
    level = anch::log::Level::DEBUG;
    break;
  case anch::log::Level::WARN:
    level = anch::log::Level::INFO;
    break;
  case anch::log::Level::ERROR:
    level = anch::log::Level::WARN;
    break;
  case anch::log::Level::FATAL:
    level = anch::log::Level::ERROR;
    break;
  default:
    break;
  }
}

void
Arguments::quiet() {
  if(!level) {
    level = anch::log::Level::INFO;
  }
  switch(level.value()) {
  case anch::log::Level::TRACE:
    level = anch::log::Level::DEBUG;
    break;
  case anch::log::Level::DEBUG:
    level = anch::log::Level::INFO;
    break;
  case anch::log::Level::INFO:
    level = anch::log::Level::WARN;
    break;
  case anch::log::Level::WARN:
    level = anch::log::Level::ERROR;
    break;
  case anch::log::Level::ERROR:
    level = anch::log::Level::FATAL;
    break;
  default:
    break;
  }
}

void
Arguments::addProfile(const std::string& profile) {
  if(!profiles) {
    profiles = std::set<std::string>();
  }
  profiles->insert(profile);
}
// Methods -
