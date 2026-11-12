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

#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "events/observer.hpp"
#include "events/event.hpp"
#include "events/observerList.hpp"
#include "singleton.hpp"

namespace anch::events {

  /*!
   * \brief A class which manage global events firing and events QoS.
   *
   * Events are queued and notified asynchronously, in their firing order, by a dedicated thread (started on first
   * event). \ref fireEvent does not wait for observers: observers can fire events from their \c handle method.\n
   * Observers can add or remove observers from their \c handle method. When \ref removeObserver returns, the
   * observer will not be notified anymore and can be destroyed.\n
   * \n
   * \ref getInstance returns the global \ref EventBus. Other buses can be created as plain objects.\n
   * The destructor notifies the queued events and stops the thread: it must not be called from an observer.
   * The global bus is destroyed at program exit: its observers must not use already destroyed static objects.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename T>
  class EventBus: public anch::Singleton<EventBus<T>> {
    friend class anch::Singleton<EventBus<T>>;

    // Attributes +
  private:
    /*! Observers list */
    anch::events::ObserverList<T> _observers;

    /*! Protects events queue and stop flag */
    std::mutex _mutex;

    /*! The bus thread waits for events on this condition */
    std::condition_variable _eventCV;

    /*! Waiting event queue */
    std::queue<anch::events::Event<T>> _events;

    /*! The bus thread stops when \c true and the queue is empty */
    bool _stop;

    /*! Bus thread (declared last: joined first on destruction) */
    std::jthread _worker;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref EventBus default constructor.\n
     * This constructor is public to allow to declare other buses than the global one.
     */
    EventBus();

    /*!
     * Prohibits \ref EventBus copy constructor
     */
    EventBus(const EventBus&) = delete;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref EventBus destructor.\n
     * Notify the queued events and stop the bus thread.
     */
    ~EventBus();
    // Destructor -

    // Operators +
  public:
    /*!
     * Prohibits \ref EventBus copy assignment
     */
    EventBus& operator=(const EventBus&) = delete;
    // Operators -

    // Methods +
  public:
    /*!
     * Register \ref Observer to \ref EventBus
     *
     * \param observer the \ref Observer to register
     *
     * \return \c true when observer has been added, \c false otherwise (when observer has already been registered)
     */
    bool addObserver(anch::events::Observer<T>& observer) noexcept;

    /*!
     * Retrieve \ref EventBus instance and register \ref Observer to \ref EventBus
     *
     * \param observer the \ref Observer to register
     *
     * \return \c true when observer has been added, \c false otherwise (when observer has already been registered)
     */
    static bool AddObserver(anch::events::Observer<T>& observer) noexcept;

    /*!
     * Remove observer for notifications.\n
     * Wait for the running notification (when not called from an observer of this bus).
     *
     * \param observer the \ref Observer to remove
     */
    void removeObserver(anch::events::Observer<T>& observer) noexcept;

    /*!
     * Retrieve \ref EventBus instance and remove observer for notifications
     *
     * \param observer the \ref Observer to remove
     */
    static void RemoveObserver(anch::events::Observer<T>& observer) noexcept;

    /*!
     * Notify all observer that an event has been fired.\n
     * The event is queued: this method does not wait for observers.
     *
     * \param event the event which has been fired
     * \param headers the event's context
     */
    void fireEvent(const T& event, const std::map<std::string,std::string>& headers = {}) noexcept;

    /*!
     * Notify all observer that an event has been fired.\n
     * The event is queued: this method does not wait for observers.
     *
     * \param event the event which has been fired
     */
    void fireEvent(const anch::events::Event<T>& event) noexcept;

    /*!
     * Retrieve \ref EventBus instance and notify all observer that an event has been fired
     *
     * \param event the event which has been fired
     * \param headers the event's context
     */
    static void FireEvent(const T& event, const std::map<std::string,std::string>& headers = {}) noexcept;

    /*!
     * Retrieve \ref EventBus instance and notify all observer that an event has been fired
     *
     * \param event the event which has been fired
     */
    static void FireEvent(const anch::events::Event<T>& event) noexcept;

    /*!
     * Delete \c this .\n
     * Only for buses allocated with \c new (never for the global bus returned by \ref getInstance).
     */
    void release();

  private:
    /*!
     * Process events in queue (bus thread)
     */
    void processEvents() noexcept;
    // Methods -

  };

}

#include "events/impl/eventBus.hpp"
