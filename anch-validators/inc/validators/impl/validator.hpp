/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2023 Vincent Lachenal

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

namespace anch::val {

  template<typename RT>
  inline
  bool
  ValidatorConfig<RT>::hasConstraint() noexcept {
    return mandatory || min.has_value() || max.has_value() || regexp.has_value();
  }

  template<typename T, typename RT>
  Validator<T,RT>::Validator(const ValidatorConfig<RT>& config): _config(config) {
    // Nothing to do
  }

  template<typename T, typename RT>
  Validator<T,RT>::~Validator() {
    // Nothing to do
  }

  template<typename T, typename RT>
  inline bool
  Validator<T,RT>::validate(const T& value) {
    if(!checkMandatory(value)) {
      return false;
    }
    //!checkMandatory(value) and return false;
    if(!checkMin(value)) {
      return false;
    }
    if(!checkMax(value)) {
      return false;
    }
    if(!checkPattern(value)) {
      return false;
    }
    return true;
  }

  template<typename T, typename RT>
  inline bool
  Validator<T,RT>::checkMandatory(const T& value) {
    if(!_conifg.mandatory) {
      return true;
    }
    return checkMandatoryImpl(value);
  }

  template<typename T, typename RT>
  inline bool
  Validator<T,RT>::checkMin(const T& value) {
    if(!_conifg.min.has_value()) {
      return true;
    }
    return checkMinImpl(value);
  }

  template<typename T, typename RT>
  inline bool
  Validator<T,RT>::checkMax(const T& value) {
    if(!_conifg.max.has_value()) {
      return true;
    }
    return checkMaxImpl(value);
  }

  template<typename T, typename RT>
  inline bool
  Validator<T,RT>::checkPattern(const T& value) {
    if(!_conifg.pattern.has_value()) {
      return true;
    }
    return checkPatternImpl(value);
  }

}  // anch::val
