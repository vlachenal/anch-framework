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
#pragma once

#include "anch-app-export.hpp"

#include <string>
#include <set>
#include <list>
#include <optional>

#include "cli/arg.hpp"

#include "log/levels.hpp"

namespace anch::app {

  /*!
   * \brief AnCH application default arguments
   *
   * Manage AnCH application default arguments:
   *  - Logging level through \c v / \c verbose to increase log level and \c q / \c quiet to decrease log level (default to \c INFO )
   *  - Profiles with \c anch-profile
   *
   * \version 0.1
   *
   * \author Vincent Lachenal
   */
  class ANCH_APP_EXPORT Arguments {

    // Attributes +
  public:
    /*! Min/max logging level (-v and -q multiple arguments) */
    std::optional<anch::log::Level> level;

    /*! AnCH profiles (--anch-profile multiple arguments) */
    std::optional<std::set<std::string>> profiles;

  private:
    /*! Arguments list */
    std::list<anch::cli::Arg> _args;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Arguments default constructor
     */
    Arguments();

    /*!
     * Forbids \ref Arguments copy constructor
     *
     * \param args the \ref Arguments not to copy
     */
    Arguments(const Arguments& args) = delete;

    /*!
     * Forbids \ref Arguments move constructor
     *
     * \param args the \ref Arguments not to move
     */
    Arguments(Arguments&& args) = delete;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Arguments destructor
     */
    virtual ~Arguments();
    // Destructor +

    // Methods +
  public:
    /*!
     * Increase log level (arg -v | --verbose)
     */
    void verbose();

    /*!
     * Decrease log level (arg -q | --quiet)
     */
    void quiet();

    /*!
     * Add AnCH profile (arg --anch-profile)
     *
     * \param profile the profile to add
     */
    void addProfile(const std::string& profile);
    // Methods -

  };

}
