/*
  This file is part of ANCH Framework.

  ANCH Framework is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ANCH Framework is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _ANCH_EVENTS_OBSERVABLE_H_
#define _ANCH_EVENTS_OBSERVABLE_H_

#include <mutex>
#include <map>

#include "events/observer.hpp"


namespace anch {
  namespace events {

    /**
     * Class observable implementation of the observers/observable design pattern
     *
     * @author Vincent Lachenal
     */
    template<typename T>
    class Observable {

    private:
      // Attributes +
      /** Observers */
      std::map<uint16_t, const Observer<T>&> _observers;

      /** Current identifier */
      uint16_t _currentId;

      /** Mutex */
      std::mutex _mutex;
      // Attributes -

    public:
      // Constructors +
      /**
       * {@link Observable} default constructor
       */
      Observable(): _observers(), _currentId(0), _mutex() {
	// Nothing to do
      };
      // Constructors -

      // Destructor +
      /**
       * {@link Observable} destructor
       */
      virtual ~Observable() {
	// Nothing to do
      };
      // Destructor -

    public:
      // Methods +
      /**
       * Add observer for notifications
       *
       * @param observer The observer to add
       *
       * @return <code>true</code> if observer has been added, <code>false</code> otherwise
       */
      bool addObserver(anch::events::Observer<T>& observer) {
	_mutex.lock();
	bool added = false;
	uint16_t obsId = observer.getIdentifier();
	obsId = ++_currentId;
	if(_observers.find(observer.getIdentifier()) == _observers.cend()) {
	  _observers.insert(std::pair<uint16_t,const anch::events::Observer<T>&>(obsId, observer));
	  added = true;
	  observer.setIdentifier(obsId);
	}
	_mutex.unlock();
	return added;
      };

      /**
       * Remove observer for notifications
       *
       * @param observer The observer to remove
       */
      void removeObserver(const anch::events::Observer<T>& observer) {
	_mutex.lock();
	_observers.erase(observer.getIdentifier());
	_mutex.unlock();
      };

      /**
       * Remove observer for notifications according to its identifier
       *
       * @param observerId The observer identifier to remove
       */
      void removeObserver(const uint16_t observerId) {
	_mutex.lock();
	_observers.erase(observerId);
	_mutex.unlock();
      };

      /**
       * Notify every observer that an event has been fired
       */
      void notifyObservers(const T& event) {
	_mutex.lock();
	for(auto iter = _observers.begin() ; iter != _observers.end() ; ++iter) {
	  iter->second.notify(event);
	}
	_mutex.unlock();
      };
      // Methods -

    };

  }
}

#endif // _ANCH_EVENTS_OBSERVABLE_H_
