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
#include <sstream>

#include "ini/section.hpp"

using anch::ini::Section;
using anch::ini::ParserError;


// Static intialization +
const std::regex SECTION_REGEX = std::regex("^( |\t)*\\[([^#; \t]+)\\]");
const std::regex KEYVAL_REGEX = std::regex("^[#; \t]*([^=]+)( |\t)*=( |\t)*(.+)");
//const std::regex COMMENT_REGEX = std::regex("( |\t)*([^\\\\][#|;].*)$");
// Static intialization -

/*!
 * Parse value
 *
 * \param value the value
 * \param section the current section
 */
std::string
parseValue(const std::string& value, std::istream& input) {
  std::ostringstream oss;
  std::string spaces;
  for(auto iter = value.cbegin() ; iter != value.end() ; ++iter) {
    switch(*iter) {
    case ' ':
    case '\t': // Put spaces into buffer
      spaces += *iter;
      break;
    case '#':
    case ';':
      goto end;
    case '\\': // Read next char
      ++iter;
      if(iter == value.cend()) {
	std::ostringstream oss;
	oss << value << " ends with \\";
	throw ParserError(oss.str(), ParserError::ErrorCode::PARSING_ERROR);
      }
      if(*iter == '\\') {
	for(; iter != value.end() ; ++iter) {
	  if(*iter != ' ' && *iter != '\t' ) {
	    std::ostringstream oss;
	    oss << "Multiline should not have any character but spaces after \\\\ (" << value << ')';
	    throw ParserError(oss.str(), ParserError::ErrorCode::PARSING_ERROR);
	  }
	}
	if(input.eof()) {
	  throw ParserError("End of file as been reached altough multiline is requested",
			    ParserError::ErrorCode::PARSING_ERROR);
	}
	std::string line;
	std::getline(input, line);
	return oss.str() + parseValue(line, input);
      }
      [[ fallthrough ]];
    default:
      if(!spaces.empty()) {
	oss.write(spaces.data(), static_cast<std::streamsize>(spaces.size()));
	spaces.clear();
      }
      oss.put(*iter);
    }
  }
 end:
  return oss.str();
}

/*!
 * Parse key to store value.\n
 * Key can contain sections delmited by '.'.
 *
 * \param key the key
 * \param value the value
 * \param section the current section
 * \param input the input stream
 */
void
parseValue(const std::string& key, const std::string& value, Section& section, std::istream& input) {
  auto pos = key.find('.');
  if(pos != key.npos) {
    parseValue(key.substr(pos + 1), value, section.section(key.substr(0, pos)), input);
  } else {
    section.putValue(key, parseValue(value, input));
  }
}

/*!
 * Parse section
 *
 * \param name the section name
 * \param section the current section
 *
 * \return the found section
 */
Section&
parseSection(const std::string& name, Section& section) {
  auto pos = name.find('.');
  if(pos != name.npos) {
    return parseSection(name.substr(pos + 1), section.section(name.substr(0, pos)));
  }
  return section.section(name);
}

/*!
 * Parse line
 *
 * \param line the read line as \c std::string
 * \param section the current \ref Section to fill
 * \param section the root \ref Section
 * \param input the input stream
 */
Section*
parseLine(const std::string& line, Section& section, Section& root, std::istream& input) {
  std::smatch match;
  Section* res = &section;
  if(std::regex_search(line, match, SECTION_REGEX)) {
    res = &parseSection(match[2], root);

  } else if(std::regex_search(line, match, KEYVAL_REGEX)) {
    parseValue(match[1], match[4], section, input);
  }
  return res;
}

void
anch::ini::merge(const std::filesystem::path& path, anch::ini::Section& section) {
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
  Section* current = &section;
  std::string out;
  do {
    std::getline(file, out);
    current = parseLine(out, *current, section, file);
    out.clear();

  } while(!file.eof());
  // Parse file -

  file.close();
}
