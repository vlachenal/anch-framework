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

#include "events/eventBus.hpp"

#include <functional>


namespace anch::events {

  /*!
   * \brief An event handler.
   *
   * It provides a simple way to inherits several times from Observer and cutomize event receiving method name.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename Event, typename Derived>
  class EventHandler: public Observer<Event> {
  private:
    /*! Callback method to call on event */
    std::function<void(Derived&,const Event&)> _callbackFunction;

    /*! \ref EventBus registration */
    anch::events::EventBus<Event>* _eventBus;

  public:
    /*!
     * \ref EventHandler constructor.\n
     * Register callback method.\n
     * Register itself in global \ref EventBus if set.
     *
     * \param callbackFunction the callback method to use on notify
     * \param useEventBus if \c true register itself in global \ref EventBus, \c false otherwise (default)
     */
    EventHandler(const std::function<void(Derived&,const Event&)>& callbackFunction, bool useEventBus = false);

    /*!
     * \ref EventHandler constructor.\n
     * Registers callback method.\n
     * Register itself in \ref EventBus.
     *
     * \param callbackFunction the callback method to use on notify
     * \param eventBus the \ref EventBus to register itself
     */
    EventHandler(const std::function<void(Derived&,const Event&)>& callbackFunction, anch::events::EventBus<Event>& eventBus);

    /*!
     * \ref EventHandler destructor.\n
     * Unregister \ref EventHandler from \ref EventBus if needed.
     */
    virtual ~EventHandler();

  public:
    /*!
     * Invokes callback method on receiving event.
     *
     * \param event the event which has been fired
     */
    virtual void notify(const Event& event) noexcept;

  };

}

#include "events/impl/eventHandler.hpp"
