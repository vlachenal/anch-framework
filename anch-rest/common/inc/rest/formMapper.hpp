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

#include <optional>
#include <string_view>
#include <vector>

#include "rest/codec.hpp"
#include "rest/form.hpp"
#include "rest/paramConverter.hpp"

namespace anch::rest {

  /*!
   * Mapping between \c T and \ref Form , used by \ref FormUrlEncoded and \ref Multipart codecs to read
   * and write \c T bodies. Specializations define:
   * - <tt>static void toForm(const T& value, anch::rest::Form& form, anch::rest::Side side)</tt>
   * - <tt>static void fromForm(T& value, const anch::rest::Form& form, anch::rest::Side side)</tt> :
   *   errors are reported with exceptions ( \c 400 on server side)
   *
   * \code{.cpp}
template<>
struct anch::rest::FormMapper<UploadPhoto> {
  static void toForm(const UploadPhoto& value, anch::rest::Form& form, anch::rest::Side) {
    anch::rest::addFormField(form, "description", value.description);
    anch::rest::addFormFile(form, "photo", value.photo);
  }
  static void fromForm(UploadPhoto& value, const anch::rest::Form& form, anch::rest::Side) {
    value.description = anch::rest::formField<std::string>(form, "description");
    value.photo = anch::rest::requiredFormFile(form, "photo");
  }
};

api.post("/pets/{id}/photo", [](uint64_t id, anch::rest::Body<UploadPhoto> upload) { ... })
  .consumes<anch::rest::Multipart>();
   * \endcode
   *
   * \tparam T the mapped type
   *
   * \since 0.2
   */
  template<typename T>
  struct FormMapper;

  /*!
   * Type with a \ref anch::rest::FormMapper "FormMapper"
   *
   * \tparam T the type
   *
   * \since 0.2
   */
  template<typename T>
  concept FormMapped = requires(const T& value, T& target, anch::rest::Form& form, const anch::rest::Form& source,
				anch::rest::Side side) {
    anch::rest::FormMapper<T>::toForm(value, form, side);
    anch::rest::FormMapper<T>::fromForm(target, source, side);
  };

  /*!
   * Add field formatted with \ref toParameterString
   *
   * \param form the form
   * \param name the field name
   * \param value the value
   *
   * \since 0.2
   */
  template<anch::rest::ParameterValue T>
  void addFormField(anch::rest::Form& form, std::string_view name, const T& value);

  /*!
   * Add field when value is set
   *
   * \param form the form
   * \param name the field name
   * \param value the value
   *
   * \since 0.2
   */
  template<anch::rest::ParameterValue T>
  void addFormField(anch::rest::Form& form, std::string_view name, const std::optional<T>& value);

  /*!
   * Add one field per value (repeated name)
   *
   * \param form the form
   * \param name the field name
   * \param values the values
   *
   * \since 0.2
   */
  template<anch::rest::ParameterValue T>
  void addFormField(anch::rest::Form& form, std::string_view name, const std::vector<T>& values);

  /*!
   * Add part (file) with name
   *
   * \param form the form
   * \param name the part name (replaces the part one)
   * \param part the part
   *
   * \since 0.2
   */
  void addFormFile(anch::rest::Form& form, std::string_view name, const anch::rest::Part& part);

  /*!
   * Add part (file) with name when it is set
   *
   * \param form the form
   * \param name the part name
   * \param part the part
   *
   * \since 0.2
   */
  void addFormFile(anch::rest::Form& form, std::string_view name, const std::optional<anch::rest::Part>& part);

  /*!
   * Add parts (files) with name
   *
   * \param form the form
   * \param name the parts name
   * \param parts the parts
   *
   * \since 0.2
   */
  void addFormFile(anch::rest::Form& form, std::string_view name, const std::vector<anch::rest::Part>& parts);

  /*!
   * Add \c application/json part serialized with \ref Json codec (constraints checked according to
   * side)
   *
   * \param form the form
   * \param name the part name
   * \param value the value
   * \param side the side which writes form
   *
   * \since 0.2
   */
  template<typename T>
  void addJsonFormField(anch::rest::Form& form, std::string_view name, const T& value, anch::rest::Side side);

  /*!
   * Get every field converted with \ref ParamConverter
   *
   * \param form the form
   * \param name the field name
   *
   * \return the values, in order
   *
   * \throw HttpException \c 400 when a value is invalid
   *
   * \since 0.2
   */
  template<anch::rest::PathParameter T>
  std::vector<T> formFields(const anch::rest::Form& form, std::string_view name);

  /*!
   * Get first part with name (file or field)
   *
   * \param form the form
   * \param name the part name
   *
   * \return the part, empty when absent
   *
   * \since 0.2
   */
  std::optional<anch::rest::Part> formFile(const anch::rest::Form& form, std::string_view name);

  /*!
   * Get first part with name (file or field)
   *
   * \param form the form
   * \param name the part name
   *
   * \return the part
   *
   * \throw HttpException \c 400 when part is absent
   *
   * \since 0.2
   */
  anch::rest::Part requiredFormFile(const anch::rest::Form& form, std::string_view name);

  /*!
   * Get every part with name
   *
   * \param form the form
   * \param name the parts name
   *
   * \return the parts, in order
   *
   * \since 0.2
   */
  std::vector<anch::rest::Part> formFiles(const anch::rest::Form& form, std::string_view name);

  /*!
   * Get first part with name deserialized with \ref Json codec (constraints checked according to
   * side)
   *
   * \param form the form
   * \param name the part name
   * \param side the side which reads form
   *
   * \return the value, empty when part is absent
   *
   * \throw anch::json::MappingError when part is not a valid JSON value
   *
   * \since 0.2
   */
  template<typename T>
  std::optional<T> jsonFormField(const anch::rest::Form& form, std::string_view name, anch::rest::Side side);

  /*!
   * Get first part with name deserialized with \ref Json codec (constraints checked according to
   * side)
   *
   * \param form the form
   * \param name the part name
   * \param side the side which reads form
   *
   * \return the value
   *
   * \throw HttpException \c 400 when part is absent
   * \throw anch::json::MappingError when part is not a valid JSON value
   *
   * \since 0.2
   */
  template<typename T>
  T requiredJsonFormField(const anch::rest::Form& form, std::string_view name, anch::rest::Side side);

  /*!
   * Check anch-json constraints of a value read from a form on server side ( \ref Json codec checks
   * request bodies, form mappers do the same with this function). Nothing is checked on client
   * side.
   *
   * \param value the value
   * \param side the side which reads value
   *
   * \throw std::invalid_argument with the first violation ( \c "/name: ..." ) on server side
   *
   * \since 0.2
   */
  template<typename T>
  void checkFormConstraints(const T& value, anch::rest::Side side);

}

#include "rest/impl/formMapper.hpp"
