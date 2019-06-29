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

#include <vector>
#include <functional>


namespace anch {

  /*!
   * \brief Java style stream class
   *
   * \ref Stream aims to lazily apply treatment on container.\n
   * Every operation is stored until final operation is call. They are applied in their insertion order.
   *
   * \author Vincent Lachenal
   */
  template<typename T, template<typename> typename C>
  class Stream {
    // Attributes +
  private:
    /*! Values */
    C<T>* _internalValues;

    /*! Values */
    C<T>& _values;

    /*! Filters to apply on each entry */
    std::vector<std::function<bool(const T&)>> _filters;

    /*! Skip the <skip> first elements */
    uint64_t _skip;

    /*! Stop treatment after <limit> elements */
    uint64_t _limit;

    /*! Current index (for skip feature) */
    uint64_t _index;

    /*! Number of treated items (for limit feature) */
    uint64_t _treated;

    /*! Next stream (use to concatenate stream) */
    Stream<T,C>* _next;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Forbids \ref Stream constructor
     */
    Stream() = delete;

    /*!
     * Build stream from container
     *
     * \param container the container
     */
    Stream(C<T>& container);

    /*!
     * \ref Stream copy constructor
     */
    Stream(const Stream& other);

    /*!
     * \ref Stream move constructor
     */
    Stream(Stream&& other);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Stream destructor
     */
    virtual ~Stream();
    // Destructor -

    // Methods +
  public:
    /*!
     * Add filter function
     *
     * \param predicate the filter function
     *
     * \return the current \ref Stream
     */
    Stream& filter(std::function<bool(const T&)> predicate);

    /*!
     * Set skip
     *
     * \param skip the elements to skip
     *
     * \return the current \ref Stream
     */
    Stream& skip(uint64_t skip);

    /*!
     * Set limit
     *
     * \param func the number maximum of elements to treat
     *
     * \return the current \ref Stream
     */
    Stream& limit(uint64_t limit);

    /*!
     * Stream lazy 'concatenation'
     *
     * \param other the stream to concatenate
     */
    Stream& concat(Stream<T,C>& other) noexcept;

    /*!
     * Check if every items in stream match a predicate
     *
     * \param predicate the function to use to check items
     */
    bool allMatch(std::function<bool(const T&)> predicate);

    /*!
     * Check if there is at least one item in stream that matches a predicate
     *
     * \param predicate the function to use to check items
     */
    bool anyMatch(std::function<bool(const T&)> predicate);

    /*!
     * Check if there is no item in stream that matches a predicate
     *
     * \param predicate the function to use to check items
     */
    bool noneMatch(std::function<bool(const T&)> predicate);

    /*!
     * Apply treatment for each item in stream
     *
     * \param action the function to execute on each item
     */
    void forEach(std::function<void(T&)> action);

    /*!
     * Create a new stream transforming current stream type into another.
     *
     * \param mapper the mapping function
     */
    template<typename U>
    Stream<U,std::vector> map(std::function<U(const T&)> mapper);

    /*!
     * Collect stream items in container
     *
     * \param container the container to fill
     * \param collector the collector function
     */
    template<typename D>
    void collect(D& container, std::function<void(D&,const T&)> collector);

    /*!
     * Collect stream items in container
     *
     * \param collector the collector function
     *
     * \return the result container
     */
    template<typename D>
    D collect(std::function<void(D&,const T&)> collector);

    /*!
     * Collect stream items in container
     *
     * \param container the container to fill
     * \param collector the collector function
     */
    template<typename D, typename R>
    void collect(D& container, R(D::*collector)(const T&));

    /*!
     * Collect stream items in container
     *
     * \param collector the collector function
     *
     * \return the result container
     */
    template<typename D, typename R>
    D collect(R(D::*collector)(const T&));

  private:
    /*!
     * Apply treatment for each item in stream
     *
     * \param action the function to execute on each item
     * \param filters the filters to apply
     */
    void forEach(std::function<void(T&)> action, const std::vector<std::function<bool(const T&)>>& filters);

    /*!
     * Check if every items in stream match a predicate
     *
     * \param predicate the function to use to check items
     * \param filters the filters to apply
     */
    bool allMatch(std::function<bool(const T&)> predicate, const std::vector<std::function<bool(const T&)>>& filters);

    /*!
     * Check if there is at least one item in stream that matches a predicate
     *
     * \param predicate the function to use to check items
     * \param filters the filters to apply
     */
    bool anyMatch(std::function<bool(const T&)> predicate, const std::vector<std::function<bool(const T&)>>& filters);

    /*!
     * Check if there is no item in stream that matches a predicate
     *
     * \param predicate the function to use to check items
     * \param filters the filters to apply
     */
    bool noneMatch(std::function<bool(const T&)> predicate, const std::vector<std::function<bool(const T&)>>& filters);

    /*!
     * Check if limit has been reached
     *
     * \return \c true if limit has been reached, \c false otherwise
     */
    bool limitReached() const;
    // Methods -

  };

  // Implementation +
  template<typename T, template<typename> typename C>
  Stream<T,C>::Stream(C<T>& values): _internalValues(NULL),
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
  Stream<T,C>::Stream(const Stream& other): _internalValues(NULL),
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
  Stream<T,C>::Stream(Stream&& other): _internalValues(new C<T>(other._values)),
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
  Stream<T,C>::~Stream() {
    if(_internalValues != NULL) {
      delete _internalValues;
    }
  }

  // Filters +
  template<typename T, template<typename> typename C>
  inline Stream<T,C>&
  Stream<T,C>::filter(std::function<bool(const T&)> predicate) {
    _filters.push_back(predicate);
    return *this;
  }

  template<typename T, template<typename> typename C>
  inline Stream<T,C>&
  Stream<T,C>::skip(uint64_t skip) {
    if(_skip == 0) {
      _skip = skip;
      _filters.push_back([this](const T&) -> bool {
			   return ++this->_index > this->_skip;
			 });
    }
    return *this;
  }

  template<typename T, template<typename> typename C>
  inline Stream<T,C>&
  Stream<T,C>::limit(uint64_t limit) {
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
  inline Stream<T,C>&
  Stream<T,C>::concat(Stream<T,C>& other) noexcept {
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
  Stream<T,C>::allMatch(std::function<bool(const T&)> predicate) {
    _index = 0;
    _treated = 0;
    return allMatch(predicate, _filters);
  }

  template<typename T, template<typename> typename C>
  bool
  Stream<T,C>::allMatch(std::function<bool(const T&)> predicate, const std::vector<std::function<bool(const T&)>>& filters) {
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
  Stream<T,C>::anyMatch(std::function<bool(const T&)> predicate) {
    _index = 0;
    _treated = 0;
    return anyMatch(predicate, _filters);
  }

  template<typename T, template<typename> typename C>
  bool
  Stream<T,C>::anyMatch(std::function<bool(const T&)> predicate, const std::vector<std::function<bool(const T&)>>& filters) {
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
  Stream<T,C>::noneMatch(std::function<bool(const T&)> predicate) {
    _index = 0;
    _treated = 0;
    return noneMatch(predicate, _filters);
  }

  template<typename T, template<typename> typename C>
  bool
  Stream<T,C>::noneMatch(std::function<bool(const T&)> predicate, const std::vector<std::function<bool(const T&)>>& filters) {
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
  Stream<T,C>::forEach(std::function<void(T&)> action) {
    _index = 0;
    _treated = 0;
    forEach(action, _filters);
  }

  template<typename T, template<typename> typename C>
  void
  Stream<T,C>::forEach(std::function<void(T&)> action, const std::vector<std::function<bool(const T&)>>& filters) {
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
  Stream<U,std::vector>
  Stream<T,C>::map(std::function<U(const T&)> mapper) {
    std::vector<U> values;
    forEach([&values, mapper](const T& val) {
	      values.push_back(mapper(val));
	    });
    return std::move(anch::Stream(values));
  }

  template<typename T, template<typename> typename C>
  inline bool
  Stream<T,C>::limitReached() const {
    return _treated >= _limit;
  }

  // Collect +
  template<typename T, template<typename> typename C>
  template<typename D>
  void
  Stream<T,C>::collect(D& result, std::function<void(D&,const T&)> collector) {
    forEach([&result, &collector](const T& val) {
	      std::invoke(collector, result, val);
  	    });
  }

  template<typename T, template<typename> typename C>
  template<typename D>
  inline D
  Stream<T,C>::collect(std::function<void(D&,const T&)> collector) {
    D result;
    collect(result, collector);
    return result;
  }

  template<typename T, template<typename> typename C>
  template<typename D, typename R>
  void
  Stream<T,C>::collect(D& result, R(D::*collector)(const T&)) {
    forEach([&result, &collector](const T& val) {
	      std::invoke(collector, result, val);
	    });
  }

  template<typename T, template<typename> typename C>
  template<typename D, typename R>
  inline D
  Stream<T,C>::collect(R(D::*collector)(const T&)) {
    D result;
    collect(result, collector);
    return result;
    }
  // Collect -
  // Implementation -

}
