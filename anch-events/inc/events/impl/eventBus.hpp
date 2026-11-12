/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

namespace anch::events {

  template<typename T>
  EventBus<T>::EventBus(): _observers(), _mutex(), _eventCV(), _events(), _stop(false), _worker() {
    // Nothing to do: the bus thread is started on first event
  }

  template<typename T>
  EventBus<T>::~EventBus() {
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _stop = true;
    }
    _eventCV.notify_all();
    if(_worker.joinable()) {
      _worker.join(); // queued events are notified before the thread stops
    }
  }

  template<typename T>
  inline bool
  EventBus<T>::addObserver(anch::events::Observer<T>& observer) noexcept {
    return _observers.add(observer);
  }

  template<typename T>
  inline bool
  EventBus<T>::AddObserver(anch::events::Observer<T>& observer) noexcept {
    return EventBus<T>::getInstance().addObserver(observer);
  }

  template<typename T>
  inline void
  EventBus<T>::removeObserver(anch::events::Observer<T>& observer) noexcept {
    _observers.remove(observer);
  }

  template<typename T>
  inline void
  EventBus<T>::RemoveObserver(anch::events::Observer<T>& observer) noexcept {
    EventBus<T>::getInstance().removeObserver(observer);
  }

  template<typename T>
  inline void
  EventBus<T>::fireEvent(const T& event, const std::map<std::string,std::string>& headers) noexcept {
    fireEvent({.headers = headers, .body = event});
  }

  template<typename T>
  void
  EventBus<T>::fireEvent(const anch::events::Event<T>& event) noexcept {
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _events.push(event);
      if(!_worker.joinable()) {
	_worker = std::jthread(&EventBus<T>::processEvents, this);
      }
    }
    _eventCV.notify_one();
  }

  template<typename T>
  inline void
  EventBus<T>::FireEvent(const T& event, const std::map<std::string,std::string>& headers) noexcept {
    EventBus<T>::getInstance().fireEvent(event, headers);
  }

  template<typename T>
  inline void
  EventBus<T>::FireEvent(const anch::events::Event<T>& event) noexcept {
    EventBus<T>::getInstance().fireEvent(event);
  }

  template<typename T>
  inline void
  EventBus<T>::release() {
    delete this;
  }

  template<typename T>
  void
  EventBus<T>::processEvents() noexcept {
    std::unique_lock<std::mutex> lock(_mutex);
    while(true) {
      _eventCV.wait(lock, [this]() { return _stop || !_events.empty(); });
      if(_events.empty()) { // stopped and every event has been notified
	return;
      }
      anch::events::Event<T> event = std::move(_events.front());
      _events.pop();
      lock.unlock();
      _observers.notify(event);
      lock.lock();
    }
  }

}
