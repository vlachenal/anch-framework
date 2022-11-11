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

  // PoolableResource internal usage class declaration ; you can look at implementation but you should use auto keyword
  template<typename T, typename C, std::shared_ptr<T>(*make_ptr)(const C&)>
  class PoolableResource;

  /*!
   * \brief Generic resource pool utility class.
   *
   * Maximum number of resources is mandatory.\n
   * Intialize pool size will instanciate resources for imediate use.\n
   * Resources will be instanciated with their configuration.\n
   * If no resource is available, pool will wait until timeout (default to 100ms) is reached.\n
   * Resources are automatically released through their destructor.\n
   * If you have only one action to do, you can use pool.borrowResource.get().doAction() ;
   * otherwise you should to keep reference to \c anch::PoolableResource until all actions have been done (with auto res = pool.borrowResource()).\n
   * You can specifiy an \c std::shared_ptr creation function as third template parameter. By default, \c std::make_shared<T> will be used.
   * It can be usefull for polymorphism dynamic allocation.
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<typename T, typename C, std::shared_ptr<T>(*make_ptr)(const C&) = std::make_shared<T> >
  class ResourcePool {

    friend class PoolableResource<T,C,make_ptr>;

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
		 std::chrono::milliseconds timeout = std::chrono::milliseconds(100));

    /*!
     * Prohibits \ref ResourcePool copy constructor
     */
    ResourcePool(const ResourcePool&) = delete;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref ResourcePool destructor\n
     * Destroy every resource in pool.
     */
    virtual ~ResourcePool();
    // Destructor -

    // Methods +
  public:
    /*!
     * Retrieve available resource if there is one.\n
     * Create a new resource if maximum size has not been reached.\n
     * \n
     * This method will raise \ref TimeoutException if timeout has been reached while waiting
     * available resource or any other exception thrown by the resource constructor.
     *
     * \return the poolable resource (\c anch::PoolableResource)
     */
    anch::PoolableResource<T,C,make_ptr> borrowResource();

  private:
    /*!
     * Return valid resource in pool
     *
     * \param res the resource to push back in pool
     */
    void returnResource(std::shared_ptr<T> res);

    /*!
     * Invalidate valid resource in pool.\n
     * The resource will not be reachable anymore.
     *
     * \param res the resource to invalidate
     */
    void invalidateResource(std::shared_ptr<T> res);
    // Methods -

    // Accessors +
  public:
    /*!
     * Timeout setter
     *
     * \param timeout the timeout to set
     */
    void setTimeout(const std::chrono::milliseconds timeout);
    // Accessors -

  };

}

#include "impl/resourcePool.hpp"
