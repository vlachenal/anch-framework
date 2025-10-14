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
#include "conf/resolvers.hpp"

#include <sstream>

#include "conf/configuration.hpp"
#include "conf/confError.hpp"

using anch::conf::Resolvers;
using anch::conf::ConfError;
using anch::conf::Configuration;


namespace anch::conf {

  std::string
  resolvEnv(const std::string& name,
	    [[maybe_unused]] const std::optional<std::string>& args,
	    const std::optional<std::string>& defaultVal) {
    char* env = std::getenv(name.data());
    if(env != NULL) {
      return std::string(env);
    }
    if(!defaultVal.has_value()) {
      std::ostringstream oss;
      oss << "Environment variable " << name << " has not been found and has no default value";
      throw ConfError(oss.str(), ConfError::ErrorCode::CONF_NOT_FOUND);
    }
    return defaultVal.value();
  }

  std::string
  resolvConf(const std::string& name,
	     [[maybe_unused]] const std::optional<std::string>& args,
	     const std::optional<std::string>& defaultVal) {
    auto val = Configuration::inst().value(name);
    if(val.has_value()) {
      return val.value();
    }
    if(!defaultVal.has_value()) {
      std::ostringstream oss;
      oss << "Environment variable " << name << " has not been found and has no default value";
      throw ConfError(oss.str(), ConfError::ErrorCode::CONF_NOT_FOUND);
    }
    return defaultVal.value();
  }

}

Resolvers::Resolvers(): _registry(), _protected() {
  // Resolvers registration +
  // Environment variables resolver +
  _registry["env"] = anch::conf::resolvEnv;
  _protected.insert("env");
  // Environment variables resolver -

  // Configuration variables resolver +
  _registry["cnf"] = anch::conf::resolvConf;
  _protected.insert("cnf");
  // Configuration variables resolver -

  // \todo check libs, load them and protect them
  // Resolvers registration -
}

Resolvers::~Resolvers() {
  // Nothing to do for now
}

Resolvers&
Resolvers::registerResolver(anch::conf::Resolver resolver, const std::string& key) {
  if(_protected.contains(key)) {
    std::ostringstream oss;
    oss << "Resolver " << key << " has been registered and is protected";
    throw ConfError(oss.str(), ConfError::ErrorCode::RES_PROTECTED);
  }
  _registry[key] = resolver;
  return *this;
}

anch::conf::Resolver
Resolvers::getResolver(const std::string& key) const {
  auto iter = _registry.find(key);
  if(iter == _registry.end()) {
    std::ostringstream oss;
    oss << "Resolver " << key << " has not been found";
    throw ConfError(oss.str(), ConfError::ErrorCode::RES_NOT_FOUND);
  }
  return iter->second;
}
