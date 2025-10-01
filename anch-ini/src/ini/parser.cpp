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
#include "ini/parser.hpp"

#include <regex>
#include <fstream>
#include <string_view>

#include "ini/section.hpp"

using anch::ini::Section;
using anch::ini::ParserError;


// Static intialization +
const std::regex SECTION_REGEX = std::regex("^\\[([^# \t]+)\\]");
const std::regex KEYVAL_REGEX = std::regex("^([^# \t]+)( |\t)*=( |\t)*(.+)");
const std::regex COMMENT_REGEX = std::regex("( |\t)*([^\\\\]#).*");
// Static intialization -

/*!
 * Parse key to store value.\n
 * Key can contain sections delmited by '.'.
 *
 * \param key the key
 * \param value the value
 * \param section the current section
 */
void
parseValue(const std::string& key, const std::string& value, Section& section) {
  auto pos = key.find('.');
  if(pos != key.npos) {
    parseValue(key.substr(pos + 1), value, section.section(key.substr(0, pos)));
  } else {
    section.putValue(key, value);
  }
}

/*!
 * Parse line
 *
 * \param line the read line as \c std::string
 * \param section the \ref Section to fill
 */
Section*
parseLine(const std::string& line, Section& section) {
  std::smatch match;
  Section* res = &section;
  if(std::regex_search(line, match, SECTION_REGEX)) {
    res = &section.section(match[1]);

  } else if(std::regex_search(line, match, KEYVAL_REGEX)) {
    const std::string& key = match[1];
    std::string value = match[4];
    if(std::regex_search(value, match, COMMENT_REGEX)) {
      value = match.prefix();
    }
    parseValue(key, value, section);
  }
  return res;
}

Section
anch::ini::parse(const std::filesystem::path& path) {
  if(!std::filesystem::exists(path)) {
    std::ostringstream oss;
    oss << "File " << path << " does not exists";
    throw ParserError(oss.str(), ParserError::ErrorCode::NOT_READABLE);
  }

  // Open file +
  std::ifstream file(path);
  if(!file.is_open()) {
    std::ostringstream oss;
    oss << "Can not read " << path;
    throw ParserError(oss.str(), ParserError::ErrorCode::NOT_READABLE);
  }
  // Open file -

  // Parse file +
  Section root;
  Section* current = &root;
  std::string out;
  do {
    std::getline(file, out);
    current = parseLine(out, *current);
    out.clear();

  } while(!file.eof());
  // Parse file -

  file.close();

  return root;
}
