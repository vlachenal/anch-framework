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
#ifndef _ANCH_THREAD_POOL_H_
#define _ANCH_THREAD_POOL_H_

#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
#include <atomic>

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
    unsigned int _maxThreads;

    /*! Remaining threads */
    std::atomic<unsigned int> _available;

    /*! Maximum number of elements in queue */
    std::size_t _maxElems;

    /*! Thread pool internal state */
    bool _running;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref ThreadPool constructor
     *
     * \param maxThreads the maximum number of alive threads (default number of processor or 1 if not defined)
     * \param maxElems the maximum number of elements in queue
     */
    ThreadPool(unsigned int maxThreads = 0, std::size_t maxElems = 0):
      _threads(),
      _mutex(),
      _available(0),
      _maxElems(maxElems),
      _running(false) {
      if(maxThreads == 0) {
	_maxThreads = std::thread::hardware_concurrency();
	if(_maxThreads == 0) { // Check if maximum number of thread is at least 1 to execute at least one thread
	  _maxThreads = 1;
	}
      } else {
	_maxThreads = maxThreads;
      }
    }
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref ThreadPool destructor
     */
    virtual ~ThreadPool() {
      // Nothing to do
    }
    // Destructor -

    // Methods +
  public:
    /*!
     * Start executing threads in queue
     */
    void start() {
      std::lock_guard<std::mutex> lock(_mutex);
      if(!_running) {
	_available.store(_maxThreads);
	_running = true;
	// Check if there is some threads to proceed +
	while(_available.load() > 0 && !_threads.empty()) {
	  _available--;
	  std::thread exec(&ThreadPool::execute, this, _threads.front());
	  exec.detach();
	  _threads.pop();
	}
	// Check if there is some threads to proceed -
      }
    }

    /*!
     * Stop executing threads in queue
     */
    void stop() {
      std::lock_guard<std::mutex> lock(_mutex);
      if(_running) {
	_running = false;
	// Wait for alive threads
	while(_available.load() != _maxThreads) {
	  std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
      }
    }

    /*!
     * Add a new thread in queue
     *
     * \param function the function which will be call in a separated thread
     * \param args the function arguments
     */
    template<typename Func, typename... Args>
    void add(Func function, Args... args) {
      std::lock_guard<std::mutex> lock(_mutex);
      if(_running) {
	if(_available.load() > 0 && _threads.size() < _maxThreads) {
	  _available--;
	  std::thread exec(&ThreadPool::execute, this, static_cast<std::function<void()> >(std::bind(function, args...)));
	  exec.detach();
	} else {
	  _threads.push(std::bind(function, args...));
	}

      } else {
	_threads.push(std::bind(function, args...));
      }
    }

  private:
    /*!
     * Execute the thread
     *
     * \param func the function to execute in new thread
     */
    void execute(std::function<void()> func) {
      try {
	func();
	_available++;
      } catch(...) {
	_available++;
      }
      _mutex.lock();
      if(_running && _available.load() > 0 && !_threads.empty()) {
	_available--;
	std::thread exec(&ThreadPool::execute, this, _threads.front());
	exec.detach();
	_threads.pop();
      }
      _mutex.unlock();
    }
    // Methods -

  };

}

#endif // _ANCH_THREAD_POOL_H_
