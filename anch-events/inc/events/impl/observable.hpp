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

  template<typename T>
  Observable<T>::Observable(): _observers(), _mutex() {
    // Nothing to do
  }

  template<typename T>
  Observable<T>::~Observable() {
    // Nothing to do
  }

  template<typename T>
  bool
  Observable<T>::addObserver(anch::events::Observer<T>& observer) {
    _mutex.lock();
    bool added = _observers.insert(&observer).second;
    _mutex.unlock();
    return added;
  }

  template<typename T>
  void
  Observable<T>::removeObserver(anch::events::Observer<T>& observer) {
    _mutex.lock();
    _observers.erase(&observer);
    _mutex.unlock();
  }

  template<typename T>
  void
  Observable<T>::notifyObservers(const T& event, const std::map<std::string,std::string>& headers) {
    notifyObservers({.headers = headers, .body = event});
  }

  template<typename T>
  void
  Observable<T>::notifyObservers(const anch::events::Event<T>& event) {
    _mutex.lock();
    for(anch::events::Observer<T>* observer : _observers) {
      observer->handle(event);
    }
    _mutex.unlock();
  }

}
