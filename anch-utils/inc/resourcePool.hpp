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

#include <chrono>
#include <concepts>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <exception>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>


namespace anch {

  /*!
   * Concept used to validate resource.\n
   * Objects in resource pool must implements this following methods:
   * - \code{bool isValid() const}
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<typename T>
  concept Validable = requires(const T& object) {
    {object.isValid()} -> std::convertible_to<bool>;
  };

  /*!
   * \brief Timeout exception
   *
   * Timeout error
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class TimeoutException: public std::exception {

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
    TimeoutException(const std::chrono::milliseconds& timeout, std::string msg) noexcept: std::exception(),
      _timeout(timeout),
      _msg(std::move(msg)) {
    }
    // Constructors -

    // Accessors +
  public:
    /*!
     * Error message getter
     *
     * \return the error message
     */
    const char* what() const noexcept override {
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
   * \brief Resource pool shared state (internal usage).
   *
   * State shared between \ref ResourcePool and its borrowed \ref PoolableResource.\n
   * It is kept alive by borrowed resources: a resource can be released after its pool destruction.\n
   * Every member is protected by \c _mutex.
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<anch::Validable T>
  struct ResourcePoolState {

    // Attributes +
    /*! State mutex */
    std::mutex _mutex;

    /*! Wait condition variable (used with \c _mutex) */
    std::condition_variable _wait;

    /*! Available resources */
    std::deque<std::shared_ptr<T>> _availables;

    /*! Number of borrowed resources (including resources which are being created) */
    std::size_t _used = 0;

    /*! \c true when the pool has been destroyed */
    bool _closed = false;
    // Attributes -

    // Methods +
    /*!
     * Release a borrowed resource slot.\n
     * Valid resource is pushed back in pool (unless the pool has been destroyed) ; invalid one is destroyed.\n
     * Resources are destroyed outside the lock.
     *
     * \param res the resource (can be \c nullptr when its creation has failed)
     * \param valid \c true if the resource can be reused, \c false otherwise
     */
    void release(std::shared_ptr<T> res, bool valid) noexcept;
    // Methods -

  };

  /*!
   * \brief Generic poolable resource
   *
   * Borrowed resource which returns itself to its pool on destruction.\n
   * It can be moved but not copied: a resource is returned only once.
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<anch::Validable T>
  class PoolableResource {

    // Attributes +
  private:
    /*! Pool state (\c nullptr when moved) */
    std::shared_ptr<anch::ResourcePoolState<T>> _state;

    /*! Resource */
    std::shared_ptr<T> _ptr;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref PoolableResource constructor (should only be called by \ref ResourcePool)
     *
     * \param state the pool state
     * \param ptr the resource
     */
    PoolableResource(std::shared_ptr<anch::ResourcePoolState<T>> state, std::shared_ptr<T> ptr) noexcept;

    /*!
     * Prohibits \ref PoolableResource copy constructor
     */
    PoolableResource(const PoolableResource&) = delete;

    /*!
     * \ref PoolableResource move constructor
     *
     * \param other the resource to move
     */
    PoolableResource(PoolableResource&& other) noexcept = default;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref PoolableResource destructor\n
     * Returns the resource to its pool if it is valid, destroys it otherwise.
     */
    ~PoolableResource() noexcept;
    // Destructor -

    // Operators +
  public:
    /*!
     * Prohibits \ref PoolableResource copy assignment
     */
    PoolableResource& operator=(const PoolableResource&) = delete;

    /*!
     * Prohibits \ref PoolableResource move assignment
     */
    PoolableResource& operator=(PoolableResource&&) = delete;
    // Operators -

    // Accessors +
  public:
    /*!
     * Resource getter
     *
     * \return the resource
     */
    T& get() noexcept;
    // Accessors -

  };

  /*!
   * \brief Generic resource pool utility class.
   *
   * Maximum number of resources is mandatory.\n
   * Intialize pool size will instanciate resources for imediate use.\n
   * Resources will be instanciated with their configuration, outside the pool lock.\n
   * If no resource is available, pool will wait until timeout (default to 100ms) is reached.\n
   * Resources are automatically released through their destructor ; invalid resources are destroyed
   * and will be replaced on demand.\n
   * If you have only one action to do, you can use \code{pool.borrowResource().get().doAction()} ;
   * otherwise you should to keep \c anch::PoolableResource until all actions have been done (with \code{auto res = pool.borrowResource()}).\n
   * Borrowed resources can outlive their pool: they will be destroyed when released.\n
   * You can specifiy an \c std::shared_ptr creation function as third template parameter. By default, \c std::make_shared<T> will be used.
   * It can be usefull for polymorphism dynamic allocation.
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<anch::Validable T, typename C, std::shared_ptr<T>(*make_ptr)(const C&) = std::make_shared<T> >
  class ResourcePool {

    // Attributes +
  private:
    /*! Shared state */
    std::shared_ptr<anch::ResourcePoolState<T>> _state;

    /*! Pool maximum size */
    std::size_t _maxSize;

    /*! Resource configuration */
    C _config;

    /*! Maximum time to wait for available resource (protected by state mutex) */
    std::chrono::milliseconds _timeout;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref ResourcePool constructor\n
     * Resources which can not be created at initialization will be created on demand.
     *
     * \param config the resource configuration
     * \param maxSize the pool maximum size
     * \param initialSize the initialize size
     * \param timeout the timeout
     *
     * \throw std::invalid_argument when \c maxSize is \c 0
     */
    ResourcePool(const C& config,
		 std::size_t maxSize,
		 std::size_t initialSize = 0,
		 std::chrono::milliseconds timeout = std::chrono::milliseconds(100));

    /*!
     * Prohibits \ref ResourcePool copy constructor
     */
    ResourcePool(const ResourcePool&) = delete;

    /*!
     * Prohibits \ref ResourcePool move constructor
     */
    ResourcePool(ResourcePool&&) = delete;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref ResourcePool destructor\n
     * Destroy every available resource. Borrowed resources will be destroyed when released.
     */
    ~ResourcePool();
    // Destructor -

    // Operators +
  public:
    /*!
     * Prohibits \ref ResourcePool copy assignment
     */
    ResourcePool& operator=(const ResourcePool&) = delete;

    /*!
     * Prohibits \ref ResourcePool move assignment
     */
    ResourcePool& operator=(ResourcePool&&) = delete;
    // Operators -

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
    anch::PoolableResource<T> borrowResource();
    // Methods -

    // Accessors +
  public:
    /*!
     * Timeout setter
     *
     * \param timeout the timeout to set
     */
    void setTimeout(std::chrono::milliseconds timeout);
    // Accessors -

  };

}

#include "impl/resourcePool.hpp"
