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
    private:
      /*! Result set fields */
      std::map<std::string, std::size_t> _fields;

      /** Number of result */
      int _nbRow;

    protected:
      /*! Current row */
      int _currentRow;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref ResultSet constructor
       *
       * \param fields the fields' name
       * \param nbRow the number of row in result set
       */
      ResultSet(const std::vector<std::string>& fields, int nbRow);
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref ResultSet destrcutor
       */
      virtual ~ResultSet();
      // Destructor -

      // Methods +
      /*!
       * Check if there is more row in result set
       *
       * \return \c true if there is more row, \c false otherwise
       */
      inline bool hasNext() {
	return (_currentRow + 1 < _nbRow);
      }

      /*!
       * Retrieve next row
       *
       * \throw SqlException any error
       */
      void next() throw(SqlException);

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
      const T* get(std::size_t idx) throw(SqlException);

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
       */
      template<typename T>
      bool get(std::size_t idx, T& out) throw(SqlException);

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
      bool get(const std::string field, T& out) throw(SqlException) {
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
      const T* get(const std::string field) throw(SqlException) {
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

    protected:
      /*!
       * Retrieve string value from result set according to SQL database engine.
       *
       * \param idx the field index
       * \param out the result
       */
      virtual bool getValue(std::size_t idx, std::string& out) = 0;

      /*!
       * Fetch next row in SQL result set.
       *
       * \throw SqlException any error
       */
      virtual void fetchNextRow() throw(SqlException) = 0;
      // Methods -

    };

  }
}

#endif // _ANCH_SQL_RESULT_SET_H_
