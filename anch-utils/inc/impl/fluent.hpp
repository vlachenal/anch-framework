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

  template<typename T, template<typename> typename C>
  Fluent<T,C>::Fluent(C<T>& values): _internalValues(NULL),
				     _values(values),
				     _filters(),
				     _skip(0),
				     _limit(std::numeric_limits<uint64_t>::max()),
				     _index(0),
				     _treated(0),
				     _next(NULL) {
    // Nothing to do
  }

  template<typename T, template<typename> typename C>
  Fluent<T,C>::Fluent(const Fluent& other): _internalValues(NULL),
					    _values(other._values),
					    _filters(other._filters),
					    _skip(other._skip),
					    _limit(other._limit),
					    _index(0),
					    _treated(0),
					    _next(other._next) {
    // Nothing to do
  }

  template<typename T, template<typename> typename C>
  Fluent<T,C>::Fluent(Fluent&& other): _internalValues(new C<T>(other._values)),
				       _values(*_internalValues),
				       _filters(other._filters),
				       _skip(other._skip),
				       _limit(other._limit),
				       _index(0),
				       _treated(0),
				       _next(other._next) {
    // Nothing to do
  }

  template<typename T, template<typename> typename C>
  Fluent<T,C>::~Fluent() {
    if(_internalValues != NULL) {
      delete _internalValues;
    }
  }

  // Filters +
  template<typename T, template<typename> typename C>
  inline Fluent<T,C>&
  Fluent<T,C>::filter(std::function<bool(const T&)> predicate) {
    _filters.push_back(predicate);
    return *this;
  }

  template<typename T, template<typename> typename C>
  inline Fluent<T,C>&
  Fluent<T,C>::skip(uint64_t skip) {
    if(_skip == 0) {
      _skip = skip;
      _filters.push_back([this](const T&) -> bool {
	return ++this->_index > this->_skip;
      });
    }
    return *this;
  }

  template<typename T, template<typename> typename C>
  inline Fluent<T,C>&
  Fluent<T,C>::limit(uint64_t limit) {
    if(_limit == std::numeric_limits<uint64_t>::max()) {
      _limit = limit;
      _filters.push_back([this](const T&) -> bool {
	bool ok = !this->limitReached();
	++this->_treated;
	return ok;
      });
    }
    return *this;
  }
  // Filters -

    template<typename T, template<typename> typename C>
    inline Fluent<T,C>&
    Fluent<T,C>::concat(Fluent<T,C>& other) noexcept {
      if(_next == NULL) {
	_next = &other;
      } else {
	_next->concat(other);
      }
      return *this;
    }

  // All match +
  template<typename T, template<typename> typename C>
  bool
  Fluent<T,C>::allMatch(std::function<bool(const T&)> predicate) {
    _index = 0;
    _treated = 0;
    return allMatch(predicate, _filters);
  }

    template<typename T, template<typename> typename C>
    bool
    Fluent<T,C>::allMatch(std::function<bool(const T&)> predicate, const std::vector<std::function<bool(const T&)>>& filters) {
      bool match = true;
      for(auto val : _values) {
	for(auto filter : filters) {
	  if(!filter(val)) {
	    goto next;
	  }
	}
	if(!predicate(val)) {
	  match = false;
	  break;
	}
      next:
	if(limitReached()) {
	  break;
	}
      }
      if(_next != NULL && match && !limitReached()) {
	match = _next->allMatch(predicate, filters);
      }
      return match;
    }
  // All match -

  // Any match +
  template<typename T, template<typename> typename C>
  bool
  Fluent<T,C>::anyMatch(std::function<bool(const T&)> predicate) {
    _index = 0;
    _treated = 0;
    return anyMatch(predicate, _filters);
  }

    template<typename T, template<typename> typename C>
    bool
    Fluent<T,C>::anyMatch(std::function<bool(const T&)> predicate, const std::vector<std::function<bool(const T&)>>& filters) {
      bool match = false;
      for(auto val : _values) {
	for(auto filter : filters) {
	  if(!filter(val)) {
	    goto next;
	  }
	}
	if(predicate(val)) {
	  match = true;
	  break;
	}
      next:
	if(limitReached()) {
	  break;
	}
      }
      if(_next != NULL && !match && !limitReached()) {
	match = _next->anyMatch(predicate, filters);
      }
      return match;
    }
  // Any match -

  // None match +
  template<typename T, template<typename> typename C>
  bool
  Fluent<T,C>::noneMatch(std::function<bool(const T&)> predicate) {
    _index = 0;
    _treated = 0;
    return noneMatch(predicate, _filters);
  }

    template<typename T, template<typename> typename C>
    bool
    Fluent<T,C>::noneMatch(std::function<bool(const T&)> predicate, const std::vector<std::function<bool(const T&)>>& filters) {
      bool match = false;
      for(auto val : _values) {
	for(auto filter : filters) {
	  if(!filter(val)) {
	    goto next;
	  }
	}
	if(predicate(val)) {
	  match = true;
	  break;
	}
      next:
	if(limitReached()) {
	  break;
	}
      }
      if(_next != NULL && !match && !limitReached()) {
	match = _next->noneMatch(predicate, filters);
      }
      return !match;
    }
  // None match -

  // Foreach +
  template<typename T, template<typename> typename C>
  void
  Fluent<T,C>::forEach(std::function<void(T&)> action) {
    _index = 0;
    _treated = 0;
    forEach(action, _filters);
  }

    template<typename T, template<typename> typename C>
    void
    Fluent<T,C>::forEach(std::function<void(T&)> action, const std::vector<std::function<bool(const T&)>>& filters) {
      for(auto val : _values) {
	for(auto filter : filters) {
	  if(!filter(val)) {
	    goto next;
	  }
	}
	action(val);
      next:
	if(limitReached()) {
	  break;
	}
      }
      if(_next != NULL && !limitReached()) {
	_next->forEach(action, filters);
      }
    }
  // Foreach -

  template<typename T, template<typename> typename C>
  template<typename U>
  Fluent<U,std::vector>
  Fluent<T,C>::map(std::function<U(const T&)> mapper) {
    std::vector<U> values;
    forEach([&values, mapper](const T& val) {
      values.push_back(mapper(val));
    });
    anch::Fluent fluent(values);
    return std::move(fluent);
  }

    template<typename T, template<typename> typename C>
    inline bool
    Fluent<T,C>::limitReached() const {
      return _treated >= _limit;
    }

  // Collect +
  template<typename T, template<typename> typename C>
  template<typename D>
  void
  Fluent<T,C>::collect(D& result, std::function<void(D&,const T&)> collector) {
    forEach([&result, &collector](const T& val) {
      std::invoke(collector, result, val);
    });
  }

    template<typename T, template<typename> typename C>
    template<typename D>
    inline D
    Fluent<T,C>::collect(std::function<void(D&,const T&)> collector) {
      D result;
      collect(result, collector);
      return result;
    }

  template<typename T, template<typename> typename C>
  template<typename D, typename R>
  void
  Fluent<T,C>::collect(D& result, R(D::*collector)(const T&)) {
    forEach([&result, &collector](const T& val) {
      std::invoke(collector, result, val);
    });
  }

  template<typename T, template<typename> typename C>
  template<typename D, typename R>
  inline D
  Fluent<T,C>::collect(R(D::*collector)(const T&)) {
    D result;
    collect(result, collector);
    return result;
  }
  // Collect -

}
