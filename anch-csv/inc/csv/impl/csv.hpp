/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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

namespace anch::csv {

  // Iterator +
  template<typename T>
  Records<T>::iterator::iterator() noexcept: _records(nullptr) {
    // Nothing to do
  }

  template<typename T>
  Records<T>::iterator::iterator(Records<T>* records) noexcept: _records(records) {
    // Nothing to do
  }

  template<typename T>
  const T&
  Records<T>::iterator::operator*() const noexcept {
    return _records->_value;
  }

  template<typename T>
  typename Records<T>::iterator&
  Records<T>::iterator::operator++() {
    _records->next();
    return *this;
  }

  template<typename T>
  void
  Records<T>::iterator::operator++(int) {
    ++*this;
  }

  template<typename T>
  bool
  Records<T>::iterator::operator==(std::default_sentinel_t) const noexcept {
    return _records == nullptr || _records->_done;
  }
  // Iterator -

  template<typename T>
  Records<T>::Records(std::istream& input, const anch::csv::Options& options):
    _reader(input, options), _objects(_reader), _value(), _done(false) {
    // Nothing to do
  }

  template<typename T>
  void
  Records<T>::next() {
    _value = T();
    _done = !_objects.read(_value);
  }

  template<typename T>
  typename Records<T>::iterator
  Records<T>::begin() {
    next();
    return iterator(this);
  }

  template<typename T>
  std::default_sentinel_t
  Records<T>::end() const noexcept {
    return std::default_sentinel;
  }

  template<typename T>
  anch::csv::Records<T>
  records(std::istream& input, const anch::csv::Options& options) {
    return anch::csv::Records<T>(input, options);
  }

  template<anch::Described T>
  void
  deserialize(std::vector<T>& values, std::istream& input, const anch::csv::Options& options) {
    values.clear();
    anch::csv::Reader reader(input, options);
    anch::csv::ObjectReader<T> objects(reader);
    T value{};
    while(objects.read(value)) {
      values.push_back(std::move(value));
      value = T();
    }
  }

  template<std::ranges::input_range R>
  requires anch::Described<std::remove_cvref_t<std::ranges::range_value_t<R>>>
  void
  serialize(const R& values, std::ostream& out, const anch::csv::Options& options) {
    using T = std::remove_cvref_t<std::ranges::range_value_t<R>>;
    anch::csv::Writer writer(out, options);
    anch::csv::ObjectWriter<T> objects(writer, options.header);
    objects.writeHeader();
    for(const T& value : values) {
      objects.write(value);
    }
  }

}
