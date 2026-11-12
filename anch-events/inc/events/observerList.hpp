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

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#include "events/event.hpp"
#include "events/observer.hpp"

namespace anch::events {

  /*!
   * \brief Thread safe observers list (internal usage).
   *
   * List shared by \ref Observable and \ref EventBus "EventBus".\n
   * Notifications are done without lock on a snapshot of the list (copy on write) so observers can add or
   * remove observers, or notify again, from their \c handle method.\n
   * Guarantees:
   * - an observer removed during a notification is not notified anymore by this notification ;
   * - when \ref remove returns, the observer will not be notified anymore by any thread and can be destroyed.
   *   \ref remove waits for notifications running in other threads. Called from a \c handle method (current
   *   thread is notifying this list), it does not wait for the current thread notifications.
   *
   * Observers are notified in their registration order.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename T>
  class ObserverList {

    // Attributes +
  private:
    /*! Protects observers list (and notifications counter increment) */
    std::mutex _mutex;

    /*! Observers list snapshot (replaced on each modification) */
    std::shared_ptr<const std::vector<anch::events::Observer<T>*>> _observers;

    /*! Number of running notifications (\ref remove waits on it) */
    std::atomic<std::size_t> _active;

    /*! Number of removals (lets running notifications detect removed observers without lock) */
    std::atomic<uint64_t> _removals;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref ObserverList constructor
     */
    ObserverList();

    /*!
     * Prohibits \ref ObserverList copy constructor
     */
    ObserverList(const ObserverList&) = delete;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref ObserverList destructor
     */
    ~ObserverList() = default;
    // Destructor -

    // Operators +
  public:
    /*!
     * Prohibits \ref ObserverList copy assignment
     */
    ObserverList& operator=(const ObserverList&) = delete;
    // Operators -

    // Methods +
  public:
    /*!
     * Register observer
     *
     * \param observer the observer to register
     *
     * \return \c true when observer has been added, \c false when it was already registered
     */
    bool add(anch::events::Observer<T>& observer);

    /*!
     * Unregister observer.\n
     * Wait for notifications running in other threads.
     *
     * \param observer the observer to unregister
     */
    void remove(anch::events::Observer<T>& observer);

    /*!
     * Notify every registered observer
     *
     * \param event the event to notify
     */
    void notify(const anch::events::Event<T>& event);

  private:
    /*!
     * Count the notifications of this list running in the current thread
     *
     * \return the number of notifications
     */
    std::size_t currentThreadNotifications() const;
    // Methods -

  };

}

#include "events/impl/observerList.hpp"
