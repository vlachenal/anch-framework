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

namespace anch {

  // Constructors +
  inline
  ThreadPool::ThreadPool(uint32_t maxThreads):
    _mutex(),
    _taskCV(),
    _idleCV(),
    _tasks(),
    _maxThreads(maxThreads),
    _active(0),
    _running(false),
    _terminating(false),
    _shutdown(false),
    _workers() {
    if(_maxThreads == 0) {
      _maxThreads = std::thread::hardware_concurrency();
      if(_maxThreads == 0) { // Check if maximum number of thread is at least 1 to execute at least one thread
	_maxThreads = 1;
      }
    }
    _workers.reserve(_maxThreads);
    try {
      for(uint32_t i = 0 ; i < _maxThreads ; ++i) {
	_workers.emplace_back(&ThreadPool::worker, this);
      }
    } catch(...) {
      // Stop already created workers before propagating error (they would wait forever otherwise)
      {
	std::lock_guard<std::mutex> lock(_mutex);
	_shutdown = true;
      }
      _taskCV.notify_all();
      _workers.clear();
      throw;
    }
  }
  // Constructors -

  // Destructor +
  inline
  ThreadPool::~ThreadPool() {
    std::queue<std::move_only_function<void()>> dropped;
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _shutdown = true;
      _running = false;
      dropped.swap(_tasks);
    }
    _taskCV.notify_all();
    _workers.clear(); // join workers: wait for running tasks
  }
  // Destructor -

  // Methods +
  inline
  void
  ThreadPool::start() {
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _running = true;
    }
    _taskCV.notify_all();
  }

  inline
  void
  ThreadPool::stop() {
    std::unique_lock<std::mutex> lock(_mutex);
    _running = false;
    _idleCV.wait(lock, [this] { return _active == 0; });
  }

  template<typename R, typename P>
  void
  ThreadPool::awaitTermination(std::chrono::duration<R,P> duration) {
    std::queue<std::move_only_function<void()>> dropped;
    std::unique_lock<std::mutex> lock(_mutex);
    _terminating = true;
    auto done = [this] { return _active == 0 && (_tasks.empty() || !_running); };
    if(duration.count() > 0) {
      _idleCV.wait_for(lock, duration, done);
    } else {
      _idleCV.wait(lock, done);
    }
    // Drop tasks which have not been started and wait for running ones
    dropped.swap(_tasks);
    _idleCV.wait(lock, [this] { return _active == 0; });
    _running = false;
    _terminating = false;
    lock.unlock(); // dropped tasks are destroyed outside the lock
  }

  template<typename Func, typename... Args>
  bool
  ThreadPool::add(Func function, Args... args) {
    {
      std::lock_guard<std::mutex> lock(_mutex);
      if(_terminating || _shutdown) {
	return false;
      }
      _tasks.emplace([function = std::move(function), ...args = std::move(args)]() mutable {
	std::invoke(std::move(function), std::move(args)...);
      });
    }
    _taskCV.notify_one();
    return true;
  }

  inline
  void
  ThreadPool::worker() {
    std::unique_lock<std::mutex> lock(_mutex);
    while(true) {
      _taskCV.wait(lock, [this] { return _shutdown || (_running && !_tasks.empty()); });
      if(_shutdown) {
	return;
      }
      std::move_only_function<void()> task = std::move(_tasks.front());
      _tasks.pop();
      ++_active;
      lock.unlock();
      try {
	task();
      } catch(...) {
	// Nothing to do => we don't want this pool to crash applications
      }
      task = nullptr; // destroy task (and its arguments) outside the lock
      lock.lock();
      --_active;
      _idleCV.notify_all();
    }
  }
  // Methods -

}  // anch
