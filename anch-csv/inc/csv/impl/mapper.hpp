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

#include <charconv>
#include <stdexcept>
#include <system_error>
#include <utility>

#ifdef ANCH_DATE
#include "date/rfc3339.hpp"
#endif

namespace anch::csv::detail {

  template<anch::csv::Scalar V>
  bool
  parse(std::string_view text, V& value, const anch::csv::Options& options) {
    if constexpr (std::same_as<V, std::string>) {
      value.assign(text);
      return true;
    } else if constexpr (std::same_as<V, bool>) {
      if(text == options.trueValue) {
	value = true;
	return true;
      }
      if(text == options.falseValue) {
	value = false;
	return true;
      }
      return false;
    } else if constexpr (std::integral<V> || std::floating_point<V>) {
      const char* end = text.data() + text.size();
      const std::from_chars_result res = std::from_chars(text.data(), end, value);
      return res.ec == std::errc() && res.ptr == end;
#ifdef ANCH_UUID
    } else if constexpr (std::same_as<V, anch::UUID>) {
      try {
	value = anch::UUID::parseUUID(std::string(text));
	return true;
      } catch(const std::invalid_argument&) {
	return false;
      }
#endif
#ifdef ANCH_DATE
    } else if constexpr (std::same_as<V, anch::date::DateTime>) {
      try {
	value = anch::date::rfc3339::parseDateTime(text);
	return true;
      } catch(const std::invalid_argument&) {
	return false;
      }
    } else if constexpr (std::same_as<V, anch::date::Date>) {
      try {
	value = anch::date::rfc3339::parseDate(text);
	return true;
      } catch(const std::invalid_argument&) {
	return false;
      }
#endif
    } else {
      static_assert(sizeof(V) == 0, "Unsupported CSV value type");
    }
  }

  template<anch::csv::Scalar V>
  std::string
  format(const V& value, const anch::csv::Options& options) {
    if constexpr (std::same_as<V, std::string>) {
      return value;
    } else if constexpr (std::same_as<V, bool>) {
      return value ? options.trueValue : options.falseValue;
    } else if constexpr (std::integral<V> || std::floating_point<V>) {
      char buffer[64];
      const std::to_chars_result res = std::to_chars(buffer, buffer + sizeof(buffer), value);
      return std::string(buffer, res.ptr);
#ifdef ANCH_UUID
    } else if constexpr (std::same_as<V, anch::UUID>) {
      return value.toString();
#endif
#ifdef ANCH_DATE
    } else if constexpr (std::same_as<V, anch::date::DateTime>) {
      return anch::date::rfc3339::formatDateTime(value);
    } else if constexpr (std::same_as<V, anch::date::Date>) {
      return anch::date::rfc3339::formatDate(value);
#endif
    } else {
      static_assert(sizeof(V) == 0, "Unsupported CSV value type");
    }
  }

  /*! Value type of a field: the scalar type of a \c std::optional field, the field type otherwise */
  template<typename P>
  using ValueType = std::conditional_t<anch::val::isNullable<P>(), anch::val::detail::Unwrap<P>, P>;

}

namespace anch::csv {

  // Object mapper +
  template<typename T>
  ObjectMapper<T>::ObjectMapper(): _fields() {
    static_assert(anch::Described<T>, "T has to be described by an anch::Mapping<T> specialization (mapping.hpp), declared in"
		  " the header of T");
    anch::describe<T>(*this);
    _fields.shrink_to_fit();
  }

  template<typename T>
  const ObjectMapper<T>&
  ObjectMapper<T>::instance() {
    static const ObjectMapper<T> mapper;
    return mapper;
  }

  template<typename T>
  const std::vector<typename ObjectMapper<T>::Field>&
  ObjectMapper<T>::fields() const noexcept {
    return _fields;
  }

  template<typename T>
  template<typename P, typename S, typename... C>
  auto
  ObjectMapper<T>::reader(S set, C&&... constraints) {
    using V = anch::csv::detail::ValueType<P>;
    return [set, ...cts = std::forward<C>(constraints)](T& obj, std::string_view text, bool quoted,
							anch::csv::detail::ReadContext& context,
							const anch::csv::Options& options) -> void {
      if constexpr (anch::val::isNullable<P>()) {
	if(text.empty() && !quoted) { // absent value
	  set(obj, P());
	  return;
	}
      }
      V value{};
      if(!anch::csv::detail::parse(text, value, options)) {
	throw anch::csv::CsvError(anch::csv::ErrorCode::CONVERSION, context.name + ": invalid value '" + std::string(text) + "'",
				  context.line, context.column);
      }
      if constexpr (anch::val::hasValueConstraints<std::remove_cvref_t<C>...>()) {
	anch::val::detail::checkConstraints(value, context, cts...);
      } else {
	((void)cts, ...);
      }
      set(obj, P(std::move(value)));
    };
  }

  template<typename T>
  template<typename P, typename G, typename... C>
  auto
  ObjectMapper<T>::writer(G get, C&&... constraints) {
    return [get, ...cts = std::forward<C>(constraints)](const T& obj, anch::csv::detail::WriteContext& context,
							const anch::csv::Options& options) -> anch::csv::detail::FieldText {
      decltype(auto) value = std::invoke(get, obj);
      if constexpr (anch::val::hasValueConstraints<std::remove_cvref_t<C>...>()) {
	anch::val::detail::checkConstraints(value, context, cts...);
      } else {
	((void)cts, ...);
	(void)context;
      }
      if constexpr (anch::val::isNullable<P>()) {
	if(!value) {
	  return std::nullopt;
	}
	return anch::csv::detail::format(*value, options);
      } else {
	return anch::csv::detail::format(value, options);
      }
    };
  }

  template<typename T>
  template<typename P, typename... C>
  requires (anch::val::FieldConstraint<C, P> && ...)
  ObjectMapper<T>&
  ObjectMapper<T>::registerField(const std::string& name, P T::* member, C&&... constraints) {
    static_assert(anch::csv::FieldType<P>, "CSV fields are strings, booleans, numbers, UUID, dates or std::optional of them"
		  " (nested objects and collections are not supported)");
    _fields.push_back(Field{name, anch::val::isRequired<std::remove_cvref_t<C>...>(),
	reader<P>([member](T& obj, P&& value) { obj.*member = std::move(value); }, constraints...),
	writer<P>([member](const T& obj) -> const P& { return obj.*member; }, constraints...)});
    return *this;
  }

  template<typename T>
  template<typename P, typename... C>
  requires (anch::val::FieldConstraint<C, P> && ...)
  ObjectMapper<T>&
  ObjectMapper<T>::registerField(const std::string& name, std::function<P(const T&)> getter, C&&... constraints) {
    static_assert(anch::csv::FieldType<P>, "CSV fields are strings, booleans, numbers, UUID, dates or std::optional of them"
		  " (nested objects and collections are not supported)");
    _fields.push_back(Field{name, anch::val::isRequired<std::remove_cvref_t<C>...>(), {}, writer<P>(std::move(getter), constraints...)});
    return *this;
  }

  template<typename T>
  template<typename P, typename... C>
  requires (anch::val::FieldConstraint<C, P> && ...)
  ObjectMapper<T>&
  ObjectMapper<T>::registerField(const std::string& name, std::function<void(T&, const P&)> setter, C&&... constraints) {
    static_assert(anch::csv::FieldType<P>, "CSV fields are strings, booleans, numbers, UUID, dates or std::optional of them"
		  " (nested objects and collections are not supported)");
    _fields.push_back(Field{name, anch::val::isRequired<std::remove_cvref_t<C>...>(),
	reader<P>([setter](T& obj, P&& value) { setter(obj, value); }, constraints...), {}});
    return *this;
  }

  template<typename T>
  template<typename P, typename... C>
  requires (anch::val::FieldConstraint<C, P> && ...)
  ObjectMapper<T>&
  ObjectMapper<T>::registerField(const std::string& name, std::function<P(const T&)> getter,
				 std::function<void(T&, const P&)> setter, C&&... constraints) {
    static_assert(anch::csv::FieldType<P>, "CSV fields are strings, booleans, numbers, UUID, dates or std::optional of them"
		  " (nested objects and collections are not supported)");
    _fields.push_back(Field{name, anch::val::isRequired<std::remove_cvref_t<C>...>(),
	reader<P>([setter](T& obj, P&& value) { setter(obj, value); }, constraints...),
	writer<P>(std::move(getter), constraints...)});
    return *this;
  }
  // Object mapper -

  // Object reader +
  template<typename T>
  ObjectReader<T>::ObjectReader(anch::csv::Reader& reader):
    _reader(reader), _mapper(anch::csv::ObjectMapper<T>::instance()), _columns() {
    const auto& fields = _mapper.fields();
    const std::vector<std::string>& header = _reader.header();
    _columns.assign(fields.size(), NONE);
    if(!_reader.options().header) {
      // By position: each registered field is a column (as ObjectWriter writes them), columns of fields which are not
      // read are skipped
      for(std::size_t i = 0; i < fields.size(); ++i) {
	if(fields[i].read) {
	  _columns[i] = i;
	}
      }
      return;
    }
    if(header.empty()) { // empty input
      return;
    }
    for(std::size_t i = 0; i < fields.size(); ++i) {
      if(!fields[i].read) {
	continue;
      }
      _columns[i] = _reader.column(fields[i].name).value_or(NONE);
      if(_columns[i] == NONE && fields[i].required) {
	throw anch::csv::CsvError(anch::csv::ErrorCode::MISSING_COLUMN, "missing required column '" + fields[i].name + "'", 1, 0);
      }
    }
    if(!_reader.options().ignoreUnknownColumns) {
      for(std::size_t column = 0; column < header.size(); ++column) {
	bool known = false;
	for(std::size_t i = 0; i < fields.size() && !known; ++i) {
	  known = fields[i].read && fields[i].name == header[column];
	}
	if(!known) {
	  throw anch::csv::CsvError(anch::csv::ErrorCode::UNKNOWN_COLUMN, "unknown column '" + header[column] + "'", 1, column + 1);
	}
      }
    }
  }

  template<typename T>
  bool
  ObjectReader<T>::read(T& value) {
    if(!_reader.next()) {
      return false;
    }
    const auto& fields = _mapper.fields();
    const anch::csv::Options& options = _reader.options();
    for(std::size_t i = 0; i < fields.size(); ++i) {
      const std::size_t column = _columns[i];
      if(!fields[i].read || column == NONE) {
	continue;
      }
      if(column >= _reader.size()) {
	if(fields[i].required) {
	  throw anch::csv::CsvError(anch::csv::ErrorCode::MISSING_COLUMN, "missing required column '" + fields[i].name + "'",
				    _reader.line(), column + 1);
	}
	continue;
      }
      anch::csv::detail::ReadContext context{options.readCheckConstraints, fields[i].name, _reader.line(), column + 1};
      fields[i].read(value, _reader[column], _reader.quoted(column), context, options);
    }
    return true;
  }
  // Object reader -

  // Object writer +
  template<typename T>
  ObjectWriter<T>::ObjectWriter(anch::csv::Writer& writer, bool header):
    _writer(writer), _mapper(anch::csv::ObjectMapper<T>::instance()), _header(header), _positional(!header), _records(0) {
    // Nothing to do
  }

  template<typename T>
  void
  ObjectWriter<T>::writeHeader() {
    if(!_header) {
      return;
    }
    std::vector<std::string_view> names;
    for(const auto& field : _mapper.fields()) {
      if(field.write) {
	names.push_back(field.name);
      }
    }
    _writer.write(std::span<const std::string_view>(names));
    _header = false;
    ++_records;
  }

  template<typename T>
  void
  ObjectWriter<T>::write(const T& value) {
    if(_header) {
      writeHeader();
    }
    const anch::csv::Options& options = _writer.options();
    std::vector<std::string> texts;
    std::vector<bool> quoted;
    for(const auto& field : _mapper.fields()) {
      if(!field.write) {
	if(_positional) { // the column of the field is kept: records are read by position
	  quoted.push_back(false);
	  texts.emplace_back();
	}
	continue;
      }
      anch::csv::detail::WriteContext context{options.writeCheckConstraints, field.name, _records + 1, texts.size() + 1};
      anch::csv::detail::FieldText text = field.write(value, context, options);
      quoted.push_back(text.has_value() && text->empty()); // empty string: "" ; absent value: empty field
      texts.push_back(text.value_or(std::string()));
    }
    const std::vector<std::string_view> fields(texts.begin(), texts.end());
    _writer.write(std::span<const std::string_view>(fields), quoted);
    ++_records;
  }
  // Object writer -

}
