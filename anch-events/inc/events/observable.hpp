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

#include <mutex>
#include <set>

#include "events/observer.hpp"
#include "lessPtrCompare.hpp"


namespace anch {
  namespace events {

    /*!
     * \brief An observable implementation of the observers/observable design pattern.
     *
     * This class maintains an observer list which have to be notified on event.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    template<typename Event>
    class Observable {

    private:
      // Attributes +
      /*! Observers list */
      std::set<anch::events::Observer<Event>*, anch::LessPtrCompare<Observer<Event> > > _observers;

      /*! Mutex */
      std::mutex _mutex;
      // Attributes -

    public:
      // Constructors +
      /*!
       * \ref Observable default constructor
       */
      Observable();
      // Constructors -

      // Destructor +
      /*!
       * \ref Observable destructor
       */
      virtual ~Observable();
      // Destructor -

    public:
      // Methods +
      /*!
       * Add observer for notifications
       *
       * \param observer The observer to add
       *
       * \return \c true if observer has been added, \c false otherwise
       */
      bool addObserver(anch::events::Observer<Event>& observer);

      /*!
       * Remove observer for notifications
       *
       * \param observer The observer to remove
       */
      void removeObserver(anch::events::Observer<Event>& observer);

      /*!
       * Notify every observer that an event has been fired
       *
       * \param event the event to fire
       */
      void notifyObservers(const Event& event);
      // Methods -

    };


    // Implementation +
    template<typename Evt>
    Observable<Evt>::Observable(): _observers(), _mutex() {
      // Nothing to do
    }

    template<typename Evt>
    Observable<Evt>::~Observable() {
      // Nothing to do
    }

    template<typename Evt>
    bool
    Observable<Evt>::addObserver(anch::events::Observer<Evt>& observer) {
      _mutex.lock();
      bool added = _observers.insert(&observer).second;
      _mutex.unlock();
      return added;
    }

    template<typename Evt>
    void
    Observable<Evt>::removeObserver(anch::events::Observer<Evt>& observer) {
      _mutex.lock();
      _observers.erase(&observer);
      _mutex.unlock();
    }

    template<typename Evt>
    void
    Observable<Evt>::notifyObservers(const Evt& event) {
      _mutex.lock();
      for(anch::events::Observer<Evt>* observer : _observers) {
	observer->notify(event);
      }
      _mutex.unlock();
    }
    // Implementation -

  }
}
