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
#include "codec/format.hpp"

#include "codec/text.hpp"

#include <algorithm>
#include <mutex>

using anch::codec::Format;

namespace {

  struct Registry {
    std::mutex mutex;
    std::vector<Format> formats;
  };

  Registry&
  registry() {
    static Registry instance;
    return instance;
  }

  [[maybe_unused]] const bool REGISTERED = anch::codec::registerFormat(Format{"text", {std::string(anch::codec::Text::mediaType())}});

}

bool
anch::codec::registerFormat(Format format) {
  Registry& reg = registry();
  std::lock_guard<std::mutex> lock(reg.mutex);
  auto iter = std::lower_bound(reg.formats.begin(), reg.formats.end(), format.name,
			       [](const Format& registered, const std::string& name) { return registered.name < name; });
  if(iter != reg.formats.end() && iter->name == format.name) {
    return false;
  }
  reg.formats.insert(iter, std::move(format));
  return true;
}

std::vector<Format>
anch::codec::formats() {
  Registry& reg = registry();
  std::lock_guard<std::mutex> lock(reg.mutex);
  return reg.formats;
}
