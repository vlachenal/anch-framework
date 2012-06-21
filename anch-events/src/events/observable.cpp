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
#include "events/observable.hpp"

using std::map;
using std::pair;

using anch::events::Observable;
using anch::events::Observer;


// Constructors +
/**
 * {@link Observable} default constructor
 */
Observable::Observable(): _observers(), _currentId(0), _mutex() {
  // Nothing to do
}
// Constructors -

// Destructor +
/**
 * {@link Observable} destructor
 */
Observable::~Observable() {
  // Nothing to do
}
// Destructor -


// Methods +
/**
 * Add observer for notifications
 *
 * @param observer The observer to add
 *
 * @return <code>true</code> if observer has been added, <code>false</code> otherwise
 */
bool
Observable::addObserver(Observer<T>& observer) {
  _mutex.lock();
  bool added = false;
  uint16_t obsId = observer.getIdentifier();
  if(obsId == 0) {
    obsId = ++_currentId;
    if(_observers.find(observer.getIdentifier() == _observer.cend())) {
      _observers.insert(pair<uint16_t,const const Observer<T>&>(obsId,observer));
      added = true;
      observer.setIdentifier(obsId);
    }
  }
  _mutex.unlock();
  return added;
}

/**
 * Remove observer for notifications
 *
 * @param observer The observer to remove
 */
void
Observable::removeObserver(const Observer<T>& observer) {
  _mutex.lock();
  _observers.erase(observer.getIdentifier());
  _mutex.unlock();
}

/**
 * Remove observer for notifications according to its identifier
 *
 * @param observerId The observer identifier to remove
 */
void
Observable::removeObserver(const uint16_t observerId) {
  _mutex.lock();
  _observers.erase(observerId);
  _mutex.unlock();
}

/**
 * Notify every observer that an event has been fired
 */
void
Observable::notifyObservers(const T& event) {
  _mutex.lock();
  for(auto iter = _observers.begin() ; iter != _observers.end() ; ++iter) {
    iter->first.second.notify(event);
  }
  _mutex.unlock();
}
// Methods -
