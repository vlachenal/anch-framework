/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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

#include <functional>
#include <utility>

namespace anch::ut {

  template<typename F>
  auto
  ThreadErrors::wrap(F&& func) {
    return [this, func = std::forward<F>(func)](auto&&... args) mutable {
      try {
	std::invoke(func, std::forward<decltype(args)>(args)...);
      } catch(...) {
	capture();
      }
    };
  }

  inline void
  ThreadErrors::capture() noexcept {
    std::lock_guard<std::mutex> lock(_mutex);
    if(!_error) {
      _error = std::current_exception();
    }
  }

  inline bool
  ThreadErrors::failed() {
    std::lock_guard<std::mutex> lock(_mutex);
    return static_cast<bool>(_error);
  }

  inline void
  ThreadErrors::rethrow() {
    std::exception_ptr error;
    {
      std::lock_guard<std::mutex> lock(_mutex);
      error = _error;
    }
    if(error) {
      std::rethrow_exception(error);
    }
  }

}  // anch::ut
