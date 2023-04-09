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

#include <map>
#include <string>
#include <istream>
#include <ostream>
#include <optional>
#include <any>

#include "singleton.hpp"


namespace anch::rest {

  /*!
   * Cast mapper according to content type
   *
   * \param contentType the content type
   * \param mapper the mapper to cast
   */
  auto& castMapper(const std::string& contentType, std::any& mapper);

  /*!
   * Mapper registry
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class MapperRegistry {

    // Attributes +
  private:
    /*! Mapper indexed by content type */
    std::map<std::string, std::any> _mappers;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref MapperRegistry default constructor
     */
    MapperRegistry();
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref MapperRegistry destructor
     */
    ~MapperRegistry();
    // Destructor -

    // Methods +
  public:
    /*!
     * Register mapper
     *
     * \tparam the mapper type
     *
     * \param contentType the content type to register the mapper
     * \param mapper the mapper to register
     */
    template<typename T>
    void registerMapper(const std::string& contentType, const T& mapper);

    /*!
     * Serialize object according to content type
     *
     * \tparam the object type
     *
     * \param contentType the content type
     * \param value the object
     * \param out the output stream to write in
     */
    template<typename T>
    void serialize(const std::string& contentType, T value, std::ostream& out);

    /*!
     * Deserialize object according to content type
     *
     * \tparam the object type
     *
     * \param contentType the content type
     * \param value the object
     * \param input the input stream to read fomr
     */
    template<typename T>
    void deserialize(const std::string& contentType, T& value, std::istream& input);

    /*!
     * Access mapper for modification
     *
     * \tparam the mapper type
     *
     * \param contentType the content type
     *
     * \return the mapper if found, \code{empty} otherwise
     */
    template<typename T>
    std::optional<T> mapper(const std::string& contentType);
    // Methods -

  };

  /*!
   * Global \ref MapperRegistry
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class GlobalMapperRegistry: public MapperRegistry, public anch::Singleton<GlobalMapperRegistry> {
    friend anch::Singleton<GlobalMapperRegistry>;

  private:
    /*!
     * \ref GlobalMapperRegistry private constructor
     */
    GlobalMapperRegistry();

    /*!
     * \ref GlobalMapperRegistry private destructor
     */
    virtual ~GlobalMapperRegistry();

  };

}  // anch::rest

#include "rest/impl/mapperRegistry.hpp"
