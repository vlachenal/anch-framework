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

#include <mutex>
#include <set>
#include <map>
#include <string>

#include "events/observer.hpp"
#include "lessPtrCompare.hpp"


namespace anch::events {

  /*!
   * \brief An observable implementation of the observers/observable design pattern.
   *
   * This class maintains an observer list which have to be notified on event.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename T>
  class Observable {

  private:
    // Attributes +
    /*! Observers list */
    std::set<anch::events::Observer<T>*, anch::LessPtrCompare<anch::events::Observer<T> > > _observers;

    /*! Mutex */
    std::mutex _mutex;
    // Attributes -

  public:
    // Constructors +
    /*!
     * \ref Observable default constructor
     */
    Observable();
    // Constructors -

    // Destructor +
    /*!
     * \ref Observable destructor
     */
    virtual ~Observable();
    // Destructor -

  public:
    // Methods +
    /*!
     * Add observer for notifications
     *
     * \param observer The observer to add
     *
     * \return \c true if observer has been added, \c false otherwise
     */
    bool addObserver(anch::events::Observer<T>& observer);

    /*!
     * Remove observer for notifications
     *
     * \param observer The observer to remove
     */
    void removeObserver(anch::events::Observer<T>& observer);

    /*!
     * Notify every observer that an event has been fired
     *
     * \param event the event to fire
     * \param headers the event's headers (default to empty)
     */
    void notifyObservers(const T& event, const std::map<std::string,std::string>& headers = {});
    // Methods -

  };

}

#include "events/impl/observable.hpp"
