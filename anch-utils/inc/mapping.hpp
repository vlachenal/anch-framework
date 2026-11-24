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

#include <concepts>
#include <functional>
#include <string>
#include <type_traits>

namespace anch {

  /*!
   * \brief Description of an object for every format (anch-json, ...)
   *
   * A type is described by specializing this class template with a \c registerObject function which registers its
   * fields on the mapper of a format ( \ref anch::ObjectMapper ). The function is a template (the mapper type is the
   * one of each format).\n
   * \n
   * The specialization has to be declared in the header of the type, just after it: every translation unit which
   * uses the type sees its description. A description which is not visible changes the behaviour of formats and
   * validation (the type is not described there) and violates the one definition rule: formats refuse undescribed
   * objects at compile time, and anch-val refuses classes which are neither described nor scalars.
   * \snippet{trimleft} anch-utils/examples/mapping.cpp mapping
   *
   * Fields are registered with their name and their member pointer, or a getter and / or a setter
   * ( \c std::function ), and constraints (anch-val: \c anch::val::required , \c anch::val::minLength(1) , ...).
   * The primary template marks types which are not described ( \ref anch::Described ).
   *
   * \tparam T the described type
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  template<typename T>
  struct Mapping {
    /*! Marker of the primary template: \c T is not described */
    using Undescribed = void;
  };

  /*!
   * \brief Type described by an \ref anch::Mapping specialization
   *
   * \tparam T the type
   *
   * \since 0.3
   */
  template<typename T>
  concept Described = !requires { typename anch::Mapping<T>::Undescribed; };

  /*!
   * \brief Mapper of a format on which \ref anch::Mapping registers the fields of \c T
   *
   * A mapper registers fields with their name and:
   * - a member pointer: <tt>registerField(name, &T::member, constraints...)</tt> ;
   * - a getter: <tt>registerField(name, std::function<P(const T&)>, constraints...)</tt> (written only) ;
   * - a setter: <tt>registerField(name, std::function<void(T&, const P&)>, constraints...)</tt> (read only) ;
   * - a getter and a setter.
   *
   * Each function returns the mapper (fluent registration). Descriptions only use these forms, common to formats: a
   * format deduces how it maps a field from its type (anch-json maps fields of \c std::optional , pointers,
   * collections and maps with the mapper of their value type). Constraints are anch-val ones ; a format which does
   * not support one of them refuses it at compile time.
   *
   * \tparam M the mapper type
   * \tparam T the object type
   *
   * \since 0.3
   */
  template<typename M, typename T>
  concept ObjectMapper = std::is_class_v<T>
    && requires(M& mapper, const std::string& name, int T::* member, std::function<int(const T&)> getter,
		std::function<void(T&, const int&)> setter) {
    { mapper.registerField(name, member) } -> std::same_as<M&>;
    { mapper.registerField(name, getter) } -> std::same_as<M&>;
    { mapper.registerField(name, setter) } -> std::same_as<M&>;
    { mapper.registerField(name, getter, setter) } -> std::same_as<M&>;
  };

  /*!
   * Register the fields of a described type on a mapper (used by formats)
   *
   * \tparam T the described type
   * \tparam M the mapper type
   *
   * \param mapper the mapper
   *
   * \since 0.3
   */
  template<anch::Described T, anch::ObjectMapper<T> M>
  void describe(M& mapper);

}

#include "impl/mapping.hpp"
