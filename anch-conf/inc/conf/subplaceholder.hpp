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
#include <optional>

#include "events/observer.hpp"

namespace anch::conf {

  /* Declare anch::conf::ConfEvt which is the event that will be send when configuration has been resolved */
  struct ResConfEvt;

  /*!
   * \brief Placeholder items
   *
   * Placeholder items contain the resolver (empty when default value) and argument
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct SubPlaceholder: anch::events::Observer<anch::conf::ResConfEvt> {

    /*! Resolver (empty when default value) */
    std::string res;

    /*! Resolver argument */
    std::string arg;

    /*! The found value (default to empty => not found) */
    std::optional<std::string> val = std::optional<std::string>();

    /*!
     * \c SubPlaceholder default constructor
     */
    SubPlaceholder() noexcept;

    /*!
     * \c SubPlaceholder desctructor
     */
    virtual ~SubPlaceholder() noexcept;

    /*!
     * Handle \c anch::conf::ResConfEvt event
     *
     * \param event the event to manage
     */
    virtual void handle(const anch::events::Event<anch::conf::ResConfEvt>& event) noexcept;

    /*!
     * Placeholder item has been resolved
     *
     * \return \c true
     */
    bool resvoled() noexcept;

  };

}
