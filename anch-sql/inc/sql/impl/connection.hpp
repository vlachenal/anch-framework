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
#pragma once

namespace anch::sql {

  inline void
  Connection::begin() {
    startTransaction();
  }

  template<typename F>
  auto
  Connection::guard(F&& func) -> decltype(func()) {
    try {
      return func();
    } catch(const SqlException& e) {
      onError(e);
      throw;
    }
  }

  template<typename T, typename Iterable>
  uint64_t
  Connection::batchUpdate(const std::string& query, std::function<void(anch::sql::Parameters&, const T&)> mapper, const Iterable& values) {
    PreparedStatement& stmt = prepareStatement(query);
    uint64_t nbRows = 0;
    for(auto iter = values.cbegin() ; iter != values.cend() ; ++iter) {
      anch::sql::Parameters parameters;
      mapper(parameters, *iter);
      stmt.set(parameters);
      nbRows += stmt.executeUpdate();
    }
    return nbRows;
  }

  template<std::ranges::input_range Iterable>
  requires anch::Described<std::ranges::range_value_t<Iterable>>
  uint64_t
  Connection::batchUpdate(const std::string& query, const Iterable& values) {
    PreparedStatement& stmt = prepareStatement(query);
    uint64_t nbRows = 0;
    for(const auto& value : values) {
      stmt.set(anch::sql::fieldsOf(value));
      nbRows += stmt.executeUpdate();
    }
    return nbRows;
  }

  template<anch::Described T>
  ResultSet*
  Connection::query(const std::string& query, const anch::sql::FieldsOf<T>& fields) {
    PreparedStatement& stmt = prepareStatement(query);
    stmt.set(fields);
    return stmt.executeQuery();
  }

  template<anch::Described T>
  void
  Connection::queryMapRow(const std::string& query, std::function<void(ResultSet&)> rowMapper, const anch::sql::FieldsOf<T>& fields) {
    Connection::mapRow(this->query(query, fields), rowMapper);
  }

  template<anch::Described T>
  void
  Connection::queryExtract(const std::string& query, std::function<void(ResultSet&)> resExtractor, const anch::sql::FieldsOf<T>& fields) {
    Connection::extract(this->query(query, fields), resExtractor);
  }

  template<anch::Described T>
  uint64_t
  Connection::update(const std::string& query, const anch::sql::FieldsOf<T>& fields) {
    PreparedStatement& stmt = prepareStatement(query);
    stmt.set(fields);
    return stmt.executeUpdate();
  }

  inline
  bool
  Connection::isValid() const noexcept {
    return _valid && checkConnection();
  }

  inline
  void
  Connection::setValid(bool valid) noexcept {
    _valid = valid;
  }

}
