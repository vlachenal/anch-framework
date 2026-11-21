/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <optional>
#include <string>

//! [example]
#include "conf/configuration.hpp"

int
main() {
  const anch::conf::Configuration& conf = anch::conf::Configuration::loader()
    .name("application")
    .folders({"/etc/myapp", "."})
    .load();
  std::optional<std::string> url = conf.value("server.url");
  std::cout << "server.url=" << url.value_or("<none>") << std::endl;
  return 0;
}
//! [example]
