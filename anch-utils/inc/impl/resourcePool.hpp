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

  /*!
   * \brief Generic poolable resource
   *
   * Poolable resource which ensure return and invalidate to pool
   *
   * \author Vincen Lachenal
   *
   * \since 0.1
   */
  template<typename T, typename C, std::shared_ptr<T>(*make_ptr)(const C&)>
  class PoolableResource {

    // Attributes +
  private:
    /*! Current resource pool */
    ResourcePool<T, C, make_ptr>& _pool;

    /*! Resource */
    std::shared_ptr<T> _ptr;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref PoolableResource constructor
     */
    PoolableResource(ResourcePool<T, C, make_ptr>& pool, std::shared_ptr<T> ptr): _pool(pool), _ptr(ptr) {
      // Nothing to do
    }
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref PoolableResource destructor
     */
    virtual ~PoolableResource() {
      if(_ptr.get()->isValid()) {
	_pool.returnResource(_ptr);
      } else {
	_pool.invalidateResource(_ptr);
      }
    }
    // Destructor -

    // Accessors +
  public:
    /*!
     * Smart pointer inner resource reference getter
     *
     * \return the resource
     */
    inline T& get() {
      return *(_ptr.get());
    }
    // Accessors -
  };


  template<typename T, typename C, std::shared_ptr<T>(*make_ptr)(const C&)>
  ResourcePool<T,C,make_ptr>::ResourcePool(const C& config,
					   std::size_t maxSize,
					   std::size_t initialiSize,
					   std::chrono::milliseconds timeout):
    _mutex(),
    _wait(),
    _waitex(),
    _availables(),
    _maxSize(maxSize),
    _used(0),
    _config(config),
    _timeout(timeout) {
    for(std::size_t i = 0 ; i < initialiSize && i < maxSize ; ++i) {
      try {
	std::shared_ptr<T> ptr = make_ptr(_config);
	_availables.push_back(ptr);
      } catch(...) {
	// Nothing to do => resources will be instanciated later ... or not
      }
    }
  }

  template<typename T, typename C, std::shared_ptr<T>(*make_ptr)(const C&)>
  ResourcePool<T,C,make_ptr>::~ResourcePool() {
    while(!_availables.empty()) {
      _availables.front().reset();
      _availables.pop_front();
    }
  }

  template<typename T, typename C, std::shared_ptr<T>(*make_ptr)(const C&)>
  anch::PoolableResource<T,C,make_ptr>
  ResourcePool<T,C,make_ptr>::borrowResource() {
    _mutex.lock();
    std::shared_ptr<T> ptr;
    if(_availables.empty()) {
      if(_used < _maxSize) {
	try {
	  ptr = make_ptr(_config);
	} catch(...) {
	  _mutex.unlock();
	  throw;
	}
	++_used;
	_mutex.unlock();

      } else {
	_mutex.unlock();
	std::unique_lock<std::mutex> lock(_waitex);
	if(_wait.wait_for(lock, _timeout, [this](){return !this->_availables.empty();})) {
	  _mutex.lock();
	  ptr = _availables.front();
	  _availables.pop_front();
	  ++_used;
	  _mutex.unlock();
	} else {
	  throw TimeoutException(_timeout, "Resource could not be retrieved.");
	}
      }

    } else {
      ptr = _availables.front();
      _availables.pop_front();
      ++_used;
      _mutex.unlock();
    }
    return anch::PoolableResource<T,C,make_ptr>(*this, ptr);
  }

  template<typename T, typename C, std::shared_ptr<T>(*make_ptr)(const C&)>
  void
  ResourcePool<T,C,make_ptr>::returnResource(std::shared_ptr<T> res) {
    std::lock_guard<std::mutex> lock(_mutex);
    _availables.push_back(res);
    --_used;
    _wait.notify_one();
  }

  template<typename T, typename C, std::shared_ptr<T>(*make_ptr)(const C&)>
  void
  ResourcePool<T,C,make_ptr>::invalidateResource(std::shared_ptr<T> res) {
    std::lock_guard<std::mutex> lock(_mutex);
    res.reset();
    --_used;
    try {
      _availables.push_back(make_ptr(_config));
      _wait.notify_one();
    } catch(...) {
      // Nothing to do => until resource will be instanciate other waiters will fail on timeout
    }
  }

  template<typename T, typename C, std::shared_ptr<T>(*make_ptr)(const C&)>
  inline
  void
  ResourcePool<T,C,make_ptr>::setTimeout(const std::chrono::milliseconds timeout) {
    _timeout = timeout;
  }

}
