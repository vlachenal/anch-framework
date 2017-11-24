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
#include "sql/resultSet.hpp"

#include "convert.hpp"


using anch::sql::ResultSet;


ResultSet::ResultSet() noexcept: _fields() {
  // Nothing to do
}

ResultSet::~ResultSet() noexcept {
  // Nothing to do
}

const anch::date::DateFormatter&
ResultSet::getDefaultDateFormatter() {
  static anch::date::DateFormatter formatter("%Y-%m-%d");
  return formatter;
}

const anch::date::DateFormatter&
ResultSet::getDefaultTimeFormatter() {
  static anch::date::DateFormatter formatter("%H:%M:%S.%s");
  return formatter;
}

const anch::date::DateFormatter&
ResultSet::getDefaultTimestampFormatter() {
  static anch::date::DateFormatter formatter("%Y-%m-%d %H:%M:%S.%s");
  return formatter;
}

// SQL field conversion specializations +
namespace anch {
  namespace sql {

    /*!
     * Get field string value by index.\n
     * The output parameter will be set only if SQL result is not \c NULL.
     *
     * \param idx the index
     * \param out the result
     *
     * \return \c true if result is NULL, \c false otherwise
     *
     * \throw SqlException any error
     */
    template<>
    bool
    ResultSet::get<std::string>(std::size_t idx, std::string& out) {
      if(idx >= _fields.size()) {
	std::ostringstream msg;
	msg << "Index out of range: try to retrieve index " << idx
	    << " but result set contains " << _fields.size() << " fields";
	throw SqlException(msg.str());
      }
      return getValue(idx, out);
    }

    /*!
     * Get field int64_t value by index.\n
     * The output parameter will be set only if SQL result is not \c NULL.
     *
     * \param idx the index
     * \param out the result
     *
     * \return \c true if result is NULL, \c false otherwise
     *
     * \throw SqlException any error
     */
    template<>
    bool
    ResultSet::get<int64_t>(std::size_t idx, int64_t& out) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	try {
	  out = anch::convert<int64_t>(strVal);
	} catch(const std::bad_cast& e) {
	  std::ostringstream msg;
	  msg << "Can not convert '" << strVal << "' into int64_t";
	  throw SqlException(msg.str());
	}
      }
      return null;
    }

    /*!
     * Get field int32_t value by index.\n
     * The output parameter will be set only if SQL result is not \c NULL.
     *
     * \param idx the index
     * \param out the result
     *
     * \return \c true if result is NULL, \c false otherwise
     *
     * \throw SqlException any error
     */
    template<>
    bool
    ResultSet::get<int32_t>(std::size_t idx, int32_t& out) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	try {
	  out = anch::convert<int32_t>(strVal);
	} catch(const std::bad_cast& e) {
	  std::ostringstream msg;
	  msg << "Can not convert '" << strVal << "' into int32_t";
	  throw SqlException(msg.str());
	}
      }
      return null;
    }

    /*!
     * Get field int16_t value by index.\n
     * The output parameter will be set only if SQL result is not \c NULL.
     *
     * \param idx the index
     * \param out the result
     *
     * \return \c true if result is NULL, \c false otherwise
     *
     * \throw SqlException any error
     */
    template<>
    bool
    ResultSet::get<int16_t>(std::size_t idx, int16_t& out) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	try {
	  out = anch::convert<int16_t>(strVal);
	} catch(const std::bad_cast& e) {
	  std::ostringstream msg;
	  msg << "Can not convert '" << strVal << "' into int16_t";
	  throw SqlException(msg.str());
	}
      }
      return null;
    }

    /*!
     * Get field uint64_t value by index.\n
     * The output parameter will be set only if SQL result is not \c NULL.
     *
     * \param idx the index
     * \param out the result
     *
     * \return \c true if result is NULL, \c false otherwise
     *
     * \throw SqlException any error
     */
    template<>
    bool
    ResultSet::get<uint64_t>(std::size_t idx, uint64_t& out) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	try {
	  out = anch::convert<uint64_t>(strVal);
	} catch(const std::bad_cast& e) {
	  std::ostringstream msg;
	  msg << "Can not convert '" << strVal << "' into uint64_t";
	  throw SqlException(msg.str());
	}
      }
      return null;
    }

    /*!
     * Get field uint32_t value by index.\n
     * The output parameter will be set only if SQL result is not \c NULL.
     *
     * \param idx the index
     * \param out the result
     *
     * \return \c true if result is NULL, \c false otherwise
     *
     * \throw SqlException any error
     */
    template<>
    bool
    ResultSet::get<uint32_t>(std::size_t idx, uint32_t& out) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	try {
	  out = anch::convert<uint32_t>(strVal);
	} catch(const std::bad_cast& e) {
	  std::ostringstream msg;
	  msg << "Can not convert '" << strVal << "' into uint32_t";
	  throw SqlException(msg.str());
	}
      }
      return null;
    }

    /*!
     * Get field uint16_t value by index.\n
     * The output parameter will be set only if SQL result is not \c NULL.
     *
     * \param idx the index
     * \param out the result
     *
     * \return \c true if result is NULL, \c false otherwise
     *
     * \throw SqlException any error
     */
    template<>
    bool
    ResultSet::get<uint16_t>(std::size_t idx, uint16_t& out) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	try {
	  out = anch::convert<uint16_t>(strVal);
	} catch(const std::bad_cast& e) {
	  std::ostringstream msg;
	  msg << "Can not convert '" << strVal << "' into uint16_t";
	  throw SqlException(msg.str());
	}
      }
      return null;
    }

    /*!
     * Get field \ref Date value by index.\n
     * You have to delete result once treated.
     *
     * \param idx the index
     * \param out the result
     *
     * \throw SqlException any error
     */
    template<>
    bool
    ResultSet::get<anch::sql::Date>(std::size_t idx, anch::sql::Date& out) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	const anch::date::DateFormatter& formatter = getDateFormatter();
	anch::date::Date date;
	formatter.parse(strVal, date);
	out = new anch::sql::Date(date);
      }
      return null;
    }

    /*!
     * Get field \ref Time value by index.\n
     * You have to delete result once treated.
     *
     * \param idx the index
     * \param out the result
     *
     * \throw SqlException any error
     */
    template<>
    bool
    ResultSet::get<anch::sql::Time>(std::size_t idx, anch::sql::Time& out) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	const anch::date::DateFormatter& formatter = getTimeFormatter();
	anch::date::Date date;
	formatter.parse(strVal, date);
	out = new anch::sql::Time(date);
      }
      return null;
    }

    /*!
     * Get field \ref Timestamp value by index.\n
     * You have to delete result once treated.
     *
     * \param idx the index
     * \param out the result
     *
     * \throw SqlException any error
     */
    template<>
    bool
    ResultSet::get<anch::sql::Timestamp>(std::size_t idx, anch::sql::Timestamp& out) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	const anch::date::DateFormatter& formatter = getTimestampFormatter();
	anch::date::Date date;
	formatter.parse(strVal, date);
	out = new anch::sql::Timestamp(date);
      }
      return null;
    }

    /*!
     * Get field string value by index.\n
     * You have to delete result once treated.
     *
     * \param idx the index
     *
     * \return the result
     *
     * \throw SqlException any error
     */
    template<>
    const std::string*
    ResultSet::get<std::string>(std::size_t idx) {
      std::string out;
      std::string* res = NULL;
      if(!get(idx, out)) {
	res = new std::string(out);
      }
      return res;
    }

    /*!
     * Get field int64_t value by index.\n
     * You have to delete result once treated.
     *
     * \param idx the index
     *
     * \return the result
     *
     * \throw SqlException any error
     */
    template<>
    const int64_t*
    ResultSet::get<int64_t>(std::size_t idx) {
      int64_t* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	res = new int64_t();
	try {
	  *res = anch::convert<int64_t>(strVal);
	} catch(const std::bad_cast& e) {
	  delete res;
	  std::ostringstream msg;
	  msg << "Can not convert '" << strVal << "' into int64_t";
	  throw SqlException(msg.str());
	}
      }
      return res;
    }

    /*!
     * Get field int32_t value by index.\n
     * You have to delete result once treated.
     *
     * \param idx the index
     *
     * \return the result
     *
     * \throw SqlException any error
     */
    template<>
    const int32_t*
    ResultSet::get<int32_t>(std::size_t idx) {
      int32_t* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	res = new int32_t();
	try {
	  *res = anch::convert<int32_t>(strVal);
	} catch(const std::bad_cast& e) {
	  delete res;
	  std::ostringstream msg;
	  msg << "Can not convert '" << strVal << "' into int32_t";
	  throw SqlException(msg.str());
	}
      }
      return res;
    }

    /*!
     * Get field int16_t value by index.\n
     * You have to delete result once treated.
     *
     * \param idx the index
     *
     * \return the result
     *
     * \throw SqlException any error
     */
    template<>
    const int16_t*
    ResultSet::get<int16_t>(std::size_t idx) {
      int16_t* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	res = new int16_t();
	try {
	  *res = anch::convert<int16_t>(strVal);
	} catch(const std::bad_cast& e) {
	  delete res;
	  std::ostringstream msg;
	  msg << "Can not convert '" << strVal << "' into int16_t";
	  throw SqlException(msg.str());
	}
      }
      return res;
    }

    /*!
     * Get field uint64_t value by index.\n
     * You have to delete result once treated.
     *
     * \param idx the index
     *
     * \return the result
     *
     * \throw SqlException any error
     */
    template<>
    const uint64_t*
    ResultSet::get<uint64_t>(std::size_t idx) {
      uint64_t* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	res = new uint64_t();
	try {
	  *res = anch::convert<uint64_t>(strVal);
	} catch(const std::bad_cast& e) {
	  delete res;
	  std::ostringstream msg;
	  msg << "Can not convert '" << strVal << "' into uint64_t";
	  throw SqlException(msg.str());
	}
      }
      return res;
    }

    /*!
     * Get field uint32_t value by index.\n
     * You have to delete result once treated.
     *
     * \param idx the index
     *
     * \return the result
     *
     * \throw SqlException any error
     */
    template<>
    const uint32_t*
    ResultSet::get<uint32_t>(std::size_t idx) {
      uint32_t* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	res = new uint32_t();
	try {
	  *res = anch::convert<uint32_t>(strVal);
	} catch(const std::bad_cast& e) {
	  delete res;
	  std::ostringstream msg;
	  msg << "Can not convert '" << strVal << "' into uint32_t";
	  throw SqlException(msg.str());
	}
      }
      return res;
    }

    /*!
     * Get field uint16_t value by index.\n
     * You have to delete result once treated.
     *
     * \param idx the index
     *
     * \return the result
     *
     * \throw SqlException any error
     */
    template<>
    const uint16_t*
    ResultSet::get<uint16_t>(std::size_t idx) {
      uint16_t* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	res = new uint16_t();
	try {
	  *res = anch::convert<uint16_t>(strVal);
	} catch(const std::bad_cast& e) {
	  delete res;
	  std::ostringstream msg;
	  msg << "Can not convert '" << strVal << "' into uint16_t";
	  throw SqlException(msg.str());
	}
      }
      return res;
    }

    /*!
     * Get field \ref Date value by index.\n
     * You have to delete result once treated.
     *
     * \param idx the index
     *
     * \return the result
     *
     * \throw SqlException any error
     */
    template<>
    const anch::sql::Date*
    ResultSet::get<anch::sql::Date>(std::size_t idx) {
      anch::sql::Date* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	const anch::date::DateFormatter& formatter = getDateFormatter();
	anch::date::Date date;
	formatter.parse(strVal, date);
	res = new anch::sql::Date(date);
      }
      return res;
    }

    /*!
     * Get field \ref Time value by index.\n
     * You have to delete result once treated.
     *
     * \param idx the index
     *
     * \return the result
     *
     * \throw SqlException any error
     */
    template<>
    const anch::sql::Time*
    ResultSet::get<anch::sql::Time>(std::size_t idx) {
      anch::sql::Time* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	const anch::date::DateFormatter& formatter = getDateFormatter();
	anch::date::Date date;
	formatter.parse(strVal, date);
	res = new anch::sql::Time(date);
      }
      return res;
    }

    /*!
     * Get field \ref Timestamp value by index.\n
     * You have to delete result once treated.
     *
     * \param idx the index
     *
     * \return the result
     *
     * \throw SqlException any error
     */
    template<>
    const anch::sql::Timestamp*
    ResultSet::get<anch::sql::Timestamp>(std::size_t idx) {
      anch::sql::Timestamp* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	const anch::date::DateFormatter& formatter = getDateFormatter();
	anch::date::Date date;
	formatter.parse(strVal, date);
	res = new anch::sql::Timestamp(date);
      }
      return res;
    }

  }
}
// SQL field conversion specializations -
