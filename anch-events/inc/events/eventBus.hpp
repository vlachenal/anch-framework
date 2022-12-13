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

#include <thread>
#include <mutex>
#include <queue>
#include <set>

#include "events/observer.hpp"
#include "lessPtrCompare.hpp"
#include "singleton.hpp"


namespace anch::events {

  /*!
   * \brief A class which manage global events firing and events QoS.
   *
   * EventBus is a singleton.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename Event>
  class EventBus: public anch::Singleton<EventBus<Event>> {
    friend class anch::Singleton<EventBus<Event>>;

    // Attributes +
  private:
    /*! Mutex for firing event*/
    std::mutex _eventMutex;

    /*! Mutex dequeue events */
    std::mutex _queueMutex;

    /*! Thread instance */
    std::thread* _thread;

    /*! Waiting event queue */
    std::queue<Event> _events;

    /*! Observers list */
    std::set<anch::events::Observer<Event>*, anch::LessPtrCompare<Observer<Event>>> _observers;
    // Attributes -


    // Constructors +
  private:
    /*!
     * \ref EventBus default constructor
     */
    EventBus();
    // Constructors -

    // Destructor +
    /*!
     * \ref EventBus destructor
     */
    virtual ~EventBus();
    // Destructor -


    // Methods +
  public:
    /*!
     * Register \ref Observer to \ref EventBus
     *
     * \param observer the \ref Observer to register
     */
    bool addObserver(anch::events::Observer<Event>& observer) noexcept;

    /*!
     * Retrieve \ref EventBus instance and register \ref Observer to \ref EventBus
     *
     * \param observer the \ref Observer to register
     */
    static bool AddObserver(anch::events::Observer<Event>& observer) noexcept;

    /*!
     * Remove observer for notifications
     *
     * \param observer The observer to remove
     */
    void removeObserver(anch::events::Observer<Event>& observer) noexcept;

    /*!
     * Retrieve \ref EventBus instance and remove observer for notifications
     *
     * \param observer The observer to remove
     */
    static void RemoveObserver(anch::events::Observer<Event>& observer) noexcept;

    /*!
     * Notify all observer that an event has been fired
     *
     * \param event the event which has been fired
     */
    void fireEvent(const Event& event) noexcept;

    /*!
     * Retrieve \ref EventBus instance and notify all observer that an event has been fired
     *
     * \param event the event which has been fired
     */
    static void FireEvent(const Event& event) noexcept;

    /*!
     * Put event in sheduler.
     *
     * \param event the event to process
     */
    void scheduleDeferred(const Event& event) noexcept;

    /*!
     * Retrieve \ref EventBus instance and put event in sheduler.
     *
     * \param event the event to process
     */
    static void ScheduleDeferred(const Event& event) noexcept;

  private:
    /*!
     * Process events in queue
     */
    void processEvents() noexcept;
    // Methods -

  };

}

#include "events/impl/eventBus.hpp"
