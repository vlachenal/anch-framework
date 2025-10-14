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
#include "conf/parsers.hpp"

#include "ini/parser.hpp"

using anch::conf::Parsers;
using anch::conf::Parser;
using anch::conf::Section;
using anch::conf::ConfError;


Parsers::Parsers(): _registry() {
  // Register anch::ini parser for ini, conf, cnf and properties extensions and marked them as protected +
  std::set<std::string> iniExts = {
    "ini",
    "conf",
    "cnf",
    "properties"
  };
  registerParser(anch::ini::merge, iniExts);
  _protected = iniExts;
  // Register anch::ini parser for ini, conf, cnf and properties extensions and marked them as protected -
#ifdef ANCH_JSON
  // Register anch::json parser for json extension and marked it as protected +
  std::set<std::string> jsonExts = {
    "json"
  };
  // \todo register JSON parser
  _protected = jsonExts;
  // Register anch::json parser for json extension and marked it as protected -
#endif
#ifdef ANCH_YAML
  // Register anch::yaml parser for yaml and yml extensions and marked them as protected +
  std::set<std::string> yamlExts = {
    "yaml",
    "yml"
  };
  // \todo register YAML parser
  _protected = yamlExts;
  // Register anch::yaml parser for yaml and yml extensions and marked them as protected -
#endif
#ifdef ANCH_XML
  // Register anch::xml parser for xml extension and marked it as protected +
  std::set<std::string> xmlExts = {
    "xml"
  };
  // \todo register XML parser
  _protected = xmlExts;
  // Register anch::xml parser for xml extension and marked it as protected -
#endif
}

Parsers::~Parsers() {
  // Nothing to do
}

Parsers&
Parsers::registerParser(Parser parser, const std::set<std::string>& extensions) {
  for(const std::string& ext : extensions) {
    if(_protected.contains(ext)) {
      std::ostringstream oss;
      oss << "Extension " << ext << " has been registered for parser which is protected";
      throw ConfError(oss.str(), ConfError::ErrorCode::PARSER_PROTECTED);
    }
    _registry[ext] = parser;
  }
  return *this;
}

Parser
Parsers::getParser(const std::string& extension) const {
  auto iter = _registry.find(extension);
  if(iter == _registry.end()) {
    std::ostringstream oss;
    oss << "Extension " << extension << " has no registered parser";
    throw ConfError(oss.str(), ConfError::ErrorCode::PARSER_NOT_FOUND);
  }
  return iter->second;
}
