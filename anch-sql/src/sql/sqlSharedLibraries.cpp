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
  if(_libraries.find(db) != _libraries.end()) {
    return;
  }
  // The library is registered only once it has been loaded with its functions: a failed registration can be retried
  void* layerLib = dlopen(library.data(), RTLD_NOW);
  if(layerLib == NULL) {
    std::ostringstream oss;
    oss << "Can not open " << library << ": " << dlerror();
    throw SqlException(oss.str());
  }
  SQLSharedLibrary lib;
  lib.layer = library;
  lib._layer = layerLib;
  lib.make_shared_connection = reinterpret_cast<std::shared_ptr<Connection> (*)(const SqlConnectionConfiguration&)>(dlsym(layerLib, "create_shared_connection"));
  lib.create = reinterpret_cast<Connection* (*)(const SqlConnectionConfiguration&)>(dlsym(layerLib, "create_connection"));
  if(lib.make_shared_connection == NULL || lib.create == NULL) {
    dlclose(layerLib);
    throw SqlException(library + " is not an AnCH SQL driver (create_shared_connection or create_connection not found)");
  }
  try {
    _libraries[db] = lib;
  } catch(...) {
    dlclose(layerLib);
    throw;
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
