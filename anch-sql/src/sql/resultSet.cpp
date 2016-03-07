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


ResultSet::ResultSet(): _fields() {
  // Nothing to do
}

ResultSet::~ResultSet() {
  // Nothing to do
}

// SQL field conversion specializations +
namespace anch {
  namespace sql {

    template<>
    bool
    ResultSet::get<std::string>(std::size_t idx, std::string& out) throw(SqlException) {
      if(idx >= _fields.size()) {
	std::ostringstream msg;
	msg << "Index out of range: try to retrieve index " << idx
	    << " but result set contains " << _fields.size() << " fields";
	throw SqlException(msg.str());
      }
      return getValue(idx, out);
    }

    template<>
    bool
    ResultSet::get<int64_t>(std::size_t idx, int64_t& out) throw(SqlException) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	out = anch::convert<int64_t>(strVal);
      }
      return null;
    }

    template<>
    bool
    ResultSet::get<int32_t>(std::size_t idx, int32_t& out) throw(SqlException) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	out = anch::convert<int32_t>(strVal);
      }
      return null;
    }

    template<>
    bool
    ResultSet::get<int16_t>(std::size_t idx, int16_t& out) throw(SqlException) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	out = anch::convert<int16_t>(strVal);
      }
      return null;
    }

    template<>
    bool
    ResultSet::get<uint64_t>(std::size_t idx, uint64_t& out) throw(SqlException) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	out = anch::convert<uint64_t>(strVal);
      }
      return null;
    }

    template<>
    bool
    ResultSet::get<uint32_t>(std::size_t idx, uint32_t& out) throw(SqlException) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	out = anch::convert<uint32_t>(strVal);
      }
      return null;
    }

    template<>
    bool
    ResultSet::get<uint16_t>(std::size_t idx, uint16_t& out) throw(SqlException) {
      bool null = true;
      std::string strVal;
      if(!get(idx, strVal)) {
	null = false;
	out = anch::convert<uint16_t>(strVal);
      }
      return null;
    }

    template<>
    const std::string*
    ResultSet::get<std::string>(std::size_t idx) throw(SqlException) {
      std::string out;
      std::string* res = NULL;
      if(!get(idx, out)) {
	res = new std::string(out);
      }
      return res;
    }

    template<>
    const int64_t*
    ResultSet::get<int64_t>(std::size_t idx) throw(SqlException) {
      int64_t* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	res = new int64_t();
	*res = anch::convert<int64_t>(strVal);
      }
      return res;
    }

    template<>
    const int32_t*
    ResultSet::get<int32_t>(std::size_t idx) throw(SqlException) {
      int32_t* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	res = new int32_t();
	*res = anch::convert<int32_t>(strVal);
      }
      return res;
    }

    template<>
    const int16_t*
    ResultSet::get<int16_t>(std::size_t idx) throw(SqlException) {
      int16_t* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	res = new int16_t();
	*res = anch::convert<int16_t>(strVal);
      }
      return res;
    }

    template<>
    const uint64_t*
    ResultSet::get<uint64_t>(std::size_t idx) throw(SqlException) {
      uint64_t* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	res = new uint64_t();
	*res = anch::convert<uint64_t>(strVal);
      }
      return res;
    }

    template<>
    const uint32_t*
    ResultSet::get<uint32_t>(std::size_t idx) throw(SqlException) {
      uint32_t* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	res = new uint32_t();
	*res = anch::convert<uint32_t>(strVal);
      }
      return res;
    }

    template<>
    const uint16_t*
    ResultSet::get<uint16_t>(std::size_t idx) throw(SqlException) {
      uint16_t* res = NULL;
      std::string strVal;
      if(!get(idx, strVal)) {
	res = new uint16_t();
	*res = anch::convert<uint16_t>(strVal);
      }
      return res;
    }

  }
}
// SQL field conversion specializations -
