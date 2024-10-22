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
#include <limits>
#include <cstdint>


namespace anch {

  /*!
   * \brief Java style fuent class
   *
   * \ref Fluent aims to lazily apply treatment on container.\n
   * Every operation is stored until final operation is call. They are applied in their insertion order.
   *
   * \author Vincent Lachenal
   */
  template<typename T, template<typename> typename C>
  class Fluent {
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

    /*! Next fluent (use to concatenate fluent) */
    Fluent<T,C>* _next;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Forbids \ref Fluent constructor
     */
    Fluent() = delete;

    /*!
     * Build fluent from container
     *
     * \param container the container
     */
    Fluent(C<T>& container);

    /*!
     * \ref Fluent copy constructor
     */
    Fluent(const Fluent& other);

    /*!
     * \ref Fluent move constructor
     */
    Fluent(Fluent&& other);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Fluent destructor
     */
    virtual ~Fluent();
    // Destructor -

    // Methods +
  public:
    /*!
     * Add filter function
     *
     * \param predicate the filter function
     *
     * \return the current \ref Fluent
     */
    Fluent& filter(std::function<bool(const T&)> predicate);

    /*!
     * Set skip
     *
     * \param skip the elements to skip
     *
     * \return the current \ref Fluent
     */
    Fluent& skip(uint64_t skip);

    /*!
     * Set limit
     *
     * \param limit the number maximum of elements to treat
     *
     * \return the current \ref Fluent
     */
    Fluent& limit(uint64_t limit);

    /*!
     * Fluent lazy 'concatenation'
     *
     * \param other the fluent to concatenate
     */
    Fluent& concat(Fluent<T,C>& other) noexcept;

    /*!
     * Check if every items in fluent match a predicate
     *
     * \param predicate the function to use to check items
     */
    bool allMatch(std::function<bool(const T&)> predicate);

    /*!
     * Check if there is at least one item in fluent that matches a predicate
     *
     * \param predicate the function to use to check items
     */
    bool anyMatch(std::function<bool(const T&)> predicate);

    /*!
     * Check if there is no item in fluent that matches a predicate
     *
     * \param predicate the function to use to check items
     */
    bool noneMatch(std::function<bool(const T&)> predicate);

    /*!
     * Apply treatment for each item in fluent
     *
     * \param action the function to execute on each item
     */
    void forEach(std::function<void(T&)> action);

    /*!
     * Create a new fluent transforming current fluent type into another.
     *
     * \param mapper the mapping function
     */
    template<typename U>
    Fluent<U,std::vector> map(std::function<U(const T&)> mapper);

    /*!
     * Collect fluent items in container
     *
     * \param container the container to fill
     * \param collector the collector function
     */
    template<typename D>
    void collect(D& container, std::function<void(D&,const T&)> collector);

    /*!
     * Collect fluent items in container
     *
     * \param collector the collector function
     *
     * \return the result container
     */
    template<typename D>
    D collect(std::function<void(D&,const T&)> collector);

    /*!
     * Collect fluent items in container
     *
     * \param container the container to fill
     * \param collector the collector function
     */
    template<typename D, typename R>
    void collect(D& container, R(D::*collector)(const T&));

    /*!
     * Collect fluent items in container
     *
     * \param collector the collector function
     *
     * \return the result container
     */
    template<typename D, typename R>
    D collect(R(D::*collector)(const T&));

  private:
    /*!
     * Apply treatment for each item in fluent
     *
     * \param action the function to execute on each item
     * \param filters the filters to apply
     */
    void forEach(std::function<void(T&)> action, const std::vector<std::function<bool(const T&)>>& filters);

    /*!
     * Check if every items in fluent match a predicate
     *
     * \param predicate the function to use to check items
     * \param filters the filters to apply
     */
    bool allMatch(std::function<bool(const T&)> predicate, const std::vector<std::function<bool(const T&)>>& filters);

    /*!
     * Check if there is at least one item in fluent that matches a predicate
     *
     * \param predicate the function to use to check items
     * \param filters the filters to apply
     */
    bool anyMatch(std::function<bool(const T&)> predicate, const std::vector<std::function<bool(const T&)>>& filters);

    /*!
     * Check if there is no item in fluent that matches a predicate
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

}

#include "impl/fluent.hpp"
