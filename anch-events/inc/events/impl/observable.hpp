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

namespace anch::events {

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

}
