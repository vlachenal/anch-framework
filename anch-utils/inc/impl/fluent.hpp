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

  // Constructors +
  template<typename T, template<typename> typename C>
  Fluent<T,C>::Fluent(C<T>& values): _owned(),
				     _values(&values),
				     _steps(),
				     _next(nullptr) {
    // Nothing to do
  }

  template<typename T, template<typename> typename C>
  Fluent<T,C>::Fluent(C<T>&& values): _owned(std::make_shared<C<T>>(std::move(values))),
				      _values(_owned.get()),
				      _steps(),
				      _next(nullptr) {
    // Nothing to do
  }
  // Constructors -

  // Operations +
  template<typename T, template<typename> typename C>
  inline Fluent<T,C>&
  Fluent<T,C>::filter(std::function<bool(const T&)> predicate) {
    _steps.push_back(Step{Step::Kind::FILTER, std::move(predicate), 0});
    return *this;
  }

  template<typename T, template<typename> typename C>
  inline Fluent<T,C>&
  Fluent<T,C>::skip(uint64_t skip) {
    _steps.push_back(Step{Step::Kind::SKIP, nullptr, skip});
    return *this;
  }

  template<typename T, template<typename> typename C>
  inline Fluent<T,C>&
  Fluent<T,C>::limit(uint64_t limit) {
    _steps.push_back(Step{Step::Kind::LIMIT, nullptr, limit});
    return *this;
  }

  template<typename T, template<typename> typename C>
  inline Fluent<T,C>&
  Fluent<T,C>::concat(Fluent<T,C>& other) noexcept {
    Fluent<T,C>* last = this;
    while(last->_next != nullptr) {
      last = last->_next;
    }
    last->_next = &other;
    return *this;
  }
  // Operations -

  // Traversal +
  template<typename T, template<typename> typename C>
  typename Fluent<T,C>::Verdict
  Fluent<T,C>::apply(const T& value, std::vector<uint64_t>& counters) const {
    for(std::size_t i = 0 ; i < _steps.size() ; ++i) {
      const Step& step = _steps[i];
      switch(step.kind) {
      case Step::Kind::FILTER:
	if(!step.predicate(value)) {
	  return Verdict::REJECT;
	}
	break;
      case Step::Kind::SKIP:
	if(counters[i] < step.count) {
	  ++counters[i];
	  return Verdict::REJECT;
	}
	break;
      case Step::Kind::LIMIT:
	if(counters[i] >= step.count) {
	  return Verdict::STOP; // no other element can pass this step
	}
	++counters[i];
	break;
      }
    }
    return Verdict::ACCEPT;
  }

  template<typename T, template<typename> typename C>
  template<typename F>
  void
  Fluent<T,C>::traverse(F&& consumer) {
    std::vector<uint64_t> counters(_steps.size(), 0);
    for(Fluent<T,C>* current = this ; current != nullptr ; current = current->_next) {
      for(auto& value : *current->_values) {
	switch(apply(value, counters)) {
	case Verdict::REJECT:
	  break;
	case Verdict::STOP:
	  return;
	case Verdict::ACCEPT:
	  if(!consumer(value)) {
	    return;
	  }
	  break;
	}
      }
    }
  }
  // Traversal -

  // Final operations +
  template<typename T, template<typename> typename C>
  bool
  Fluent<T,C>::allMatch(std::function<bool(const T&)> predicate) {
    bool match = true;
    traverse([&match, &predicate](const T& value) {
      match = predicate(value);
      return match;
    });
    return match;
  }

  template<typename T, template<typename> typename C>
  bool
  Fluent<T,C>::anyMatch(std::function<bool(const T&)> predicate) {
    bool match = false;
    traverse([&match, &predicate](const T& value) {
      match = predicate(value);
      return !match;
    });
    return match;
  }

  template<typename T, template<typename> typename C>
  inline bool
  Fluent<T,C>::noneMatch(std::function<bool(const T&)> predicate) {
    return !anyMatch(std::move(predicate));
  }

  template<typename T, template<typename> typename C>
  void
  Fluent<T,C>::forEach(std::function<void(T&)> action) {
    traverse([&action](T& value) {
      action(value);
      return true;
    });
  }

  template<typename T, template<typename> typename C>
  template<typename U>
  Fluent<U,std::vector>
  Fluent<T,C>::map(std::function<U(const T&)> mapper) {
    std::vector<U> values;
    traverse([&values, &mapper](const T& value) {
      values.push_back(mapper(value));
      return true;
    });
    return Fluent<U,std::vector>(std::move(values));
  }

  template<typename T, template<typename> typename C>
  template<typename D>
  void
  Fluent<T,C>::collect(D& result, std::function<void(D&,const T&)> collector) {
    traverse([&result, &collector](const T& value) {
      std::invoke(collector, result, value);
      return true;
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
    traverse([&result, collector](const T& value) {
      std::invoke(collector, result, value);
      return true;
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
  // Final operations -

}
