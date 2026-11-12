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

namespace anch {

  // ResourcePoolState +
  template<anch::Validable T>
  void
  ResourcePoolState<T>::release(std::shared_ptr<T> res, bool valid) noexcept {
    if(!valid) {
      res.reset();
    }
    {
      std::lock_guard<std::mutex> lock(_mutex);
      --_used;
      if(res && !_closed) {
	try {
	  _availables.push_back(std::move(res));
	} catch(...) {
	  // Resource will be destroyed outside the lock and created again on demand
	}
      }
    }
    _wait.notify_one();
  }
  // ResourcePoolState -

  // PoolableResource +
  template<anch::Validable T>
  PoolableResource<T>::PoolableResource(std::shared_ptr<anch::ResourcePoolState<T>> state, std::shared_ptr<T> ptr) noexcept:
    _state(std::move(state)),
    _ptr(std::move(ptr)) {
  }

  template<anch::Validable T>
  PoolableResource<T>::~PoolableResource() noexcept {
    if(!_state) { // moved resource
      return;
    }
    bool valid = false;
    try {
      if constexpr(anch::Releasable<T>) {
	if(_ptr) {
	  _ptr->release(); // reset resource state (end SQL transaction for example) before validation
	}
      }
      valid = _ptr && static_cast<bool>(_ptr->isValid());
    } catch(...) {
      // Resource which can not be released or validated is considered as invalid
    }
    _state->release(std::move(_ptr), valid);
  }

  template<anch::Validable T>
  inline T&
  PoolableResource<T>::get() noexcept {
    return *_ptr;
  }
  // PoolableResource -

  // ResourcePool +
  template<anch::Validable T, typename C, std::shared_ptr<T>(*make_ptr)(const C&)>
  ResourcePool<T,C,make_ptr>::ResourcePool(const C& config,
					   std::size_t maxSize,
					   std::size_t initialSize,
					   std::chrono::milliseconds timeout):
    _state(std::make_shared<anch::ResourcePoolState<T>>()),
    _maxSize(maxSize),
    _config(config),
    _timeout(timeout) {
    if(_maxSize == 0) {
      throw std::invalid_argument("Resource pool maximum size must be greater than 0");
    }
    for(std::size_t i = 0 ; i < initialSize && i < _maxSize ; ++i) {
      try {
	_state->_availables.push_back(make_ptr(_config));
      } catch(...) {
	// Resource will be created on demand
      }
    }
  }

  template<anch::Validable T, typename C, std::shared_ptr<T>(*make_ptr)(const C&)>
  ResourcePool<T,C,make_ptr>::~ResourcePool() {
    std::deque<std::shared_ptr<T>> availables;
    {
      std::lock_guard<std::mutex> lock(_state->_mutex);
      _state->_closed = true;
      availables.swap(_state->_availables);
    }
    // Available resources are destroyed here, outside the lock
  }

  template<anch::Validable T, typename C, std::shared_ptr<T>(*make_ptr)(const C&)>
  anch::PoolableResource<T>
  ResourcePool<T,C,make_ptr>::borrowResource() {
    anch::ResourcePoolState<T>& state = *_state;
    std::unique_lock<std::mutex> lock(state._mutex);
    // Available resource or free slot (when there is no available resource, _used is the number of created resources)
    if(!state._wait.wait_for(lock, _timeout, [this, &state]() {
      return !state._availables.empty() || state._used < _maxSize;
    })) {
      throw anch::TimeoutException(_timeout, "Resource could not be retrieved.");
    }
    ++state._used;
    if(!state._availables.empty()) {
      std::shared_ptr<T> ptr = std::move(state._availables.front());
      state._availables.pop_front();
      return anch::PoolableResource<T>(_state, std::move(ptr));
    }
    // Slot is reserved: create resource outside the lock
    lock.unlock();
    std::shared_ptr<T> ptr;
    try {
      ptr = make_ptr(_config);
    } catch(...) {
      state.release(nullptr, false); // free the reserved slot and wake up a waiting thread
      throw;
    }
    return anch::PoolableResource<T>(_state, std::move(ptr));
  }

  template<anch::Validable T, typename C, std::shared_ptr<T>(*make_ptr)(const C&)>
  void
  ResourcePool<T,C,make_ptr>::setTimeout(std::chrono::milliseconds timeout) {
    std::lock_guard<std::mutex> lock(_state->_mutex);
    _timeout = timeout;
  }
  // ResourcePool -

}
