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
#ifndef _ANCH_STREAM_H_
#define _ANCH_STREAM_H_

#include <vector>
#include <functional>

namespace anch {

  /*!
   * \brief Java-ish stream class
   *
   * Java-ish stream class
   *
   * \author Vincent Lachenal
   */
  template<typename T, template<typename> typename C>
  class Stream {
    // Attributes +
  private:
    /*! Values */
    C<T>& _values;

    /*! Filters to apply on each entry */
    std::vector<std::function<bool(const T&)>> _filters;

    /*! Skip the <skip> first elements */
    uint64_t _skip;

    /*! Stop treatment after <limit> elements */
    uint64_t _limit;
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
     * \param func the filter function
     *
     * \return the current \ref Stream
     */
    Stream& filter(std::function<bool(const T&)> func);

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
     * Apply treatment for each item in stream
     *
     * \param func the function to execute on each item
     */
    void forEach(std::function<void(T&)> func);
    // Methods -

  };

  template<typename T, template<typename> typename C>
  Stream<T,C>::Stream(C<T>& values): _values(values), _filters(), _skip(0), _limit(std::numeric_limits<uint64_t>::max()) {
    // Nothing to do
  }

  template<typename T, template<typename> typename C>
  Stream<T,C>::~Stream() {
    // Nothing to do
  }

  template<typename T, template<typename> typename C>
  inline Stream<T,C>&
  Stream<T,C>::filter(std::function<bool(const T&)> func) {
    _filters.push_back(func);
    return *this;
  }

  template<typename T, template<typename> typename C>
  inline Stream<T,C>&
  Stream<T,C>::skip(uint64_t skip) {
    _skip = skip;
    return *this;
  }

  template<typename T, template<typename> typename C>
  inline Stream<T,C>&
  Stream<T,C>::limit(uint64_t limit) {
    _limit = limit;
    return *this;
  }

  template<typename T, template<typename> typename C>
  void
  Stream<T,C>::forEach(std::function<void(T&)> func) {
    uint64_t idx = 0;
    uint64_t treated = 0;
    for(auto val : _values) {
      if(treated >= _limit) {
	break;
      }
      if(idx < _skip) {
	goto next;
      }
      // Apply filters +
      for(auto filter : _filters) {
	if(!filter(val)) {
	  goto next;
	}
      }
      // Apply filters -
      func(val);
      ++treated;
    next:
      ++idx;
    }
  }

}

#endif // _ANCH_STREAM_H_
