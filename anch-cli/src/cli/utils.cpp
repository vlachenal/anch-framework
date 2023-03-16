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
#include "cli/utils.hpp"

#include <iostream>

using anch::cli::BindArg;

void
setTrue(bool& dest, const std::string&) {
  dest = true;
}

BindArg
anch::cli::bindTrue(bool& dest) {
  return std::bind_front(setTrue, std::ref(dest));
}

void
setFalse(bool& dest, const std::string&) {
  dest = false;
}

BindArg
anch::cli::bindFalse(bool& dest) {
  return std::bind_front(setFalse, std::ref(dest));
}

void
setString(std::string& dest, const std::string& val) {
  dest = val;
}

BindArg
anch::cli::bindStr(std::string& dest) {
  return std::bind_front(setString, std::ref(dest));
}

void
setOptString(std::optional<std::string>& dest, const std::string& val) {
  dest = val;
}

BindArg
anch::cli::bindStr(std::optional<std::string>& dest) {
  return std::bind_front(setOptString, std::ref(dest));
}

void
pushVec(std::vector<std::string>& dest, const std::string& val) {
  dest.push_back(val);
}

BindArg
anch::cli::bindCol(std::vector<std::string>& dest) {
  return std::bind_front(pushVec, std::ref(dest));
}

void
pushList(std::list<std::string>& dest, const std::string& val) {
  dest.push_back(val);
}

BindArg
anch::cli::bindCol(std::list<std::string>& dest) {
  return std::bind_front(pushList, std::ref(dest));
}

void
pushSet(std::set<std::string>& dest, const std::string& val) {
  dest.insert(val);
}

BindArg
anch::cli::bindCol(std::set<std::string>& dest) {
  return std::bind_front(pushSet, std::ref(dest));
}

void
setInputStream(std::shared_ptr<std::istream>& dest, std::istream& in) {
  dest = std::make_shared<std::istream>(in.rdbuf());
}

std::function<void(std::istream&)>
anch::cli::bindPipe(std::shared_ptr<std::istream>& dest) {
  return std::bind_front(setInputStream, std::ref(dest));
}
