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
#include "sql/sqlSharedLibraries.hpp"

#include "sql/sqlException.hpp"

#include <sstream>
#include <dlfcn.h>

using anch::sql::SQLSharedLibrary;
using anch::sql::SQLSharedLibraries;
using anch::sql::Connection;
using anch::sql::SqlException;


// Constructors +
SQLSharedLibraries::SQLSharedLibraries() noexcept: _libraries() {
  // Nothing to do
}
// Constructors -

// Destructor +
SQLSharedLibraries::~SQLSharedLibraries() {
  for(auto iter = _libraries.begin() ; iter != _libraries.end() ; ++iter) {
    if(iter->second._layer != NULL) {
      dlclose(iter->second._layer);
    }
  }
}
// Destructor -

// Methods +
void
SQLSharedLibraries::registerSQLLibrary(const std::string& db, const std::string& library) {
  if(_libraries.find(db) == _libraries.end()) {
    SQLSharedLibrary lib;
    lib.layer = library;
    void* layerLib = dlopen(library.data(), RTLD_NOW);
    lib._layer = layerLib;
    if(layerLib == NULL) {
      _libraries[db] = lib;
      std::ostringstream oss;
      oss << "Can not open " << lib.layer << ": " << dlerror();
      throw SqlException(oss.str());
    }
    lib.make_shared_connection = (std::shared_ptr<Connection> (*)(const SqlConnectionConfiguration&))dlsym(layerLib, "create_shared_connection");
    lib.create = (Connection* (*)(const SqlConnectionConfiguration&))dlsym(layerLib, "create_connection");
    _libraries[db] = lib;
  }
}

std::shared_ptr<Connection>
SQLSharedLibraries::makeSharedConnection(const SqlConnectionConfiguration& config) {
  auto iter = _libraries.find(config.driver);
  if(iter == _libraries.end()) {
    std::ostringstream oss;
    oss << config.driver << " has not been registered";
    throw SqlException(oss.str());
  }
  return iter->second.make_shared_connection(config);
}

Connection*
SQLSharedLibraries::makeConnection(const SqlConnectionConfiguration& config) {
  auto iter = _libraries.find(config.driver);
  if(iter == _libraries.end()) {
    std::ostringstream oss;
    oss << config.driver << " has not been registered";
    throw SqlException(oss.str());
  }
  return iter->second.create(config);
}
// Methods -
