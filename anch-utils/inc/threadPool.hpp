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

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace anch {

  /*!
   * \brief Thread pool utility class.
   *
   * The pool runs a fixed number of worker threads which execute the queued tasks.
   * Number of threads can be parameterized in constructor. If not set, it will be equal to the number of
   * processors (or 1 if it can not be detected).\n
   * \n
   * Tasks are queued with \ref add and executed once the pool has been started with \ref start.
   * Exceptions thrown by tasks are ignored.\n
   * \n
   * \ref stop and \ref awaitTermination must not be called from a task (they wait for running tasks).\n
   * The destructor drops queued tasks and waits for running ones.
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class ThreadPool {

    // Attributes +
  private:
    /*! Mutex which protects the pool state */
    std::mutex _mutex;

    /*! Workers wait for tasks on this condition */
    std::condition_variable _taskCV;

    /*! \ref stop and \ref awaitTermination wait for running tasks on this condition */
    std::condition_variable _idleCV;

    /*! Queued tasks */
    std::queue<std::move_only_function<void()>> _tasks;

    /*! Number of worker threads */
    uint32_t _maxThreads;

    /*! Number of running tasks */
    uint32_t _active;

    /*! Queued tasks are executed when \c true */
    bool _running;

    /*! New tasks are rejected while awaiting termination */
    bool _terminating;

    /*! Workers exit when \c true (pool destruction) */
    bool _shutdown;

    /*! Worker threads (declared last: joined first on destruction) */
    std::vector<std::jthread> _workers;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref ThreadPool constructor
     *
     * \param maxThreads the number of worker threads (default number of processor or 1 if not defined)
     */
    ThreadPool(uint32_t maxThreads = 0);

    /*!
     * Prohibits \ref ThreadPool copy constructor
     */
    ThreadPool(const ThreadPool&) = delete;

    /*!
     * Prohibits \ref ThreadPool move constructor
     */
    ThreadPool(ThreadPool&&) = delete;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref ThreadPool destructor\n
     * Queued tasks are dropped ; running tasks are awaited.
     */
    ~ThreadPool();
    // Destructor -

    // Operators +
  public:
    /*!
     * Prohibits \ref ThreadPool copy assignment
     */
    ThreadPool& operator=(const ThreadPool&) = delete;

    /*!
     * Prohibits \ref ThreadPool move assignment
     */
    ThreadPool& operator=(ThreadPool&&) = delete;
    // Operators -

    // Methods +
  public:
    /*!
     * Start executing tasks in queue
     */
    void start();

    /*!
     * Stop executing tasks in queue.\n
     * Wait for running tasks. Queued tasks are kept and will be executed on next \ref start call.
     */
    void stop();

    /*!
     * Await thread pool termination.\n
     * Wait for queued tasks execution until \c duration is reached. Then queued tasks which have not been
     * started are dropped, running tasks are awaited (they can not be interrupted) and the pool is stopped.\n
     * If the pool is not started, queued tasks are dropped.\n
     * New tasks are rejected while awaiting termination.
     *
     * \param duration the maximum duration to wait for queued tasks. If \c <1 wait indefinitly.
     */
    template<typename R, typename P = std::ratio<1>>
    void awaitTermination(std::chrono::duration<R,P> duration);

    /*!
     * Add a new task in queue.\n
     * Function and arguments are moved into the task: they can be move-only.
     *
     * \param function the function which will be called in a worker thread
     * \param args the function arguments
     *
     * \return \c true if the task has been queued, \c false if the pool is awaiting its termination
     */
    template<typename Func, typename... Args>
    bool add(Func function, Args... args);

  private:
    /*!
     * Worker thread loop
     */
    void worker();
    // Methods -

  };

}

#include "impl/threadPool.hpp"
