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

#include <ostream>
#include <string>
#include <iomanip>

#include "json/mapper.hpp"


namespace anch {
  namespace json {

    /*!
     * Fields registry function\n
     * This function has to be specialized for each class/structure which has to be serialized/deserialized.
     *
     * \tparam T the object type
     *
     * \param mapper the \ref ObjectMapper to use
     */
    template<typename T>
    void registerObject(anch::json::ObjectMapper<T>& mapper);

    /*!
     * Serialize object
     *
     * \tparam T the object type
     *
     * \param value the object to serialize
     * \param out the output stream to write in
     */
    template<typename T>
    void serialize(const T& value, std::ostream& out);

    /*!
     * Serialize objects
     *
     * \tparam T the object type
     *
     * \param value the objects to serialize
     * \param out the output stream to write in
     */
    template<typename T>
    void serialize(const std::vector<T>& value, std::ostream& out);

    /*!
     * Serialize objects
     *
     * \tparam T the object type
     *
     * \param value the objects to serialize
     * \param out the output stream to write in
     */
    template<typename T>
    void serialize(const std::list<T>& value, std::ostream& out);

    /*!
     * Serialize objects
     *
     * \tparam T the object type
     *
     * \param value the objects to serialize
     * \param out the output stream to write in
     */
    template<typename T>
    void serialize(const std::set<T>& value, std::ostream& out);

    /*!
     * Serialize object
     *
     * \tparam T the object type
     *
     * \param value the object to serialize
     *
     * \return the serialized object as \c std::string
     */
    template<typename T>
    std::string serialize(const T& value);

    /*!
     * Serialize objects
     *
     * \tparam T the object type
     *
     * \param out the output stream to write in
     *
     * \return the serialized objects as \c std::string
     */
    template<typename T>
    std::string serialize(const std::vector<T>& value);

    /*!
     * Serialize objects
     *
     * \tparam T the object type
     *
     * \param out the output stream to write in
     *
     * \return the serialized objects as \c std::string
     */
    template<typename T>
    std::string serialize(const std::list<T>& value);

    /*!
     * Serialize objects
     *
     * \tparam T the object type
     *
     * \param out the output stream to write in
     *
     * \return the serialized objects as \c std::string
     */
    template<typename T>
    std::string serialize(const std::set<T>& value);

    /*!
     * Deserialize object
     *
     * \tparam T the object type
     *
     * \param value the value to fill
     * \param input the input stream to parse
     */
    template<typename T>
    void deserialize(T& value, std::istream& input);

    /*!
     * Deserialize object
     *
     * \tparam T the object type
     *
     * \param input the input stream to parse
     *
     * \return the deserialized object by copy
     */
    template<typename T>
    T deserialize(std::istream& input);

    // struct JSONIODecorator {
    // private:
    //   std::ostream & out;
    // public:
    //   explicit JSONIODecorator(): out() {}
    //   template<typename Rhs>
    //   friend std::ostream & operator<<(JSONIODecorator const& q, Rhs const& rhs) {
    // 	return q.os << rhs;
    //   }

    //   friend std::ostream& operator<<(JSONIODecorator const& q, std::string const& rhs) {
    //     return q.os << "'" << rhs << "'";
    //   }

    //   friend std::ostream& operator<<(JSONIODecorator const& q, char const* rhs) {
    //     return q.os << "'" << rhs << "'";
    //   }

    // };

    // struct jsonify_creator { } json_format;

    // JSONIODecorator operator<<(std::ostream & os, jsonify_creator) {
    //   return JSONIODecorator(os);
    // }

    // template<typename T>
    // std::ostream& JSONIODecorator(std::ostream& out, const T& value) {
    //   serialize(value, out);
    //   return out;
    // }

    // template<typename T>
    // std::omanip<T> JSONIODecorator() {
    //   serialize(value, out);
    //   return out;
    // }

  }  // json
}  // anch

#include "json/impl/json.hpp"
