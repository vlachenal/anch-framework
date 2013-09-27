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
#ifndef _ANCH_EVENTS_EVENT_BUS_H_
#define _ANCH_EVENTS_EVENT_BUS_H_

#include <thread>
#include <mutex>
#include <queue>
#include <set>

#include "events/observer.hpp"
#include "lessPtrCompare.hpp"


namespace anch {
  namespace events {

    /*!
     * Manage global events firing and events QoS.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    template<typename Event>
    class EventBus {
      // Attributes +
    private:
      /*! Mutex for retrieving instance */
      static std::mutex INSTANCE_MUTEX;

      /*! \ref EventBus unique instance */
      static EventBus<Event>* _self;

      /*! Mutex for firing event*/
      std::mutex _eventMutex;

      /*! Mutex dequeue events */
      std::mutex _queueMutex;

      /*! Thread instance */
      std::thread* _thread;

      /*! Waiting event queue */
      std::queue<Event> _events;

      /*! Observers list */
      std::set<anch::events::Observer<Event>*, anch::LessPtrCompare<Observer<Event> > > _observers;
      // Attributes -


      // Constructors +
    private:
      /*!
       * \ref EventBus default constructor
       */
      EventBus(): _eventMutex(), _queueMutex(), _thread(NULL), _events(), _observers() {
      	// Nothing to do
      }
      // Constructors -

      // Destructor +
      /*!
       * \ref EventBus destructor
       */
      virtual ~EventBus() {
      	if(_thread != NULL) {
      	  _thread->join();
      	  delete _thread;
      	}
      }
      // Destructor -


      // Methods +
    public:
      /*!
       * Get \ref EventBus unique instance
       *
       * \return the \ref EventBus unique instance
       */
      static EventBus<Event>& getEventBus() {
	if(_self == NULL) {
	  INSTANCE_MUTEX.lock();
	  if(_self == NULL) {
	    _self = new EventBus<Event>();
	  }
	  INSTANCE_MUTEX.unlock();
	}
	return *_self;
      }

      /*!
       * Register \ref Observer to \ref EventBus
       *
       * \param observer the \ref Observer to register
       */
      bool addObserver(anch::events::Observer<Event>& observer) noexcept {
	std::lock_guard<std::mutex> lock(_eventMutex);
	bool added = _observers.insert(&observer).second;
	return added;
      }

      /*!
       * Retrieve \ref EventBus instance and register \ref Observer to \ref EventBus
       *
       * \param observer the \ref Observer to register
       */
      static bool AddObserver(anch::events::Observer<Event>& observer) noexcept {
	return getEventBus().addObserver(observer);
      }

      /*!
       * Remove observer for notifications
       *
       * \param observer The observer to remove
       */
      void removeObserver(anch::events::Observer<Event>& observer) noexcept {
	std::lock_guard<std::mutex> lock(_eventMutex);
	_observers.erase(&observer);
      }

      /*!
       * Retrieve \ref EventBus instance and remove observer for notifications
       *
       * \param observer The observer to remove
       */
      static void RemoveObserver(anch::events::Observer<Event>& observer) noexcept {
	getEventBus().removeObserver(observer);
      }

      /*!
       * Notify all observer that an event has been fired
       *
       * \param event the event which has been fired
       */
      void fireEvent(const Event& event) noexcept {
	std::lock_guard<std::mutex> lock(_eventMutex);
	for(anch::events::Observer<Event>* observer : _observers) {
	  observer->notify(event);
	}
      }

      /*!
       * Retrieve \ref EventBus instance and notify all observer that an event has been fired
       *
       * \param event the event which has been fired
       */
      static void FireEvent(const Event& event) noexcept {
	getEventBus().fireEvent(event);
      }

      /*!
       * Put event in sheduler.
       *
       * \param event the event to process
       */
      void scheduleDeferred(const Event& event) noexcept {
	std::lock_guard<std::mutex> lock(_queueMutex);
	bool empty = _events.empty();
	_events.push(event);
	if(empty) {
	  if(_thread != NULL) {
	    _thread->join();
	    delete _thread;
	  }
	  _thread = new std::thread(&EventBus<Event>::processEvents,this);
	}
      }

      /*!
       * Retrieve \ref EventBus instance and put event in sheduler.
       *
       * \param event the event to process
       */
      static void ScheduleDeferred(const Event& event) noexcept {
	getEventBus().scheduleDeferred(event);
      }

    private:
      /*!
       * Process events in queue
       */
      void processEvents() noexcept {
	bool empty = false;
	do {
	  _queueMutex.lock();
	  Event event = _events.front();
	  _events.pop();
	  empty = _events.empty();
	  _queueMutex.unlock();

	  fireEvent(event);

	} while(!empty);
      }
      // Methods -

    };

  }
}

/*!
 * This macro MUST be called by the program only ONCE.
 */

#define INIT_ANCH_EVENT_BUS template<typename T> std::mutex anch::events::EventBus<T>::INSTANCE_MUTEX; \
  template<typename T> anch::events::EventBus<T>* anch::events::EventBus<T>::_self = NULL;

#endif // _ANCH_EVENTS_EVENT_BUS_H_
