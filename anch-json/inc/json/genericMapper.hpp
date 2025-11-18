/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2020 Vincent Lachenal

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

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <list>
#include <set>
#include <map>
#include <memory>

#include "json/readerContext.hpp"
#include "json/writerContext.hpp"

#include "flux.hpp"

namespace anch::json {

  /*!
   * \brief Generic mapper
   *
   * Based on C++ CRT. It provides generic functions for collections mapping.
   *
   * \param M the inherited class type (mapper)
   * \param T the type to map
   *
   * \author Vincent Lachenal
   */
  template<typename M, typename T>
  class GenericMapper {

    // Constructors +
  protected:
    /*!
     * \ref GenericMapper defaullt protected constructor
     */
    GenericMapper();

  public:
    /*!
     * Forbids \ref GenericMapper copy constructor
     *
     * \param other the \ref GenericMapper not to copy
     */
    GenericMapper(const GenericMapper<M,T>& other) = delete;

    /*!
     * Forbids \ref GenericMapper move constructor
     *
     * \param other the \ref GenericMapper not to move
     */
    GenericMapper(GenericMapper<M,T>&& other) = delete;
    // Constructors -

    // Destructor +
  protected:
    /*!
     * \ref GenericMapper destructor
     */
    virtual ~GenericMapper();
    // Destructor -

    // Methods +
  public:
    // Serialization +
    // Raw value +
    /*!
     * Serialize value
     *
     * \param value the value
     * \param context the context
     */
    void serialize(const T& value, anch::json::WriterContext& context) const;

    /*!
     * Serialize value according to optional/null option
     *
     * \param value the value
     * \param context the context
     */
    void serialize(const std::optional<T>& value, anch::json::WriterContext& context) const;

    /*!
     * Serialize value according to optional/null option
     *
     * \param value the value
     * \param context the context
     */
    void serialize(const T* const value, anch::json::WriterContext& context) const;

    /*!
     * Serialize value according to optional/null option
     *
     * \param value the value
     * \param context the context
     */
    void serialize(const std::shared_ptr<T>& value, anch::json::WriterContext& context) const;

    /*!
     * Serialize array values according to empty option
     *
     * \param value the value
     * \param context the context
     */
    void serialize(const std::vector<T>& value, anch::json::WriterContext& context) const;

    /*!
     * Serialize array values according to empty option
     *
     * \param value the value
     * \param context the context
     */
    void serialize(const std::list<T>& value, anch::json::WriterContext& context) const;

    /*!
     * Serialize array values according to empty option
     *
     * \param value the value
     * \param context the context
     */
    void serialize(const std::set<T>& value, anch::json::WriterContext& context) const;

    /*!
     * Serialize map values according to empty option
     *
     * \param value the value
     * \param context the context
     */
    void serialize(const std::map<std::string,T>& value, anch::json::WriterContext& context) const;
    // Raw value -

    // With field +
    /*!
     * Serialize value
     *
     * \param field the field's name
     * \param value the value
     * \param context the context
     *
     * \return \c true when field has been write, \c false otherwise
     */
    bool serialize(const std::string& field,
		   const T& value,
		   anch::json::WriterContext& context) const;

    /*!
     * Serialize value according to optional/null option
     *
     * \param field the field's name
     * \param value the value
     * \param context the context
     *
     * \return \c true when field has been write, \c false otherwise
     */
    bool serialize(const std::string& field,
		   const std::optional<T>& value,
		   anch::json::WriterContext& context) const;

    /*!
     * Serialize value according to optional/null option
     *
     * \param field the field's name
     * \param value the value
     * \param context the context
     *
     * \return \c true when field has been write, \c false otherwise
     */
    bool serialize(const std::string& field,
		   const T* const value,
		   anch::json::WriterContext& context) const;

    /*!
     * Serialize value according to optional/null option
     *
     * \param field the field's name
     * \param value the value
     * \param context the context
     *
     * \return \c true when field has been write, \c false otherwise
     */
    bool serialize(const std::string& field,
		   const std::shared_ptr<T>& value,
		   anch::json::WriterContext& context) const;

    /*!
     * Serialize array values according to empty option
     *
     * \param field the field's name
     * \param value the value
     * \param context the context
     *
     * \return \c true when field has been write, \c false otherwise
     */
    bool serialize(const std::string& field,
		   const std::vector<T>& value,
		   anch::json::WriterContext& context) const;

    /*!
     * Serialize array values according to empty option
     *
     * \param field the field's name
     * \param value the value
     * \param context the context
     *
     * \return \c true when field has been write, \c false otherwise
     */
    bool serialize(const std::string& field,
		   const std::list<T>& value,
		   anch::json::WriterContext& context) const;

    /*!
     * Serialize array values according to empty option
     *
     * \param field the field's name
     * \param value the value
     * \param context the context
     *
     * \return \c true when field has been write, \c false otherwise
     */
    bool serialize(const std::string& field,
		   const std::set<T>& value,
		   anch::json::WriterContext& context) const;

    /*!
     * Serialize map values according to empty option
     *
     * \param field the field's name
     * \param value the value
     * \param context the context
     *
     * \return \c true when field has been write, \c false otherwise
     */
    bool serialize(const std::string& field,
		   const std::map<std::string,T>& value,
		   anch::json::WriterContext& context) const;
    // With field -
    // Serialization -

    // Deserialization +
    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     *
     * \return \c false when value is \c null , \c false otherwise
     */
    bool deserialize(T& value, anch::json::ReaderContext& context) const;

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     *
     * \return \c false when value is \c null , \c false otherwise
     */
    bool deserialize(std::optional<T>& value, anch::json::ReaderContext& context) const;

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     *
     * \return \c false when value is \c null , \c false otherwise
     */
    bool deserialize(T* value, anch::json::ReaderContext& context) const;

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     *
     * \return \c false when value is \c null , \c false otherwise
     */
    bool deserialize(std::shared_ptr<T>& value, anch::json::ReaderContext& context) const;

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     */
    bool deserialize(std::vector<T>& value, anch::json::ReaderContext& context) const;

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     */
    bool deserialize(std::list<T>& value, anch::json::ReaderContext& context) const;

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     */
    bool deserialize(std::set<T>& value, anch::json::ReaderContext& context) const;

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     */
    bool deserialize(anch::Flux<T>& value, anch::json::ReaderContext& context) const;

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param context the mapping context
     */
    bool deserialize(std::map<std::string,T>& value, anch::json::ReaderContext& context) const;
    // Deserialization -
    // Methods -

  };

}

#include "json/impl/genericMapper.hpp"
