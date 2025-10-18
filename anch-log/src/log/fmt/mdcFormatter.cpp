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
#include "logger/formatter/mdcFormatter.hpp"

#include "logger/mdc.hpp"

using anch::logger::formatter::MDCFormatter;
using anch::logger::formatter::FormatterType;

MDCFormatter::MDCFormatter(const std::string& key): _key(key) {
  // Nothing to do
}

MDCFormatter::~MDCFormatter() {
  // Nothing to do
}

void
MDCFormatter::formatValue([[ maybe_unused ]] const void* const value, std::ostream& out) const noexcept {
  auto mdc =  anch::logger::MDC.get();
  auto iter = mdc.find(_key);
  if(iter ==  mdc.end()) {
    out << "undefined";
  } else {
    out << iter->second;
  }
}

FormatterType
MDCFormatter::getType() const noexcept {
  return FormatterType::MDC;
}
