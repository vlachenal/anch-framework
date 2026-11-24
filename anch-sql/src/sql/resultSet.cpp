/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "sql/resultSet.hpp"
#include "sql/connection.hpp"

#include "convert.hpp"

#include <stdexcept>


using anch::sql::ResultSet;


ResultSet::ResultSet() noexcept: _fields(), _connection(NULL), _statement(NULL) {
  // Nothing to do
}

bool
ResultSet::next() {
  if(_connection == NULL) {
    return fetchNext();
  }
  return _connection->guard([this]() { return fetchNext(); });
}

void
ResultSet::invalidateStatement() noexcept {
  if(_statement != NULL) {
    _statement->invalidate();
  }
}

ResultSet::~ResultSet() noexcept {
  if(_statement != NULL) {
    --_statement->_openResults;
  }
}

const anch::date::DateFormatter&
ResultSet::getDefaultTimestampFormatter() {
  static anch::date::DateFormatter formatter("%Y-%m-%d %H:%M:%S.%s");
  return formatter;
}

namespace {

  /*!
   * Parse SQL date ( \c YYYY-MM-DD )
   *
   * \param value the value
   *
   * \return the date
   *
   * \throw anch::sql::SqlException invalid date (non fatal)
   */
  anch::date::Date
  parseDate(const std::string& value) {
    try {
      return anch::date::Date::parse(value);
    } catch(const std::invalid_argument& error) {
      throw anch::sql::SqlException(std::string("Invalid SQL date: ") + error.what(), true);
    }
  }

  /*!
   * Parse SQL time ( \c HH:MM:SS[.fraction] ; MySQL durations of 24 hours or more are not times of day)
   *
   * \param value the value
   *
   * \return the time
   *
   * \throw anch::sql::SqlException invalid time (non fatal)
   */
  anch::date::Time
  parseTime(const std::string& value) {
    try {
      return anch::date::Time::parse(value);
    } catch(const std::invalid_argument& error) {
      throw anch::sql::SqlException(std::string("Invalid SQL time: ") + error.what(), true);
    }
  }

}

// SQL field conversion specializations +
namespace anch::sql {

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
      throw SqlException(msg.str(), true);
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
	throw SqlException(msg.str(), true);
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
	throw SqlException(msg.str(), true);
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
	throw SqlException(msg.str(), true);
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
	throw SqlException(msg.str(), true);
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
	throw SqlException(msg.str(), true);
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
	throw SqlException(msg.str(), true);
      }
    }
    return null;
  }

  /*!
   * Get field \c anch::date::Date value by index.
   *
   * \param idx the index
   * \param out the result
   *
   * \throw SqlException any error
   */
  template<>
  bool
  ResultSet::get<anch::date::Date>(std::size_t idx, anch::date::Date& out) {
    bool null = true;
    std::string strVal;
    if(!get(idx, strVal)) {
      null = false;
      out = parseDate(strVal);
    }
    return null;
  }

  /*!
   * Get field \c anch::date::Time value by index.
   *
   * \param idx the index
   * \param out the result
   *
   * \throw SqlException any error
   */
  template<>
  bool
  ResultSet::get<anch::date::Time>(std::size_t idx, anch::date::Time& out) {
    bool null = true;
    std::string strVal;
    if(!get(idx, strVal)) {
      null = false;
      out = parseTime(strVal);
    }
    return null;
  }

  /*!
   * Get field \c anch::date::DateTime value by index.
   *
   * \param idx the index
   * \param out the result
   *
   * \throw SqlException any error
   */
  template<>
  bool
  ResultSet::get<anch::date::DateTime>(std::size_t idx, anch::date::DateTime& out) {
    bool null = true;
    std::string strVal;
    if(!get(idx, strVal)) {
      null = false;
      getTimestampFormatter().parse(strVal, out);
    }
    return null;
  }

  /*!
   * Get field \c string value by index.
   *
   * \param idx the index
   *
   * \return the optional value
   *
   * \throw SqlException any error
   */
  template<>
  std::optional<std::string>
  ResultSet::get<std::string>(std::size_t idx) {
    if(idx >= _fields.size()) {
      std::ostringstream msg;
      msg << "Index out of range: try to retrieve index " << idx
	  << " but result set contains " << _fields.size() << " fields";
      throw SqlException(msg.str(), true);
    }
    return getValue(idx);
  }

  /*!
   * Get field \c int64_t value by index.
   *
   * \param idx the index
   *
   * \return the optional value
   *
   * \throw SqlException any error
   */
  template<>
  std::optional<int64_t>
  ResultSet::get<int64_t>(std::size_t idx) {
    std::optional<int64_t> res;
    std::optional<std::string> strRes = get<std::string>(idx);
    if(strRes) {
      try {
	res = anch::convert<int64_t>(strRes.value());
      } catch(const std::bad_cast& e) {
	std::ostringstream msg;
	msg << "Can not convert '" << strRes.value() << "' into int64_t";
	throw SqlException(msg.str(), true);
      }
    }
    return res;
  }

  /*!
   * Get field \c int32_t value by index.
   *
   * \param idx the index
   *
   * \return the optional value
   *
   * \throw SqlException any error
   */
  template<>
  std::optional<int32_t>
  ResultSet::get<int32_t>(std::size_t idx) {
    std::optional<int32_t> res;
    std::optional<std::string> strRes = get<std::string>(idx);
    if(strRes) {
      try {
	res = anch::convert<int32_t>(strRes.value());
      } catch(const std::bad_cast& e) {
	std::ostringstream msg;
	msg << "Can not convert '" << strRes.value() << "' into int64_t";
	throw SqlException(msg.str(), true);
      }
    }
    return res;
  }

  /*!
   * Get field \c int16_t value by index.
   *
   * \param idx the index
   *
   * \return the optional value
   *
   * \throw SqlException any error
   */
  template<>
  std::optional<int16_t>
  ResultSet::get<int16_t>(std::size_t idx) {
    std::optional<int16_t> res;
    std::optional<std::string> strRes = get<std::string>(idx);
    if(strRes) {
      try {
	res = anch::convert<int16_t>(strRes.value());
      } catch(const std::bad_cast& e) {
	std::ostringstream msg;
	msg << "Can not convert '" << strRes.value() << "' into int64_t";
	throw SqlException(msg.str(), true);
      }
    }
    return res;
  }

  /*!
   * Get field \c uint64_t value by index.
   *
   * \param idx the index
   *
   * \return the optional value
   *
   * \throw SqlException any error
   */
  template<>
  std::optional<uint64_t>
  ResultSet::get<uint64_t>(std::size_t idx) {
    std::optional<uint64_t> res;
    std::optional<std::string> strRes = get<std::string>(idx);
    if(strRes) {
      try {
	res = anch::convert<uint64_t>(strRes.value());
      } catch(const std::bad_cast& e) {
	std::ostringstream msg;
	msg << "Can not convert '" << strRes.value() << "' into int64_t";
	throw SqlException(msg.str(), true);
      }
    }
    return res;
  }

  /*!
   * Get field \c uint32_t value by index.
   *
   * \param idx the index
   *
   * \return the optional value
   *
   * \throw SqlException any error
   */
  template<>
  std::optional<uint32_t>
  ResultSet::get<uint32_t>(std::size_t idx) {
    std::optional<uint32_t> res;
    std::optional<std::string> strRes = get<std::string>(idx);
    if(strRes) {
      try {
	res = anch::convert<uint32_t>(strRes.value());
      } catch(const std::bad_cast& e) {
	std::ostringstream msg;
	msg << "Can not convert '" << strRes.value() << "' into int64_t";
	throw SqlException(msg.str(), true);
      }
    }
    return res;
  }

  /*!
   * Get field \c uint16_t value by index.
   *
   * \param idx the index
   *
   * \return the optional value
   *
   * \throw SqlException any error
   */
  template<>
  std::optional<uint16_t>
  ResultSet::get<uint16_t>(std::size_t idx) {
    std::optional<uint16_t> res;
    std::optional<std::string> strRes = get<std::string>(idx);
    if(strRes) {
      try {
	res = anch::convert<uint16_t>(strRes.value());
      } catch(const std::bad_cast& e) {
	std::ostringstream msg;
	msg << "Can not convert '" << strRes.value() << "' into int64_t";
	throw SqlException(msg.str(), true);
      }
    }
    return res;
  }

  /*!
   * Get field \c anch::date::Date value by index.
   *
   * \param idx the index
   *
   * \return the optional value
   *
   * \throw SqlException any error
   */
  template<>
  std::optional<anch::date::Date>
  ResultSet::get<anch::date::Date>(std::size_t idx) {
    std::optional<anch::date::Date> res;
    std::optional<std::string> strRes = get<std::string>(idx);
    if(strRes) {
      res = parseDate(*strRes);
    }
    return res;
  }

  /*!
   * Get field \c anch::date::Time value by index.
   *
   * \param idx the index
   *
   * \return the optional value
   *
   * \throw SqlException any error
   */
  template<>
  std::optional<anch::date::Time>
  ResultSet::get<anch::date::Time>(std::size_t idx) {
    std::optional<anch::date::Time> res;
    std::optional<std::string> strRes = get<std::string>(idx);
    if(strRes) {
      res = parseTime(*strRes);
    }
    return res;
  }

  /*!
   * Get field \c anch::date::DateTime value by index.
   *
   * \param idx the index
   *
   * \return the optional value
   *
   * \throw SqlException any error
   */
  template<>
  std::optional<anch::date::DateTime>
  ResultSet::get<anch::date::DateTime>(std::size_t idx) {
    std::optional<anch::date::DateTime> res;
    std::optional<std::string> strRes = get<std::string>(idx);
    if(strRes) {
      anch::date::DateTime date;
      getTimestampFormatter().parse(strRes.value(), date);
      res = date;
    }
    return res;
  }

}
// SQL field conversion specializations -
