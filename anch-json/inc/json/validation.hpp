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

#include <vector>

#include "json/constraints.hpp"
#include "json/factory.hpp"

namespace anch::json {

  /*!
   * Validate value: check every field constraint of the value and of its nested values (objects, collections items,
   * maps values, \c std::optional and pointers values) without serializing it.\n
   * Unlike serialization and deserialization checks, validation does not stop on the first violation: every
   * violation is returned. Constraints are checked whatever the mapping options are.\n
   * \n
   * Only fields which can be read are checked (fields registered with a member or a getter, not with a setter
   * only). Required fields are not checked: an object always has its fields ( \c std::optional and pointers can be
   * empty when they are required and nullable).
   *
   * \snippet{trimleft} anch-json/examples/fragments.cpp validate
   *
   * \tparam T the value type
   *
   * \param value the value to validate
   *
   * \return the violations (empty when value is valid)
   *
   * \since 0.2
   */
  template<typename T>
  std::vector<anch::json::Violation> validate(const T& value);

  /*!
   * Check if value is valid ( \ref anch::json::validate returns no violation)
   *
   * \tparam T the value type
   *
   * \param value the value to validate
   *
   * \return \c true when value is valid
   *
   * \since 0.2
   */
  template<typename T>
  bool isValid(const T& value);

}  // anch::json

#include "json/impl/validation.hpp"
