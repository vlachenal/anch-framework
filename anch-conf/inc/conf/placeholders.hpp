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

#include <string>
#include <map>

#include "conf/section.hpp"
#include "conf/confError.hpp"
#include "conf/subplaceholder.hpp"

#include "events/observable.hpp"

namespace anch::conf {

  struct ConfPlaceholders;

  /*!
   * \brief Register placeholders
   *
   * This class aims to collect and resvol placeholders
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class Placeholders: anch::events::Observable<anch::conf::ResConfEvt> {

    // Attributes +
  private:
    /*! The \ref anch::conf::Section to parse */
    anch::conf::Section& _section;

    /*! The collected placeholders to replace */
    std::map<std::string, anch::conf::ConfPlaceholders> _values;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Forbids \ref Placeholders default constructor
     */
    Placeholders() = delete;

    /*!
     * Forbids \ref Placeholders copy constructor
     *
     * \param other \ref Placeholders not to copy
     */
    Placeholders(const Placeholders& other) = delete;

    /*!
     * Forbids \ref Placeholders move constructor
     *
     * \param other \ref Placeholders not to move
     */
    Placeholders(Placeholders&& other) = delete;

    /*!
     * \ref Placeholders constructor
     *
     * \param section the section to analyze
     */
    Placeholders(anch::conf::Section& section) noexcept;
    // Constructors -

    // Destructor +
    /*!
     * \ref Placeholders destructor
     */
    virtual ~Placeholders() noexcept;
    // Destructor -

    // Methods +
  public:
    /*!
     * Collect placeholders from 'root' \ref anch::conf::Section
     *
     * \return \c true when placeholders has been detected, \c false otherwise
     *
     * \throw anch::conf::ConfError on file open or resolution error (codes: RES_NOT_FOUND)
     */
    bool collect();

    /*!
     * Resolve all placeholders
     *
     * \throw anch::conf::ConfError on file open or resolution error (codes: NOT_RES)
     */
    void resolve();

  private:
    /*!
     * Collect placeholders in \ref anch::conf::Section
     *
     * \param section the \ref anch::conf::Section
     * \param path the section's path (default to empty string for root \ref anch::conf::Section)
     *
     * \throw anch::conf::ConfError on file open or resolution error (codes: RES_NOT_FOUND)
     */
    void collect(anch::conf::Section& section, const std::string& path = "");

    /*!
     * Parse placeholders
     *
     * \param cph the configuration placeholder to fill
     * \param placeholder the placeholder to parse
     *
     * \throw anch::conf::ConfError RES_NOT_FOUND when resolver has not been registered
     */
    void parsePlaceholder(anch::conf::ConfPlaceholders& cph, const std::string& placeholder);
    // Methods -

  };

}

#include "impl/placeholders.hpp"
