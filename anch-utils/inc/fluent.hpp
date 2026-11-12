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

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace anch {

  /*!
   * \brief Java style fuent class
   *
   * \ref Fluent aims to lazily apply treatment on container.\n
   * Every operation is stored until final operation is call. They are applied in their insertion order.\n
   * \ref skip and \ref limit count elements across concatenated fluents (\ref concat) ; only the operations
   * of the first fluent are applied.\n
   * Final operations can be called several times: skip and limit counters are reset on each call.\n
   * A fluent built on a container reference does not own it: the container has to outlive the fluent.
   * A fluent built on a temporary container (or returned by \ref map) owns its values.
   *
   * \author Vincent Lachenal
   */
  template<typename T, template<typename> typename C>
  class Fluent {

    // Inner types +
  private:
    /*!
     * Lazy operation applied on each element
     */
    struct Step {
      /*! Operation kind */
      enum class Kind {
	FILTER,
	SKIP,
	LIMIT
      };

      /*! Operation kind */
      Kind kind;

      /*! Filter predicate (\c FILTER only) */
      std::function<bool(const T&)> predicate;

      /*! Number of elements to skip or to treat (\c SKIP and \c LIMIT only) */
      uint64_t count;
    };

    /*!
     * Step result for an element
     */
    enum class Verdict {
      ACCEPT,
      REJECT,
      STOP
    };
    // Inner types -

    // Attributes +
  private:
    /*! Owned values (\c nullptr when the fluent does not own its values) */
    std::shared_ptr<C<T>> _owned;

    /*! Values */
    C<T>* _values;

    /*! Operations to apply on each entry */
    std::vector<Step> _steps;

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
     * Build fluent from container. The container has to outlive the fluent.
     *
     * \param container the container
     */
    Fluent(C<T>& container);

    /*!
     * Build fluent from temporary container. The fluent owns the values.
     *
     * \param container the container
     */
    Fluent(C<T>&& container);

    /*!
     * \ref Fluent copy constructor (values are shared)
     */
    Fluent(const Fluent& other) = default;

    /*!
     * \ref Fluent move constructor
     */
    Fluent(Fluent&& other) noexcept = default;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Fluent destructor
     */
    ~Fluent() = default;
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
     * Skip elements
     *
     * \param skip the elements to skip
     *
     * \return the current \ref Fluent
     */
    Fluent& skip(uint64_t skip);

    /*!
     * Limit the number of elements
     *
     * \param limit the number maximum of elements to treat
     *
     * \return the current \ref Fluent
     */
    Fluent& limit(uint64_t limit);

    /*!
     * Fluent lazy 'concatenation'.\n
     * The concatenated fluent has to outlive this one.
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
     * Apply treatment for each item in fluent.\n
     * Items are passed by reference: the action can modify them.
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
     * Apply operations on each element of this fluent and the concatenated ones.\n
     * Skip and limit counters are local to the traversal.
     *
     * \param consumer the function called on accepted elements ; it returns \c false to stop the traversal
     */
    template<typename F>
    void traverse(F&& consumer);

    /*!
     * Apply operations on an element
     *
     * \param value the element
     * \param counters the skip and limit counters (one per operation)
     *
     * \return the element verdict
     */
    Verdict apply(const T& value, std::vector<uint64_t>& counters) const;
    // Methods -

  };

}

#include "impl/fluent.hpp"
