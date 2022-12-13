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
  EventBus<Evt>::EventBus():  _eventMutex(), _queueMutex(), _thread(NULL), _events(), _observers() {
    // Nothing to do
  }

  template<typename Evt>
  EventBus<Evt>::~EventBus() {
    if(_thread != NULL) {
      _thread->join();
      delete _thread;
    }
  }

  template<typename Evt>
  bool
  EventBus<Evt>::addObserver(anch::events::Observer<Evt>& observer) noexcept {
    std::lock_guard<std::mutex> lock(_eventMutex);
    bool added = _observers.insert(&observer).second;
    return added;
  }

  template<typename Evt>
  bool
  EventBus<Evt>::AddObserver(anch::events::Observer<Evt>& observer) noexcept {
    return EventBus<Evt>::getInstance().addObserver(observer);
  }

  template<typename Evt>
  void
  EventBus<Evt>::removeObserver(anch::events::Observer<Evt>& observer) noexcept {
    std::lock_guard<std::mutex> lock(_eventMutex);
    _observers.erase(&observer);
  }

  template<typename Evt>
  void
  EventBus<Evt>::RemoveObserver(anch::events::Observer<Evt>& observer) noexcept {
    EventBus<Evt>::getInstance().removeObserver(observer);
  }

  template<typename Evt>
  void
  EventBus<Evt>::fireEvent(const Evt& event) noexcept {
    std::lock_guard<std::mutex> lock(_eventMutex);
    for(anch::events::Observer<Evt>* observer : _observers) {
      observer->notify(event);
    }
  }

  template<typename Evt>
  void
  EventBus<Evt>::FireEvent(const Evt& event) noexcept {
    EventBus<Evt>::getInstance().fireEvent(event);
  }

  template<typename Evt>
  void
  EventBus<Evt>::scheduleDeferred(const Evt& event) noexcept {
    std::lock_guard<std::mutex> lock(_queueMutex);
    bool empty = _events.empty();
    _events.push(event);
    if(empty) {
      if(_thread != NULL) {
	_thread->join();
	delete _thread;
      }
      _thread = new std::thread(&EventBus<Evt>::processEvents,this);
    }
  }

  template<typename Evt>
  void
  EventBus<Evt>::ScheduleDeferred(const Evt& event) noexcept {
    EventBus<Evt>::getInstance().scheduleDeferred(event);
  }

  template<typename Evt>
  void
  EventBus<Evt>::processEvents() noexcept {
    bool empty = false;
    do {
      _queueMutex.lock();
      Evt event = _events.front();
      _events.pop();
      empty = _events.empty();
      _queueMutex.unlock();

      fireEvent(event);

    } while(!empty);
  }

}
