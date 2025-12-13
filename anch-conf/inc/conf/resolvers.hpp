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
#include <optional>
#include <set>

#include "conf/configuration.hpp"

#include "singleton.hpp"


namespace anch::conf {

  /*!
   * Resolver declaration
   *
   * \param name the resolver's name
   * \param args the resolver's arguments
   *
   * \return the value is found, \c empty when not found
   *
   * \throw ... any error
   */
  using Resolver = std::function<std::optional<std::string>(const std::string& /*name*/,
							    const std::optional<std::string>& /*args*/)>;

  /*!
   * \brief Value resolvers' registry
   *
   * Provides singleton class with manage value resolvers.\n
   * Values which will be try to be resolve have the following syntax: \c{${<resolver>:value}<arguments>}
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class Resolvers: public anch::Singleton<Resolvers> {
    friend anch::conf::Configuration;
    friend anch::Singleton<Resolvers>;

    // Attributes +
  private:
    /*! Resolvers' registry */
    std::map<std::string, Resolver> _registry;

    /*! Protected resolvers */
    std::set<std::string> _protected;
    // Attributes -

    // Constructors +
  private:
    /*!
     * \ref Resolvers private constructor
     */
    Resolvers();

  public:
    /*!
     * Forbids \ref Resolvers copy constructor
     *
     * \param resolvers the \ref Resolvers not to copy
     */
    Resolvers(const Resolvers& resolvers) = delete;

    /*!
     * Forbids \ref Resolvers move constructor
     *
     * \param resolvers the \ref Resolvers not to move
     */
    Resolvers(Resolvers&& resolvers) = delete;
    // Constructors -

    // Destructor +
  private:
    /*!
     * \ref Resolvers private destructor
     */
    virtual ~Resolvers();
    // Destructor -

    // Methods +
  public:
    /*!
     * Register resolver
     *
     * \param resolver the resolver to register
     * \param key the key to associate to the resolver
     *
     * \throw \c anch::conf::ConfError when extension as already been registered
     */
    Resolvers& registerResolver(Resolver resolver, const std::string& key);

    /*!
     * Resolver getter
     *
     * \param key the resolver's key
     *
     * \return the resolver
     *
     * \throw anch::conf::ConfError when extension is not registered
     */
    Resolver getResolver(const std::string& key) const;
    // Methods -

  };

}
