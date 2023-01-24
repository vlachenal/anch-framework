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
  EventBus<T>::EventBus():  _eventMutex(), _queueMutex(), _thread(NULL), _events(), _observers() {
    // Nothing to do
  }

  template<typename T>
  EventBus<T>::~EventBus() {
    if(_thread != NULL) {
      _thread->join();
      delete _thread;
    }
  }

  template<typename T>
  bool
  EventBus<T>::addObserver(anch::events::Observer<T>& observer) noexcept {
    std::lock_guard<std::mutex> lock(_eventMutex);
    bool added = _observers.insert(&observer).second;
    return added;
  }

  template<typename T>
  bool
  EventBus<T>::AddObserver(anch::events::Observer<T>& observer) noexcept {
    return EventBus<T>::getInstance().addObserver(observer);
  }

  template<typename T>
  void
  EventBus<T>::removeObserver(anch::events::Observer<T>& observer) noexcept {
    std::lock_guard<std::mutex> lock(_eventMutex);
    _observers.erase(&observer);
  }

  template<typename T>
  void
  EventBus<T>::RemoveObserver(anch::events::Observer<T>& observer) noexcept {
    EventBus<T>::getInstance().removeObserver(observer);
  }

  template<typename T>
  void
  EventBus<T>::fireEvent(const T& event, const std::map<std::string,std::string>& headers) noexcept {
    std::lock_guard<std::mutex> lock(_eventMutex);
    for(anch::events::Observer<T>* observer : _observers) {
      observer->handle({.headers = headers, .body = event});
    }
  }

  template<typename T>
  void
  EventBus<T>::FireEvent(const T& event, const std::map<std::string,std::string>& headers) noexcept {
    EventBus<T>::getInstance().fireEvent(event, headers);
  }

  template<typename T>
  void
  EventBus<T>::scheduleDeferred(const T& event, const std::map<std::string,std::string>& headers) noexcept {
    std::lock_guard<std::mutex> lock(_queueMutex);
    bool empty = _events.empty();
    _events.push({.headers = headers, .body = event});
    if(empty) {
      if(_thread != NULL) {
	_thread->join();
	delete _thread;
      }
      _thread = new std::thread(&EventBus<T>::processEvents,this);
    }
  }

  template<typename T>
  void
  EventBus<T>::ScheduleDeferred(const T& event, const std::map<std::string,std::string>& headers) noexcept {
    EventBus<T>::getInstance().scheduleDeferred(event, headers);
  }

  template<typename T>
  void
  EventBus<T>::processEvents() noexcept {
    bool empty = false;
    do {
      _queueMutex.lock();
      anch::events::Event<T> event = _events.front();
      _events.pop();
      empty = _events.empty();
      _queueMutex.unlock();

      fireEvent(event.body, event.headers);

    } while(!empty);
  }

}
