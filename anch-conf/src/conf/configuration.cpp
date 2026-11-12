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
#include "conf/configuration.hpp"

#include <filesystem>
#include <cstdlib>
#include <functional>
#include <sstream>
#include <string_view>

#include "conf/parsers.hpp"
#include "conf/placeholders.hpp"

using anch::conf::Configuration;
using anch::conf::Section;
using anch::conf::Parsers;
using anch::conf::Placeholders;

const std::string ANCH_DEFAULT_PROFILES("anch::conf.default-profiles");
const std::string ANCH_DEFAULT_PROFILE("anch::conf.default-profile"); // deprecated: default-profiles
const std::string ANCH_CONF_INC("anch::conf.includes");

/*!
 * Split string on delimiter and trim items (empty items are ignored)
 *
 * \param delim the delimiter
 * \param str the string to split
 *
 * \return the items
 */
std::vector<std::string>
splitTrim(char delim, std::string_view str) {
  std::vector<std::string> res;
  while(true) {
    const std::size_t pos = str.find(delim);
    std::string_view item = str.substr(0, pos);
    const std::size_t first = item.find_first_not_of(" \t");
    if(first != std::string_view::npos) {
      res.emplace_back(item.substr(first, item.find_last_not_of(" \t") - first + 1));
    }
    if(pos == std::string_view::npos) {
      break;
    }
    str.remove_prefix(pos + 1);
  }
  return res;
}

struct ConfFile {
  std::filesystem::path file;
  const anch::conf::Parser* parser = NULL;
};


/*!
 * Select the configuration file and the parser to use
 *
 * \param folders the folders to look for file
 * \param name the file name to look for without extension
 *
 * \return empty when not found, the file and parser to use otherwise
 */
std::optional<ConfFile>
selectConfFile(const std::vector<std::string>& folders, const std::string& name) {
  Parsers& parsers = Parsers::getInstance();
  for(auto fiter = folders.begin() ; fiter != folders.end() ; ++fiter) { // for each folder
    for(auto riter = parsers.getParsers().begin() ; riter != parsers.getParsers().end() ; ++riter) { // look for extension
      std::filesystem::path check(*fiter);
      check /= std::filesystem::path(name + "." + riter->first);
      if(std::filesystem::exists(check)) { // when found return file
	ConfFile conf;
	conf.file = check;
	conf.parser = &riter->second;
	return std::optional<ConfFile>(conf);
      }
    }
  }
  return std::optional<ConfFile>();
}


Configuration::Configuration():
  _loaded(false),
  _name("application"),
  _profiles(),
  _includes(),
  _folders(),
  _root() {
  // Nothing to do
}

Configuration::~Configuration() {
  // Nothing to do
}

Configuration&
Configuration::folders(const std::vector<std::filesystem::path>& folders) noexcept {
  if(_loaded) {
    return *this;
  }
  // Register folder when exists and is directory +
  for(auto iter = folders.begin() ; iter != folders.end() ; ++iter) {
    if(!std::filesystem::exists(*iter)) {
      continue;
    }
    if(!std::filesystem::is_directory(*iter)) {
      continue;
    }
    _folders.push_back(*iter);
  }
  // Register folder when exists and is directory -
  return *this;
}

Configuration&
Configuration::load() {
  if(_loaded) {
    return *this;
  }
  // Add current folder as default when not set +
  if(_folders.empty()) {
    _folders.push_back(".");
  }
  // Add current folder as default when not set -

  // Main file +
  std::optional<ConfFile> conf = selectConfFile(_folders, _name);
  if(!conf.has_value()) {
    std::ostringstream oss;
    oss << "File " << _name << " has not been found with registered extensions";
    throw ConfError(oss.str(), ConfError::ErrorCode::CONF_NOT_FOUND);
  }
  std::invoke(*conf->parser, conf->file, _root);
  // Main file -

  // Included files (declared in main file) +
  _includes.clear();
  std::optional<std::string> includes = value(ANCH_CONF_INC);
  if(includes.has_value()) {
    _includes = splitTrim(',', includes.value());
  }
  for(const std::string& inc: _includes) {
    conf = selectConfFile(_folders, inc);
    if(!conf.has_value()) {
      std::ostringstream oss;
      oss << "Included file " << inc << " has not been found with registered extensions";
      throw ConfError(oss.str(), ConfError::ErrorCode::CONF_NOT_FOUND);
    }
    std::invoke(*conf->parser, conf->file, _root);
  }
  // Included files -

  // Profiles: parameter, then ANCH_PROFILES environment variable, then default profiles +
  if(_profiles.empty()) {
    const char* env = std::getenv("ANCH_PROFILES");
    if(env != NULL) {
      _profiles = splitTrim(',', env);
    }
  }
  if(_profiles.empty()) {
    std::optional<std::string> defProf = value(ANCH_DEFAULT_PROFILES);
    if(!defProf.has_value()) {
      defProf = value(ANCH_DEFAULT_PROFILE);
    }
    if(defProf.has_value()) {
      _profiles = splitTrim(',', defProf.value());
    }
  }
  // Profiles -

  // Profile files override previous values, in profiles order +
  for(const std::string& profile: _profiles) {
    loadProfile(profile);
  }
  // Profile files -

  // Resolve values with placeholders +
  Placeholders placeholders(_root);
  placeholders.resolve();
  // Resolve values with placeholders -

  _loaded = true;
  return *this;
}

void
Configuration::loadProfile(const std::string& profile) {
  // Default file with profile parsing +
  std::string file = _name + "-" + profile;
  std::optional<ConfFile> conf = selectConfFile(_folders, file);
  if(conf.has_value()) {
    std::invoke(*conf->parser, conf->file, _root);
  }
  // Default file with profile parsing -

  // Included files with profile parsing +
  for(const std::string& inc: _includes) {
    file = inc + "-" + profile;
    conf = selectConfFile(_folders, file);
    if(conf.has_value()) {
      std::invoke(*conf->parser, conf->file, _root);
    }
  }
  // Included files with profile parsing -
}

const anch::conf::Section*
Configuration::section(const std::string& path) const noexcept {
  const Section* sec = &_root;
  // if path == '.' return root section +
  if(path == ".") {
    return sec;
  }
  // if path == '.' return root section -
  std::string_view rest(path);
  while(true) {
    const std::size_t pos = rest.find('.');
    auto iter = sec->getSections().find(std::string(rest.substr(0, pos)));
    if(iter == sec->getSections().end()) {
      return NULL;
    }
    sec = &iter->second;
    if(pos == std::string_view::npos) {
      return sec;
    }
    rest.remove_prefix(pos + 1);
  }
}

std::optional<std::string>
Configuration::value(const std::string& path) const noexcept {
  return _root.getValue<std::string>(path);
}
