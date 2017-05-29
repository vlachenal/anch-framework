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
#ifndef _ANCH_SQL_RESULT_SET_H_
#define _ANCH_SQL_RESULT_SET_H_

#include <string>
#include <vector>
#include <sstream>
#include <map>

#include "sql/sqlException.hpp"

namespace anch {
  namespace sql {

    /*!
     * \brief SQL result representation
     *
     * The \ref ResultSet has to be used with while(res.hasNext()) {res.next(); [...]}.\n
     * Field getter is templated. Specializations are included in library. If you need other conversions,
     * you can implement them in your own program.
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class ResultSet {

      // Attributes +
    protected:
      /*! Result set fields */
      std::map<std::string, std::size_t> _fields;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref ResultSet default constructor
       */
      ResultSet() noexcept;
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref ResultSet destrcutor
       */
      virtual ~ResultSet() noexcept;
      // Destructor -

      // Methods +
      /*!
       * Get field value by index.\n
       * You have to delete result once treated.
       *
       * \tparam T expected type
       *
       * \param idx the index
       *
       * \return the result
       *
       * \throw SqlException any error
       */
      template<typename T>
      const T* get(std::size_t idx);

      /*!
       * Get field value by index.\n
       * The output parameter will be set only if SQL result is not \c NULL.
       *
       * \tparam T expected type
       *
       * \param idx the index
       * \param out the result
       *
       * \return \c true if result is NULL, \c false otherwise
       *
       * \throw SqlException any error
       */
      template<typename T>
      bool get(std::size_t idx, T& out);

      /*!
       * Get field value by field name.\n
       * The output parameter will be set only if SQL result is not \c NULL.
       *
       * \tparam T expected type
       *
       * \param field the field name
       * \param out the result
       *
       * \return \c true if result is NULL, \c false otherwise
       */
      template<typename T>
      bool get(const std::string field, T& out) {
	auto search = _fields.find(field);
	if(search == _fields.end()) {
	  std::ostringstream msg;
	  msg << "Field " << field << " does not exist in result set. "
	    "Available fields are [";
	  for(auto iter = _fields.cbegin() ; iter != _fields.cend() ; ++iter) {
	    if(iter != _fields.cbegin()) {
	      msg << ',';
	    }
	    msg << iter->first;
	  }
	  msg << ']';
	  throw SqlException(msg.str());
	}
	return get<T>(search->second, out);
      }

      /*!
       * Get field value by field name.\n
       * You have to delete result once treated.
       *
       * \tparam T expected type
       *
       * \param field the field name
       *
       * \return the result
       *
       * \throw SqlException any error
       */
      template<typename T>
      const T* get(const std::string field) {
	auto search = _fields.find(field);
	if(search == _fields.end()) {
	  std::ostringstream msg;
	  msg << "Field " << field << " does not exist in result set. "
	    "Available fields are [";
	  for(auto iter = _fields.cbegin() ; iter != _fields.cend() ; ++iter) {
	    if(iter != _fields.cbegin()) {
	      msg << ',';
	    }
	    msg << iter->first;
	  }
	  msg << ']';
	  throw SqlException(msg.str());
	}
	return get<T>(search->second);
      }

      /*!
       * Retrieve next row
       *
       * \return \c true if next row exists, \c false otherwise
       *
       * \throw SqlException any error
       */
      virtual bool next() = 0;

    protected:
      /*!
       * Retrieve string value from result set according to SQL database engine.
       *
       * \param idx the field index
       * \param out the result
       *
       * \throw SqlException any error
       */
      virtual bool getValue(std::size_t idx, std::string& out) = 0;
      // Methods -

    };

  }
}

#endif // _ANCH_SQL_RESULT_SET_H_
