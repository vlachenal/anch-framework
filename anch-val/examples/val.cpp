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
// anch-val documentation examples (anch-val.dox)
#include "val/constraints.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

//! [context]
// Context of a format which checks constraints while it reads values: the first violation raises its error
struct ReadContext {
  std::string path;
};

template<>
struct anch::val::CheckContext<ReadContext> {
  static bool
  enabled(const ReadContext&) {
    return true; // format option
  }

  static void
  violation(ReadContext& context, const std::string& message) {
    throw std::invalid_argument(context.path + ": " + message);
  }

  template<typename F>
  static void
  item(ReadContext& context, std::string_view key, F&& check) {
    const std::size_t size = context.path.size();
    context.path += "/" + std::string(key);
    check();
    context.path.resize(size);
  }

  template<typename F>
  static void
  item(ReadContext& context, std::size_t index, F&& check) {
    const std::size_t size = context.path.size();
    context.path += "/" + std::to_string(index);
    check();
    context.path.resize(size);
  }
};
//! [context]

int
main() {
  //! [check]
  // A constraint is a value: check(value, message) ; its parameters are public (schema generators)
  const anch::val::MinLength minLength = anch::val::minLength(1);
  std::string message;
  if(!minLength.check(std::string(""), message)) {
    std::cout << message << " (minLength " << minLength.value << ')' << std::endl; // Length 0 is lower than minLength 1
  }
  //! [check]

  //! [validation]
  // Validation context: every violation is collected with its path (JSON Pointer)
  anch::val::ValidationContext validation;
  const std::vector<std::string> tags{"dog", "", "brown"};
  anch::val::detail::checkConstraints(tags, validation, anch::val::maxItems(2), anch::val::items(anch::val::minLength(1)));
  for(const anch::val::Violation& violation : validation.violations()) {
    std::cout << '\'' << violation.path << "': " << violation.message << std::endl;
  }
  // '': Item count 3 is greater than maxItems 2
  // '/1': Length 0 is lower than minLength 1
  //! [validation]

  //! [format]
  ReadContext context;
  try {
    anch::val::detail::checkConstraints(tags, context, anch::val::items(anch::val::minLength(1)));
  } catch(const std::invalid_argument& error) {
    std::cout << error.what() << std::endl; // /1: Length 0 is lower than minLength 1
  }
  //! [format]
  return validation.violations().size() == 2 ? 0 : 1;
}
