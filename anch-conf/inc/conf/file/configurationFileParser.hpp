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
#pragma once

#include <map>
#include <regex>

#include "resourceFileException.hpp"
#include "resource/section.hpp"

namespace anch::resource::file {

  /*!
   * Configuration file parser\n
   * \n
   * Classic configuration file paser\n
   * Sections are declared following pattern ^\[[-a-zA-Z0-9_]+\]\n
   * Options are declared following pattern ^[-a-zA-Z0-9_]+=.*\n
   * Comments are declared following pattern ([^\]#).*\n
   * \n
   * If empty section is found, every of its options will be store with no section.\n
   * Errors in configuration file will be ignored.
   *
   * \author Vincent Lachenal
   */
  class ConfigurationFileParser {
  private:
    // Attributes +
    /*! Section regular expression */
    static const std::regex _sectionPattern;

    /*! Option regular expression */
    static const std::regex _optionPattern;

    /*! Comments regular expression */
    static const std::regex _commentPattern;

    /*! The configuration file path */
    std::string _filePath;
    // Attributes -

  public:
    // Constructors +
    /*!
     * \ref ConfigurationFileParser constructor
     *
     * \param filePath The configuration file path
     */
    ConfigurationFileParser(const std::string& filePath);
    // Constructors -

    // Destructor +
    /*!
     * \ref ConfigurationFileParser destructor
     */
    virtual ~ConfigurationFileParser();
    // Destructor -

  public:
    /*!
     * Get configuration from file
     *
     * \param config The current configuration
     */
    void getConfiguration(std::map<std::string,anch::resource::Section>& config) const;

  private:
    /*!
     * Parse a line and store information in result map if needed
     *
     * \param line The line to parse
     * \param currentSection The current section
     * \param config The result map
     */
    void parseLine(const std::string& line,
		   std::string& currentSection,
		   std::map<std::string,anch::resource::Section>& config) const;

  };

}
