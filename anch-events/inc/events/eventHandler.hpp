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

#include "events/event.hpp"


namespace anch::events {

  /*!
   * \brief An event handler.
   *
   * It provides a simple way to inherits several times from Observer and cutomize event receiving method name.\n
   * \n
   * The handler is registered in the \ref EventBus by its constructor and unregistered by its destructor. These
   * constructor and destructor run while \c Derived is not built (or already destroyed): when events can be fired
   * from another thread, \c Derived destructor has to call \ref unsubscribe (and events must not be fired before
   * \c Derived construction ends).
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename T, typename Derived>
  class EventHandler: public Observer<T> {
  private:
    /*! Callback method to call on event */
    std::function<void(Derived&,const anch::events::Event<T>&)> _callbackFunction;

    /*! \ref EventBus registration */
    anch::events::EventBus<T>* _eventBus;

  public:
    /*!
     * \ref EventHandler constructor.\n
     * Register callback method.\n
     * Register itself in global \ref EventBus if set.
     *
     * \param callbackFunction the callback method to use on notify
     * \param useEventBus if \c true register itself in global \ref EventBus, \c false otherwise (default)
     */
    EventHandler(const std::function<void(Derived&,const anch::events::Event<T>&)>& callbackFunction, bool useEventBus = false);

    /*!
     * \ref EventHandler constructor.\n
     * Registers callback method.\n
     * Register itself in \ref EventBus.
     *
     * \param callbackFunction the callback method to use on notify
     * \param eventBus the \ref EventBus to register itself
     */
    EventHandler(const std::function<void(Derived&,const anch::events::Event<T>&)>& callbackFunction, anch::events::EventBus<T>& eventBus);

    /*!
     * \ref EventHandler destructor.\n
     * Unregister \ref EventHandler from \ref EventBus if needed.
     */
    virtual ~EventHandler();

  public:
    /*!
     * Unregister \ref EventHandler from \ref EventBus if needed.\n
     * When it returns, the handler will not be notified anymore by the \ref EventBus.
     */
    void unsubscribe() noexcept;

    /*!
     * Invokes callback method on receiving event.
     *
     * \param event the event which has been fired
     */
    virtual void handle(const anch::events::Event<T>& event) noexcept;

  };

}

#include "events/impl/eventHandler.hpp"
