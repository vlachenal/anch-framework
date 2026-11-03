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

namespace anch::events::detail {

  /*! Observer lists which are notifying in the current thread (one entry per running notification) */
  inline thread_local std::vector<const void*> notifyingLists;

  /*!
   * Register a running notification in the current thread for its lifetime
   */
  class NotificationFrame {
  public:
    explicit NotificationFrame(const void* list) {
      notifyingLists.push_back(list);
    }
    ~NotificationFrame() {
      notifyingLists.pop_back();
    }
    NotificationFrame(const NotificationFrame&) = delete;
    NotificationFrame& operator=(const NotificationFrame&) = delete;
  };

}  // anch::events::detail

namespace anch::events {

  template<typename T>
  ObserverList<T>::ObserverList():
    _mutex(),
    _observers(std::make_shared<const std::vector<anch::events::Observer<T>*>>()),
    _active(0),
    _removals(0) {
    // Nothing to do
  }

  template<typename T>
  bool
  ObserverList<T>::add(anch::events::Observer<T>& observer) {
    std::lock_guard<std::mutex> lock(_mutex);
    if(std::ranges::find(*_observers, &observer) != _observers->end()) {
      return false;
    }
    auto observers = std::make_shared<std::vector<anch::events::Observer<T>*>>(*_observers);
    observers->push_back(&observer);
    _observers = std::move(observers);
    return true;
  }

  template<typename T>
  void
  ObserverList<T>::remove(anch::events::Observer<T>& observer) {
    std::unique_lock<std::mutex> lock(_mutex);
    auto iter = std::ranges::find(*_observers, &observer);
    if(iter == _observers->end()) {
      return;
    }
    auto observers = std::make_shared<std::vector<anch::events::Observer<T>*>>(*_observers);
    observers->erase(observers->begin() + (iter - _observers->begin()));
    _observers = std::move(observers);
    _removals.fetch_add(1, std::memory_order_release);
    lock.unlock();
    // Wait for notifications of other threads (they may be calling the removed observer).
    // Notifications started after the list update use the new list.
    const std::size_t own = currentThreadNotifications();
    for(std::size_t active = _active.load() ; active > own ; active = _active.load()) {
      _active.wait(active);
    }
  }

  template<typename T>
  void
  ObserverList<T>::notify(const anch::events::Event<T>& event) {
    std::shared_ptr<const std::vector<anch::events::Observer<T>*>> snapshot;
    uint64_t removals;
    {
      std::lock_guard<std::mutex> lock(_mutex);
      snapshot = _observers;
      removals = _removals.load(std::memory_order_relaxed); // consistent with snapshot
      _active.fetch_add(1); // under lock: remove sees every notification using an old list
    }
    {
      detail::NotificationFrame frame(this);
      for(anch::events::Observer<T>* observer : *snapshot) {
	if(_removals.load(std::memory_order_acquire) != removals) {
	  // An observer has been removed since snapshot: check that this one is still registered
	  std::lock_guard<std::mutex> lock(_mutex);
	  if(std::ranges::find(*_observers, observer) == _observers->end()) {
	    continue;
	  }
	}
	observer->handle(event);
      }
    }
    _active.fetch_sub(1);
    _active.notify_all();
  }

  template<typename T>
  inline std::size_t
  ObserverList<T>::currentThreadNotifications() const {
    return static_cast<std::size_t>(std::ranges::count(detail::notifyingLists, static_cast<const void*>(this)));
  }

}
