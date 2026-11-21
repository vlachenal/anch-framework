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

#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "json/validation.hpp"

#include "codec/json.hpp"

#include "rest/httpException.hpp"

namespace anch::rest {

  template<anch::rest::ParameterValue T>
  void
  addFormField(anch::rest::Form& form, std::string_view name, const T& value) {
    form.add(name, anch::rest::toParameterString(value));
  }

  template<anch::rest::ParameterValue T>
  void
  addFormField(anch::rest::Form& form, std::string_view name, const std::optional<T>& value) {
    if(value) {
      anch::rest::addFormField(form, name, *value);
    }
  }

  template<anch::rest::ParameterValue T>
  void
  addFormField(anch::rest::Form& form, std::string_view name, const std::vector<T>& values) {
    for(const T& value : values) {
      anch::rest::addFormField(form, name, value);
    }
  }

  inline void
  addFormFile(anch::rest::Form& form, std::string_view name, const anch::rest::Part& part) {
    anch::rest::Part copy = part;
    copy.setName(name);
    form.add(std::move(copy));
  }

  inline void
  addFormFile(anch::rest::Form& form, std::string_view name, const std::optional<anch::rest::Part>& part) {
    if(part) {
      anch::rest::addFormFile(form, name, *part);
    }
  }

  inline void
  addFormFile(anch::rest::Form& form, std::string_view name, const std::vector<anch::rest::Part>& parts) {
    for(const anch::rest::Part& part : parts) {
      anch::rest::addFormFile(form, name, part);
    }
  }

  template<typename T>
  void
  addJsonFormField(anch::rest::Form& form, std::string_view name, const T& value, anch::codec::Side side) {
    std::ostringstream out;
    anch::codec::Json::serialize(value, out, side);
    anch::rest::Part part(name, out.str());
    part.setMediaType(anch::codec::MediaType("application", "json"));
    form.add(std::move(part));
  }

  template<anch::rest::PathParameter T>
  std::vector<T>
  formFields(const anch::rest::Form& form, std::string_view name) {
    std::vector<T> res;
    for(std::string_view value : form.fields(name)) {
      std::optional<T> converted = anch::rest::ParamConverter<T>::convert(value);
      if(!converted) {
	throw anch::rest::HttpException(anch::rest::Status::BAD_REQUEST,
					"Invalid form field '" + std::string(name) + "': '" + std::string(value) + "'");
      }
      res.push_back(std::move(*converted));
    }
    return res;
  }

  inline std::optional<anch::rest::Part>
  formFile(const anch::rest::Form& form, std::string_view name) {
    const anch::rest::Part* part = form.part(name);
    if(part == nullptr) {
      return std::nullopt;
    }
    return *part;
  }

  inline anch::rest::Part
  requiredFormFile(const anch::rest::Form& form, std::string_view name) {
    const anch::rest::Part* part = form.part(name);
    if(part == nullptr) {
      throw anch::rest::HttpException(anch::rest::Status::BAD_REQUEST, "Missing form part '" + std::string(name) + "'");
    }
    return *part;
  }

  inline std::vector<anch::rest::Part>
  formFiles(const anch::rest::Form& form, std::string_view name) {
    std::vector<anch::rest::Part> res;
    for(const anch::rest::Part* part : form.parts(name)) {
      res.push_back(*part);
    }
    return res;
  }

  template<typename T>
  std::optional<T>
  jsonFormField(const anch::rest::Form& form, std::string_view name, anch::codec::Side side) {
    const anch::rest::Part* part = form.part(name);
    if(part == nullptr) {
      return std::nullopt;
    }
    T value{};
    std::unique_ptr<std::istream> input = part->open();
    anch::codec::Json::deserialize(value, *input, side);
    return value;
  }

  template<typename T>
  T
  requiredJsonFormField(const anch::rest::Form& form, std::string_view name, anch::codec::Side side) {
    std::optional<T> value = anch::rest::jsonFormField<T>(form, name, side);
    if(!value) {
      throw anch::rest::HttpException(anch::rest::Status::BAD_REQUEST, "Missing form part '" + std::string(name) + "'");
    }
    return std::move(*value);
  }

  template<typename T>
  void
  checkFormConstraints(const T& value, anch::codec::Side side) {
    if(side != anch::codec::Side::SERVER) {
      return;
    }
    std::vector<anch::val::Violation> violations = anch::json::validate(value);
    if(!violations.empty()) {
      throw std::invalid_argument(violations.front().path + ": " + violations.front().message);
    }
  }

}
