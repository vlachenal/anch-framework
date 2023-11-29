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
#include "resource/file/configurationFileParser.hpp"

#include <fstream>
#include <sstream>

#include "resource/section.hpp"

using anch::resource::file::ConfigurationFileParser;
using anch::resource::file::ResourceFileException;
using anch::resource::Section;


// Static intialization +
const std::regex ConfigurationFileParser::_sectionPattern = std::regex("^\\[([^# \t]+)\\]");
const std::regex ConfigurationFileParser::_optionPattern = std::regex("^([^# \t]+)( |\t)*=( |\t)*(.+)");
const std::regex ConfigurationFileParser::_commentPattern = std::regex("( |\t)*([^\\\\]#).*");
// Static intialization -


ConfigurationFileParser::ConfigurationFileParser(const std::string& filePath):
  _filePath(filePath) {
  // Nothing to do
}

ConfigurationFileParser::~ConfigurationFileParser() {
  // Nothing to do
}

void
ConfigurationFileParser::getConfiguration(std::map<std::string, Section>& config) const {
  // Open file +
  std::ifstream file(_filePath);
  if(!file.is_open()) {
    throw ResourceFileException(_filePath, "File is not readable");
  }
  // Open file -

  // Parse file +
  std::string section;
  do {
    std::string out;
    std::getline(file, out);
    parseLine(out, section, config);

  } while(!file.eof());
  // Parse file -

  file.close();
}

void
ConfigurationFileParser::parseLine(const std::string& line,
				   std::string& currentSection,
				   std::map<std::string,Section>& config) const {
  std::smatch match;
  if(std::regex_search(line, match, _sectionPattern)) {
    currentSection = match[1];

  } else if(std::regex_search(line, match, _optionPattern)) {
    const std::string& option = match[1];
    std::string value = match[4];
    if(std::regex_search(value, match, _commentPattern)) {
      value = match.prefix();
    }
    auto iter = config.find(currentSection);
    if(iter == config.end()) {
      iter = config.insert(std::pair<std::string,Section>(currentSection, Section())).first;
    }
    iter->second.addParameter(option, value);
  }
}
