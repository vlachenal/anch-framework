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
#include "cli/formatter.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <cstdlib>
#include <cstring>


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
setPath(std::filesystem::path& dest, const std::string& val) {
  dest = std::filesystem::path(val);
}

BindArg
anch::cli::bindPath(std::filesystem::path& dest) {
  return std::bind_front(setPath, std::ref(dest));
}

void
setIFS(std::shared_ptr<std::istream>& dest, const std::string& val) {
  std::filesystem::path path(val);
  if(!std::filesystem::exists(path)) {
    std::ostringstream oss;
    oss << path << " does not exist";
    throw std::invalid_argument(oss.str());
  }
  if(!std::filesystem::is_regular_file(path)) {
    std::ostringstream oss;
    oss <<  "File path " << val << " is not a regular file";
    throw std::invalid_argument(oss.str());
  }
  dest = std::make_shared<std::ifstream>(path);
}

BindArg
anch::cli::bindIFS(std::shared_ptr<std::istream>& dest) {
  return std::bind_front(setIFS, std::ref(dest));
}

void
setOS(std::shared_ptr<std::ostream>& dest, const std::string& val) {
  dest = std::make_shared<std::ofstream>(std::filesystem::path(val));
}

BindArg
anch::cli::bindOFS(std::shared_ptr<std::ostream>& dest) {
  return std::bind_front(setOS, std::ref(dest));
}

void
setOFS(std::shared_ptr<std::ofstream>& dest, std::ios_base::openmode mode, const std::string& val) {
  dest = std::make_shared<std::ofstream>(std::filesystem::path(val), mode);
}

BindArg
anch::cli::bindOFS(std::shared_ptr<std::ofstream>& dest, std::ios_base::openmode mode) {
  return std::bind_front(setOFS, std::ref(dest), mode);
}

void
setInputStream(std::shared_ptr<std::istream>& dest, std::istream& in) {
  dest = std::make_shared<std::istream>(in.rdbuf());
}

std::function<void(std::istream&)>
anch::cli::bindPipe(std::shared_ptr<std::istream>& dest) {
  return std::bind_front(setInputStream, std::ref(dest));
}

void
anch::cli::manageEnvNoFormat() {
  if(const char* envNoFmt = std::getenv("ANCH_CLI_NO_FMT")) {
    if(envNoColor != NULL && std::strcmp(envNoFmt, "1") == 0) {
      anch::cli::Formatter::DISABLED = true;
    }
  }
}
