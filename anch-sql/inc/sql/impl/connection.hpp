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
#pragma once

namespace anch::sql {

  inline void
  Connection::begin() {
    startTransaction();
  }

  template<typename T, typename... Q>
  ResultSet*
  Connection::query(const std::string& query, const T& value, const Q&... values) {
    PreparedStatement& stmt = prepareStatement(query);
    std::size_t idx = 1;
    bindParameters(stmt, idx, value, values...);
    return stmt.executeQuery();
  }

  template<typename T, typename... Q>
  void
  Connection::queryMapRow(const std::string& query, std::function<void(ResultSet&)> rowMapper, const T& value, const Q&... values) {
    PreparedStatement& stmt = prepareStatement(query);
    std::size_t idx = 1;
    Connection::bindParameters(stmt, idx, value, values...);
    Connection::mapRow(stmt.executeQuery(), rowMapper);
  }

  template<typename T, typename... Q>
  void
  Connection::queryExtract(const std::string& query, std::function<void(ResultSet&)> resExtractor, const T& value, const Q&... values) {
    PreparedStatement& stmt = prepareStatement(query);
    std::size_t idx = 1;
    Connection::bindParameters(stmt, idx, value, values...);
    Connection::extract(stmt.executeQuery(), resExtractor);
  }

  template<typename T, typename... Q>
  uint64_t
  Connection::update(const std::string& query, const T& value, const Q&... values) {
    PreparedStatement& stmt = prepareStatement(query);
    std::size_t idx = 1;
    bindParameters(stmt, idx, value, values...);
    return stmt.executeUpdate();
  }

  template<typename T, typename Iterable>
  uint64_t
  Connection::batchUpdate(const std::string& query, std::function<void(PreparedStatement&, const T&)> mapper, const Iterable& values) {
    PreparedStatement& stmt = prepareStatement(query);
    uint64_t nbRows = 0;
    for(auto iter = values.cbegin() ; iter != values.cend() ; ++iter) {
      mapper(stmt, *iter);
      nbRows += stmt.executeUpdate();
    }
    return nbRows;
  }

  template<typename T, typename... Q>
  inline
  void
  Connection::bindParameters(PreparedStatement& stmt, std::size_t& idx, const T& value, const Q&... values) {
    stmt.set(idx, value);
    bindParameters(stmt, ++idx, values...);
  }

  template<typename T>
  inline
  void
  Connection::bindParameters(PreparedStatement& stmt, std::size_t& idx, const T& value) {
    stmt.set(idx, value);
  }

  inline
  bool
  Connection::isValid() const noexcept {
    return _valid;
  }

  inline
  void
  Connection::setValid(bool valid) noexcept {
    _valid = valid;
  }

}
