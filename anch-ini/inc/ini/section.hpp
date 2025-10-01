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
#include <string>
#include <optional>
#include <filesystem>


namespace anch::ini {

  /*!
   * \brief INI file section
   *
   * INI file section description
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class Section {

    // Attributes +
  private:
    /*! Sub sections */
    std::map<std::string, Section> _sections;

    /*! Values */
    std::map<std::string, std::string> _values;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Section default constructor
     */
    Section();

    /*!
     * \ref Section copy constructor
     *
     * \param section the \ref Section to copy
     */
    Section(const Section& section);

    /*!
     * \ref Section move constructor
     *
     * \param section the \ref Section to move
     */
    Section(Section&& section);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Section destructor
     */
    virtual ~Section();
    // Destructor -

    // Methods +
  public:
    // Getters +
    /*!
     * Subsections getter
     *
     * \return the sections
     */
    const std::map<std::string, Section>& getSections() const;

    /*!
     * Values getter
     *
     * \return the values
     */
    const std::map<std::string, std::string>& getValues() const;

    /*!
     * Get value from path.\n
     * Path is composed by (sub)sections delimited by '.' and key as last token.
     *
     * \param path the key's path to search
     * \param section the current section
     *
     * \return the \c std::string value when found, \c empty otherwise
     */
    static std::optional<std::string> getValue(const std::string& path, const Section& section);

    /*!
     * Get value from path.\n
     * Path is composed by (sub)sections delimited by '.' and key as last token.
     *
     * \param path the key's path to search
     *
     * \return the \c std::string value when found, \c empty otherwise
     */
    std::optional<std::string> getValue(const std::string& path) const;

    /*!
     * Get value and cast it into \c T
     *
     * \param key the value's key
     *
     * \return empty optional value when not found, the value otherwise
     *
     * \throw \ref std::bad_cast exception on cast error
     */
    template<typename T>
    std::optional<T> getValue(const std::string& key) const;

    /*!
     * Get value and cast it into \c T
     *
     * \param key the value's key
     * \param defaultValue default value when not found
     *
     * \return default value when not found, the value otherwise
     *
     * \throw \ref std::bad_cast exception on cast error
     */
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) const;
    // Getters -

    // Modifiers +
    /*!
     * Create new \ref Section or return already exists \ref Section
     *
     * \param key the section name
     *
     * \return the \ref Section
     */
    Section& section(const std::string& key);

    /*!
     * Put value referenced by key into current section.\n
     * It will override existiing value.
     *
     * \param key the key
     * \param value the value
     *
     * \return \c this
     */
    Section& putValue(const std::string& key, const std::string& value);
    // Modifiers -

    // Operators +
  public:
    /*!
     * Simple assignment operator
     *
     * \param other the \ref Section to copy
     *
     * \return \c this
     */
    Section& operator = (const Section& other);
    // Operators -
    // Methods -

  };

}

#include "ini/impl/section.hpp"
