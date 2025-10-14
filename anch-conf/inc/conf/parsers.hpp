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
#include <functional>
#include <string>
#include <set>

#include "conf/configuration.hpp"
#include "conf/section.hpp"

#include "singleton.hpp"


namespace anch::conf {

  /*!
   * Parser declaration
   *
   * \param path the file path to parse
   * \param section the section to fill
   *
   * \throw ... any error
   */
  using Parser = std::function<void(const std::filesystem::path& /*path*/,
				    anch::conf::Section& /*section*/)>;

  /*!
   * \brief Parsers registry
   *
   * Provides singleton class with manage parsers registry by file extension
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class Parsers: public anch::Singleton<Parsers> {
    friend anch::conf::Configuration;
    friend anch::Singleton<Parsers>;

    // Attributes +
  private:
    /*! Parsers' registry */
    std::map<std::string, Parser> _registry;

    /*! Protected parsers */
    std::set<std::string> _protected;
    // Attributes -

    // Constructors +
  private:
    /*!
     * \ref Parsers private constructor
     */
    Parsers();

  public:
    /*!
     * Forbids \ref Parsers copy constructor
     *
     * \param parsers the \ref Parsers not to copy
     */
    Parsers(const Parsers& parsers) = delete;

    /*!
     * Forbids \ref Parsers move constructor
     *
     * \param parsers the \ref Parsers not to move
     */
    Parsers(Parsers&& parsers) = delete;
    // Constructors -

    // Destructor +
  private:
    /*!
     * \ref Parsers private destructor
     */
    virtual ~Parsers();
    // Destructor -

    // Methods +
  public:
    /*!
     * Register parser
     *
     * \param parser the parser to register
     * \param extensions the extensions to associate to the parser
     *
     * \throw \c anch::conf::ConfError when extension as already been registered
     */
    Parsers& registerParser(Parser parser, const std::set<std::string>& extensions);

    /*!
     * Parser getter
     *
     * \param extension the extension
     *
     * \return the parser
     *
     * \throw anch::conf::ConfError when extension is not registered
     */
    Parser getParser(const std::string& extension) const;

    /*!
     * Parsers getter
     *
     * \return the registered parsers
     */
    const std::map<std::string, Parser>& getParsers() const;
    // Methods -

  };

}

#include "conf/impl/parsers.hpp"
