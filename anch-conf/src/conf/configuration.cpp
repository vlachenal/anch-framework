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
#include "conf/configuration.hpp"

#include <filesystem>
#include <string_view>

#include "conf/parsers.hpp"

using anch::conf::Configuration;
using anch::conf::Section;
using anch::conf::Parsers;

const std::string ANCH_CONF("anch::conf");
const std::string ANCH_DEFAULT_PROFILE("anch::conf.default-profile");
const std::string ANCH_CONF_INC("anch::conf.includes");


/*!
 * Split \c std::string using delimiter, trim left and right and add items in \c std::vector when not empty
 *
 * \param delim the delimiter
 * \param str the \c std::string to split
 * \param res the \c std::vector to fill
 */
void
split(char delim, const std::string& str, std::vector<std::string>& res) {
  std::size_t cur = 0;
  while(std::size_t pos = str.find(delim, cur) != str.npos) {
    // trim left +
    std::string_view val = str.substr(cur, pos);
    val.remove_prefix(std::min(val.find_first_not_of(" "), val.size()));
    // trim left -
    // trim right +
    auto trimPos = val.find(' ');
    if(trimPos != val.npos) {
      val.remove_suffix(val.size() - trimPos);
    }
    // trim right -
    res.push_back(val.data());
    //res.push_back(str.substr(cur, pos));
    cur = pos + 1;
  }
  res.push_back(str.substr(cur));
}

/*!
 * Configuration file and parser to use
 */
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
  for(auto fiter = folders.begin() ; fiter != folders.end() ; ++fiter) {
    for(auto riter = parsers.getParsers().begin() ; riter != parsers.getParsers().end() ; ++riter) {
      std::filesystem::path check(*fiter);
      check /= std::filesystem::path(name + "." + riter->first);
      if(std::filesystem::exists(check)) {
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

  // Check files in any folder for any extension +
  std::optional<ConfFile> conf = selectConfFile(_folders, _name);
  if(!conf.has_value()) {
    std::ostringstream oss;
    oss << "File " << _name << " has not been found with registered extensions";
    throw ConfError(oss.str(), ConfError::ErrorCode::CONF_NOT_FOUND);
  }
  // Check files in any folder for any extension -

  // Parse file and fill section +
  std::invoke(*conf->parser, conf->file, _root);
  // Parse file and fill section -

  // Add profiles with environment variable (if found) when not set +
  if(_profiles.empty()) {
    char* env = std::getenv("ANCH_PROFILES");
    if(env != NULL) {
      split(',', std::getenv("ANCH_PROFILES"), _profiles);
    }
  }
  if(_profiles.empty()) {
    std::optional<std::string> defProf = value(ANCH_DEFAULT_PROFILE);
    if(defProf.has_value()) {
      split(',', defProf.value(), _profiles);
    }
  }
  // Add profiles with environment variable (if found) when not set -

  // Load configuration from profiles +
  for(const std::string& profile: _profiles) {
    loadProfile(profile);
  }
  // Load configuration from profiles -

  // Parse included files +
  std::optional<std::string> includes = value(ANCH_CONF_INC);
  if(includes.has_value()) {
    split(',', includes.value(), _includes);
    for(const std::string& inc: _includes) {
      conf = selectConfFile(_folders, inc);
      if(!conf.has_value()) {
	std::ostringstream oss;
	oss << "Included file " << _name << " has not been found with registered extensions";
	throw ConfError(oss.str(), ConfError::ErrorCode::CONF_NOT_FOUND);
      }
      std::invoke(*conf->parser, conf->file, _root);
    }
  }
  // Parse included files -

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
  if(path == ".") {
    return sec;
  }
  std::size_t cur = 0;
  while(std::size_t pos = path.find('.', cur) != path.npos) {
    auto iter = sec->getSections().find(path.substr(cur, pos));
    if(iter == sec->getSections().end()) {
      return NULL;
    }
    sec = &iter->second;
    cur = pos + 1;
  }
  auto iter = sec->getSections().find(path.substr(cur));
  if(iter == sec->getSections().end()) {
    return NULL;
  }
  sec = &iter->second;
  return sec;
}

std::optional<std::string>
Configuration::value(const std::string& path) const noexcept {
  return _root.getValue(path);
}
