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

#include <cstdint>

#include "events/event.hpp"


namespace anch::events {

  /*!
   * \brief An observer interface of the observers/observable design pattern.
   *
   * It has to be implemented to handle events correctly.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename T>
  class Observer {

  public:
    // Methods +
    /*!
     * Receive event notification
     *
     * \param event The event to receive
     */
    virtual void handle(const anch::events::Event<T>& event) noexcept = 0;
    // Methods -

  };

}
