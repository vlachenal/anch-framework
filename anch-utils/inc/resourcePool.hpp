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
#ifndef _ANCH_RES_POOL_H_
#define _ANCH_RES_POOL_H_

#include <memory>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <atomic>


namespace anch {

  /*!
   * \brief Timeout exception
   *
   * Timeout error
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class TimeoutException: std::exception {

    // Attributes +
  private:
    /*! Timeout */
    std::chrono::milliseconds _timeout;

    /*! Error message */
    std::string _msg;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref TimeoutException constructor
     *
     * \param timeout the timeout
     * \param msg the error message
     */
    TimeoutException(const std::chrono::milliseconds& timeout, const std::string msg) noexcept: std::exception(),
      _timeout(timeout),
      _msg(msg) {
    }
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref TimeoutException destructor
     */
    virtual ~TimeoutException() {
      // nothing to do
    }
    // Destructor -

    // Accessors +
  public:
    /*!
     * Error message getter
     *
     * \return the error message
     */
    virtual const char* what() const noexcept {
      return _msg.data();
    }

    /*!
     * Timeout getter
     *
     * \return the timeout
     */
    inline const std::chrono::milliseconds& getTimeout() const noexcept {
      return _timeout;
    }
    // Accessors -

  };

  /*!
   * \brief Generic resource pool utility class.
   *
   * Maximum number of resources is mandatory.\n
   * Intialize pool size will instanciate resources for imediate use.\n
   * Resources will be instanciated with their configuration.\n
   * If no resource is available, pool will wait until timeout (default to 100ms) is reached.\n
   * Resources are automatically released through their destructor.\n
   * If you have only one action to do, you can use pool.borrowResource.get().doAction() ;
   * otherwise you should to keep reference to \ref PoolableResource until all actions have been done (with auto res = pool.borrowResource())
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<typename T, typename C>
  class ResourcePool {

    /*!
     * \brief Generic poolable resource
     *
     * Poolable resource which ensure return and invalidate to pool
     *
     * \author Vincen Lachenal
     *
     * \since 0.1
     */
    class PoolableResource {

      // Attributes +
    private:
      /*! Current resource pool */
      ResourcePool<T, C>& _pool;

      /*! Resource */
      std::shared_ptr<T> _ptr;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref PoolableResource constructor
       */
      PoolableResource(ResourcePool<T, C>& pool, std::shared_ptr<T> ptr): _pool(pool), _ptr(ptr) {
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

    // Attributes +
  private:
    /*! Pool mutex */
    std::mutex _mutex;

    /*! Wait condition variable */
    std::condition_variable _wait;

    /*! Wait condition variable mutex */
    std::mutex _waitex;

    /*! Available resources */
    std::deque<std::shared_ptr<T> > _availables;

    /*! Pool maximum size */
    std::size_t _maxSize;

    /*! Number of used resources */
    std::atomic_size_t _used;

    /*! Resource configuration */
    C _config;

    /*! Maximum time to wait for available resource */
    std::chrono::milliseconds _timeout;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref ResourcePool constructor
     *
     * \param config the resource configuration
     * \param maxSize the pool maximum size
     * \param initialiSize the initialize size
     * \param timeout the timeout
     */
    ResourcePool(const C& config,
		 std::size_t maxSize,
		 std::size_t initialiSize = 0,
		 std::chrono::milliseconds timeout = std::chrono::milliseconds(100)):
      _mutex(),
      _wait(),
      _waitex(),
      _availables(),
      _maxSize(maxSize),
      _used(0),
      _config(config),
      _timeout(timeout) {
      for(std::size_t i = 0 ; i < initialiSize && i < maxSize ; ++i) {
	_availables.push_back(std::make_shared<T>(_config));
      }
    }
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref ResourcePool destructor\n
     * Destroy every resource in pool.
     */
    virtual ~ResourcePool() {
      while(!_availables.empty()) {
	_availables.front().reset();
	_availables.pop_front();
      }
    }
    // Destructor -

    // Methods +
  public:
    /*!
     * Retrieve available resource if there is one.\n
     * Create a new resource if maximum size has not been reached.
     *
     * \return the poolable resource (\ref PoolableResource)
     *
     * \throw TimeoutException when all resources are used for timeout milliseconds
     */
    PoolableResource borrowResource() throw(TimeoutException) {
      _mutex.lock();
      std::shared_ptr<T> ptr;
      if(_availables.empty()) {
	if(_used.load() < _maxSize) {
	  ptr = std::make_shared<T>(_config);
	  _used.fetch_add(1);
	  _mutex.unlock();

	} else {
	  _mutex.unlock();
	  std::unique_lock<std::mutex> lock(_waitex);
	  if(_wait.wait_for(lock, _timeout, [this](){return !this->_availables.empty();})) {
	    _mutex.lock();
	    ptr = _availables.front();
	    _availables.pop_front();
	    _used.fetch_add(1);
	    _mutex.unlock();
	  } else {
	    throw TimeoutException(_timeout, "Resource could not be retrieved.");
	  }
	}

      } else {
	ptr = _availables.front();
	_availables.pop_front();
	_used.fetch_add(1);
	_mutex.unlock();
      }
      return PoolableResource(*this, ptr);
    }

  private:
    /*!
     * Return valid resource in pool
     *
     * \param res the resource to push back in pool
     */
    void returnResource(std::shared_ptr<T> res) {
      std::lock_guard<std::mutex> lock(_mutex);
      _availables.push_back(res);
      _used.fetch_sub(1);
      _wait.notify_one();
    }

    /*!
     * Invalidate valid resource in pool.\n
     * The resource will not be reachable anymore.
     *
     * \param res the resource to invalidate
     */
    void invalidateResource(std::shared_ptr<T> res) {
      res.reset();
      _used.fetch_sub(1);
      _availables.push_back(std::make_shared<T>(_config));
      _wait.notify_one();
    }
    // Methods -

    // Accessors +
  public:
    /*!
     * Timeout setter
     *
     * \param timeout the timeout to set
     */
    inline void setTimeout(const std::chrono::milliseconds timeout) {
      _timeout = timeout;
    }
    // Accessors -

  };

}

#endif // _ANCH_RES_POOL_H_
