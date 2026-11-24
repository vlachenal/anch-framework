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

#include <chrono>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "mapping.hpp"

#include "csv/csvError.hpp"
#include "csv/options.hpp"
#include "csv/reader.hpp"
#include "csv/writer.hpp"
#include "val/constraints.hpp"

#ifdef ANCH_UUID
#include "uuid.hpp"
#endif
#ifdef ANCH_DATE
#include "date/dateTime.hpp"
#endif

namespace anch::csv {

  /*!
   * \brief Type of a CSV field value: string, boolean, integer, floating point number, \c anch::UUID (with
   * \c ANCH_UUID ), \c anch::date::DateTime , \c anch::date::Date and \c anch::date::Time (RFC 3339, with \c ANCH_DATE )
   *
   * Fields hold such a value or a \c std::optional of it (an empty field is an absent value). Objects are flat:
   * nested objects and collections are refused at compile time.
   *
   * \tparam V the value type
   *
   * \since 0.3
   */
  template<typename V>
  concept Scalar = std::same_as<V, std::string> || std::same_as<V, bool>
    || (std::integral<V> && !std::same_as<V, char> && !std::same_as<V, wchar_t> && !std::same_as<V, char8_t>
	&& !std::same_as<V, char16_t> && !std::same_as<V, char32_t>)
    || std::floating_point<V>
#ifdef ANCH_UUID
    || std::same_as<V, anch::UUID>
#endif
#ifdef ANCH_DATE
    || std::same_as<V, anch::date::DateTime> || std::same_as<V, anch::date::Date> || std::same_as<V, anch::date::Time>
#endif
    ;

  /*!
   * \brief Type of a CSV field: \ref anch::csv::Scalar "Scalar" or \c std::optional of a scalar
   *
   * \tparam P the field type
   *
   * \since 0.3
   */
  template<typename P>
  concept FieldType = anch::csv::Scalar<P>
    || (anch::val::isNullable<P>() && std::same_as<P, std::optional<anch::val::detail::Unwrap<P>>>
	&& anch::csv::Scalar<anch::val::detail::Unwrap<P>>);

}

namespace anch::csv::detail {

  /*!
   * Parse field value
   *
   * \param text the field text
   * \param value the value to set
   * \param options the CSV options (booleans texts)
   *
   * \return \c false when text is not a valid value
   */
  template<anch::csv::Scalar V>
  bool parse(std::string_view text, V& value, const anch::csv::Options& options);

  /*!
   * Format field value
   *
   * \param value the value
   * \param options the CSV options (booleans texts)
   *
   * \return the field text
   */
  template<anch::csv::Scalar V>
  std::string format(const V& value, const anch::csv::Options& options);

  /*!
   * \brief Context of a field read by \ref anch::csv::ObjectReader (constraints violations raise \ref CsvError )
   */
  struct ReadContext {
    /*! Constraints are checked */
    bool enabled;

    /*! Field name */
    const std::string& name;

    /*! Line of the record */
    std::size_t line;

    /*! Column of the field (from 1) */
    std::size_t column;
  };

  /*!
   * \brief Context of a field written by \ref anch::csv::ObjectWriter (constraints violations raise \ref CsvError )
   */
  struct WriteContext {
    /*! Constraints are checked */
    bool enabled;

    /*! Field name */
    const std::string& name;

    /*! Written record (from 1, header included) */
    std::size_t line;

    /*! Column of the field (from 1) */
    std::size_t column;
  };

  /*! Field text written by \ref anch::csv::ObjectWriter : empty for an absent value */
  using FieldText = std::optional<std::string>;

}

namespace anch::val {

  /*!
   * \brief anch-val constraints checked when CSV fields are read ( \c Options::readCheckConstraints )
   */
  template<>
  struct CheckContext<anch::csv::detail::ReadContext> {
    /*! \return \c true when constraints are checked */
    static bool
    enabled(const anch::csv::detail::ReadContext& context) noexcept {
      return context.enabled;
    }

    /*!
     * Raise \c CONSTRAINT_VIOLATION
     *
     * \param context the context
     * \param message the message
     */
    [[noreturn]] static void
    violation(const anch::csv::detail::ReadContext& context, const std::string& message) {
      throw anch::csv::CsvError(anch::csv::ErrorCode::CONSTRAINT_VIOLATION, context.name + ": " + message, context.line, context.column);
    }

    /*! CSV fields are scalars: items constraints are not applicable */
    template<typename F>
    static void
    item(anch::csv::detail::ReadContext&, std::string_view, F&& check) {
      std::forward<F>(check)();
    }

    /*! CSV fields are scalars: items constraints are not applicable */
    template<typename F>
    static void
    item(anch::csv::detail::ReadContext&, std::size_t, F&& check) {
      std::forward<F>(check)();
    }
  };

  /*!
   * \brief anch-val constraints checked when CSV fields are written ( \c Options::writeCheckConstraints )
   */
  template<>
  struct CheckContext<anch::csv::detail::WriteContext> {
    /*! \return \c true when constraints are checked */
    static bool
    enabled(const anch::csv::detail::WriteContext& context) noexcept {
      return context.enabled;
    }

    /*!
     * Raise \c CONSTRAINT_VIOLATION
     *
     * \param context the context
     * \param message the message
     */
    [[noreturn]] static void
    violation(const anch::csv::detail::WriteContext& context, const std::string& message) {
      throw anch::csv::CsvError(anch::csv::ErrorCode::CONSTRAINT_VIOLATION, context.name + ": " + message, context.line, context.column);
    }

    /*! CSV fields are scalars: items constraints are not applicable */
    template<typename F>
    static void
    item(anch::csv::detail::WriteContext&, std::string_view, F&& check) {
      std::forward<F>(check)();
    }

    /*! CSV fields are scalars: items constraints are not applicable */
    template<typename F>
    static void
    item(anch::csv::detail::WriteContext&, std::size_t, F&& check) {
      std::forward<F>(check)();
    }
  };

}

namespace anch::csv {

  /*!
   * \brief CSV mapper of a described type ( \c anch::Mapping , anch-utils): fields are columns
   *
   * It satisfies the \c anch::ObjectMapper concept. Fields are \ref anch::csv::FieldType "scalars or optional
   * scalars" ; fields registered with a member or a getter are written, fields registered with a member or a setter
   * are read. Constraints are anch-val ones: \c anch::val::required makes the column required, value constraints are
   * checked according to \c Options::readCheckConstraints and \c Options::writeCheckConstraints .
   *
   * \tparam T the described type
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  template<typename T>
  class ObjectMapper {

  public:
    /*! \brief Field mapping */
    struct Field {
      /*! Field (column) name */
      std::string name;

      /*! Column is required */
      bool required;

      /*! Field reader (empty for fields without setter): text, quoted, context, options */
      std::function<void(T&, std::string_view, bool, anch::csv::detail::ReadContext&, const anch::csv::Options&)> read;

      /*! Field writer (empty for fields without getter) */
      std::function<anch::csv::detail::FieldText(const T&, anch::csv::detail::WriteContext&, const anch::csv::Options&)> write;
    };

    // Attributes +
  private:
    /*! Fields, in registration order */
    std::vector<Field> _fields;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref ObjectMapper constructor: fields are registered by \c anch::Mapping<T>
     */
    ObjectMapper();
    // Constructors -

    // Methods +
  public:
    /*!
     * Get the mapper of \c T (built on first use)
     *
     * \return the mapper
     */
    static const ObjectMapper<T>& instance();

    /*!
     * Fields getter
     *
     * \return the fields, in registration order
     */
    const std::vector<Field>& fields() const noexcept;

    /*!
     * Register field with its member (read and written)
     *
     * \param name the column name
     * \param member the member
     * \param constraints the constraints
     *
     * \return \c this
     */
    template<typename P, typename... C>
    requires (anch::val::FieldConstraint<C, P> && ...)
    ObjectMapper<T>& registerField(const std::string& name, P T::* member, C&&... constraints);

    /*!
     * Register field with its getter (written only)
     *
     * \param name the column name
     * \param getter the getter
     * \param constraints the constraints
     *
     * \return \c this
     */
    template<typename P, typename... C>
    requires (anch::val::FieldConstraint<C, P> && ...)
    ObjectMapper<T>& registerField(const std::string& name, std::function<P(const T&)> getter, C&&... constraints);

    /*!
     * Register field with its setter (read only)
     *
     * \param name the column name
     * \param setter the setter
     * \param constraints the constraints
     *
     * \return \c this
     */
    template<typename P, typename... C>
    requires (anch::val::FieldConstraint<C, P> && ...)
    ObjectMapper<T>& registerField(const std::string& name, std::function<void(T&, const P&)> setter, C&&... constraints);

    /*!
     * Register field with its getter and its setter
     *
     * \param name the column name
     * \param getter the getter
     * \param setter the setter
     * \param constraints the constraints
     *
     * \return \c this
     */
    template<typename P, typename... C>
    requires (anch::val::FieldConstraint<C, P> && ...)
    ObjectMapper<T>& registerField(const std::string& name, std::function<P(const T&)> getter,
				   std::function<void(T&, const P&)> setter, C&&... constraints);
    // Methods -

  private:
    /*!
     * Build field reader
     *
     * \param set the value setter
     * \param constraints the constraints
     *
     * \return the reader
     */
    template<typename P, typename S, typename... C>
    static auto reader(S set, C&&... constraints);

    /*!
     * Build field writer
     *
     * \param get the value getter
     * \param constraints the constraints
     *
     * \return the writer
     */
    template<typename P, typename G, typename... C>
    static auto writer(G get, C&&... constraints);

  };

  /*!
   * \brief Objects reader: records of a \ref Reader are read as \c T objects
   *
   * With a header, columns are associated with fields by name (columns which are not fields are ignored, or refused
   * with \c Options::ignoreUnknownColumns ; required fields columns have to be present: \c MISSING_COLUMN ) ;
   * without header, by position: each registered field is a column, in registration order (columns of fields which
   * are not read, getter only, are skipped), as \ref ObjectWriter writes them. An empty field is an absent value ( \c std::nullopt ),
   * an empty quoted field ( \c "" ) an empty string. Conversion errors ( \c CONVERSION ) and constraints violations
   * ( \c CONSTRAINT_VIOLATION ) give the line and the column of the field.
   * \snippet{trimleft} anch-csv/examples/csv.cpp objects reader
   *
   * \tparam T the described type
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  template<typename T>
  class ObjectReader {

    // Attributes +
  private:
    /*! Records reader */
    anch::csv::Reader& _reader;

    /*! Mapper */
    const anch::csv::ObjectMapper<T>& _mapper;

    /*! Column of each field ( \c NONE when it is not in the header) */
    std::vector<std::size_t> _columns;
    // Attributes -

  public:
    /*! No column */
    static constexpr std::size_t NONE = static_cast<std::size_t>(-1);

    // Constructors +
  public:
    /*!
     * \ref ObjectReader constructor: header columns are associated with fields
     *
     * \param reader the records reader
     *
     * \throw CsvError \c UNKNOWN_COLUMN or \c MISSING_COLUMN
     */
    explicit ObjectReader(anch::csv::Reader& reader);
    // Constructors -

    // Methods +
  public:
    /*!
     * Read next object
     *
     * \param value the object to fill (fields whose column is absent are not changed)
     *
     * \return \c false at the end of the input
     *
     * \throw CsvError invalid record, conversion error or constraint violation
     */
    bool read(T& value);
    // Methods -

  };

  /*!
   * \brief Objects writer: \c T objects are written as records of a \ref Writer
   *
   * The header (fields which have a value: member or getter) is written before the first object. Without header,
   * records are read by position: each registered field is a column, in registration order (fields without value,
   * setter only, are empty), as \ref ObjectReader reads them. Absent values
   * ( \c std::nullopt ) are empty fields, empty strings empty quoted fields ( \c "" ).
   * \snippet{trimleft} anch-csv/examples/csv.cpp objects writer
   *
   * \tparam T the described type
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  template<typename T>
  class ObjectWriter {

    // Attributes +
  private:
    /*! Records writer */
    anch::csv::Writer& _writer;

    /*! Mapper */
    const anch::csv::ObjectMapper<T>& _mapper;

    /*! Header has to be written before the first object */
    bool _header;

    /*! Records are written without header: each registered field is a column */
    bool _positional;

    /*! Written records (header included) */
    std::size_t _records;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref ObjectWriter constructor
     *
     * \param writer the records writer
     * \param header write the header before the first object (default \c true )
     */
    explicit ObjectWriter(anch::csv::Writer& writer, bool header = true);
    // Constructors -

    // Methods +
  public:
    /*!
     * Write object
     *
     * \param value the object
     *
     * \throw CsvError \c CONSTRAINT_VIOLATION (with \c Options::writeCheckConstraints )
     */
    void write(const T& value);

    /*!
     * Write the header when it has not been written yet (it is written before the first object): a header can be
     * written without object
     */
    void writeHeader();
    // Methods -

  };

}

#include "csv/impl/mapper.hpp"
