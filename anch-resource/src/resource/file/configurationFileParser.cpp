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

using std::string;
using std::map;
using std::pair;
using std::ifstream;
using std::getline;

#ifdef ANCH_BOOST_REGEX
using boost::regex;
using boost::smatch;
using boost::regex_search;
#else
using std::regex;
using std::smatch;
using std::regex_search;
#endif

using anch::resource::file::ConfigurationFileParser;
using anch::resource::file::ResourceFileException;
using anch::resource::Section;


// Static intialization +
const regex ConfigurationFileParser::_sectionPattern = regex("^\\[([^# \t]+)\\]");
const regex ConfigurationFileParser::_optionPattern = regex("^([^# \t]+)( |\t)*=( |\t)*(.+)");
const regex ConfigurationFileParser::_commentPattern = regex("( |\t)*([^\\\\]#).*");
// Static intialization -


ConfigurationFileParser::ConfigurationFileParser(const string& filePath):
  _filePath(filePath) {
  // Nothing to do
}

ConfigurationFileParser::~ConfigurationFileParser() {
  // Nothing to do
}

void
ConfigurationFileParser::getConfiguration(map<string, Section>& config)
  const throw(ResourceFileException) {
  // Open file +
  ifstream file(_filePath);
  if(!file.is_open()) {
    throw ResourceFileException(_filePath, "File is not readable");
  }
  // Open file -

  // Parse file +
  string section;
  do {
    string out;
    getline(file, out);
    parseLine(out, section, config);

  } while(!file.eof());
  // Parse file -

  file.close();
}

void
ConfigurationFileParser::parseLine(const string& line,
				   string& currentSection,
				   map<string,Section>& config) const {
  smatch match;
  if(regex_search(line, match, _sectionPattern)) {
    currentSection = match[1];

  } else if(regex_search(line, match, _optionPattern)) {
    const string& option = match[1];
    string value = match[4];
    if(regex_search(value, match, _commentPattern)) {
      value = match.prefix();
    }
    auto iter = config.find(currentSection);
    if(iter == config.end()) {
      iter = config.insert(pair<string,Section>(currentSection, Section())).first;
    }
    iter->second.addParameter(option, value);
  }
}
