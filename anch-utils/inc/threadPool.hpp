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

#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
#include <atomic>
#include <condition_variable>

namespace anch {

  /*!
   * \brief Thread pool utility class.
   *
   * Number of thread can be parameterized in constructor. If not set, the
   * maximum number of thread will be equal to the number of processor or 1.\n
   * \n
   * Every thread which will be run by this way will be detach. They will be not joinable.\n
   * Thread can be retrieve through event mechanism or by specific implementation in thread function.
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class ThreadPool {
    // Attributes +
  private:
    /*! Thread queue */
    std::queue<std::function<void()> > _threads;

    /*! Mutex to use when operating on threads queue */
    std::mutex _mutex;

    /*! Maximum number of running threads */
    uint32_t _maxThreads;

    /*! Remaining threads */
    std::atomic<uint32_t> _available;

    /*! Thread pool internal state */
    bool _running;

    /*! Is await termination */
    std::atomic<bool> _terminating;

    /*! Await termination condition variable */
    std::condition_variable _termCV;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref ThreadPool constructor
     *
     * \param maxThreads the maximum number of alive threads (default number of processor or 1 if not defined)
     */
    ThreadPool(uint32_t maxThreads = 0);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref ThreadPool destructor
     */
    virtual ~ThreadPool();
    // Destructor -

    // Methods +
  public:
    /*!
     * Start executing threads in queue
     */
    void start();

    /*!
     * Stop executing threads in queue
     */
    void stop();

    /*!
     * Await thread pool termination
     *
     * \param duration the maximum duration to wait. If \c <1 wait indefinitly. Default \c std::chrono::seconds(-1) .
     */
    template<typename R, typename P = std::ratio<1>>
    void awaitTermination(std::chrono::duration<R,P> duration);

    /*!
     * Add a new thread in queue
     *
     * \param function the function which will be call in a separated thread
     * \param args the function arguments
     */
    template<typename Func, typename... Args>
    void add(Func function, Args... args);

  private:
    /*!
     * Launch treatment
     */
    void launchTreatment(std::function<void()> func);

    /*!
     * Do after execute
     */
    void afterExecute();

    /*!
     * Execute the thread
     *
     * \param func the function to execute in new thread
     */
    void execute(std::function<void()> func);
    // Methods -

  };

}

#include "impl/threadPool.hpp"
