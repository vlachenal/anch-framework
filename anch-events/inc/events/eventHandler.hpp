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
#ifndef _ANCH_EVENTS_EVENT_HANDLER_H_
#define _ANCH_EVENTS_EVENT_HANDLER_H_

#include "events/eventBus.hpp"


namespace anch {
  namespace events {

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
      std::function<void(Derived&,const Event&) noexcept> _callbackFunction;

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
      EventHandler(const std::function<void(Derived&,const Event&) noexcept>& callbackFunction,
		   bool useEventBus = false) {
	_callbackFunction = callbackFunction;
	if(useEventBus) {
	  anch::events::EventBus<Event>& eventBus = anch::events::EventBus<Event>::getInstance();
	  eventBus.addObserver(*this);
	  _eventBus = &eventBus;
	} else {
	  _eventBus = NULL;
	}
      }

      /*!
       * \ref EventHandler constructor.\n
       * Registers callback method.\n
       * Register itself in \ref EventBus.
       *
       * \param callbackFunction the callback method to use on notify
       * \param eventBus the \ref EventBus to register itself
       */
      EventHandler(const std::function<void(Derived&,const Event&) noexcept>& callbackFunction,
		   anch::events::EventBus<Event>& eventBus) {
	_callbackFunction = callbackFunction;
	eventBus.addObserver(*this);
	_eventBus = &eventBus;
      }

      /*!
       * \ref EventHandler destructor.\n
       * Unregister \ref EventHandler from \ref EventBus if needed.
       */
      virtual ~EventHandler() {
	if(_eventBus != NULL) {
	  _eventBus->removeObserver(*this);
	}
      }

    public:
      /*!
       * Invokes callback method on receiving event.
       *
       * \param event the event which has been fired
       */
      virtual void notify(const Event& event) noexcept {
	_callbackFunction(*static_cast<Derived*>(this), event);
      }

    };

  }
}

#endif //_ANCH_EVENTS_EVENT_HANDLER_H_
