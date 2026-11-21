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

#include <cstddef>
#include <istream>
#include <iterator>
#include <ostream>
#include <ranges>
#include <vector>

#include "csv/mapper.hpp"
#include "csv/options.hpp"
#include "csv/reader.hpp"
#include "csv/writer.hpp"

namespace anch::csv {

  /*!
   * \brief Objects of a CSV input, read one by one (input range)
   *
   * The range is read once: each iteration reads the next record (the current object is valid until the next
   * iteration). Errors raise \ref CsvError while iterating. It is built by \ref anch::csv::records .
   * \snippet{trimleft} anch-csv/examples/csv.cpp records
   *
   * \tparam T the described type
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  template<typename T>
  class Records {

  public:
    /*! \brief Records iterator (input iterator) */
    class iterator {
    private:
      /*! Records */
      Records<T>* _records;

    public:
      /*! Value type */
      using value_type = T;
      /*! Difference type */
      using difference_type = std::ptrdiff_t;

      /*! Default constructor (end of range) */
      iterator() noexcept;

      /*!
       * Iterator constructor
       *
       * \param records the records
       */
      explicit iterator(Records<T>* records) noexcept;

      /*!
       * Current object
       *
       * \return the object
       */
      const T& operator*() const noexcept;

      /*!
       * Read next object
       *
       * \return \c this
       *
       * \throw CsvError invalid record
       */
      iterator& operator++();

      /*!
       * Read next object
       *
       * \throw CsvError invalid record
       */
      void operator++(int);

      /*!
       * Check end of range
       *
       * \return \c true at the end of the input
       */
      bool operator==(std::default_sentinel_t) const noexcept;
    };

    // Attributes +
  private:
    /*! Records reader */
    anch::csv::Reader _reader;

    /*! Objects reader */
    anch::csv::ObjectReader<T> _objects;

    /*! Current object */
    T _value;

    /*! End of input has been reached */
    bool _done;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Records constructor: the header is read
     *
     * \param input the input stream
     * \param options the options
     *
     * \throw CsvError invalid header, \c UNKNOWN_COLUMN , \c MISSING_COLUMN
     */
    Records(std::istream& input, const anch::csv::Options& options);

    /*! Forbids \ref Records copy (readers hold references on each other) */
    Records(const Records&) = delete;
    // Constructors -

    // Methods +
  public:
    /*!
     * Read the first object (the range is read once)
     *
     * \return the iterator on the first object
     *
     * \throw CsvError invalid record
     */
    iterator begin();

    /*!
     * End of range
     *
     * \return the sentinel
     */
    std::default_sentinel_t end() const noexcept;
    // Methods -

  private:
    /*! Read next object */
    void next();

  };

  /*!
   * Objects of a CSV input, read one by one without loading the whole input:
   * <tt>for(const Pet& pet : anch::csv::records<Pet>(input))</tt>
   *
   * \tparam T the described type
   *
   * \param input the input stream
   * \param options the options
   *
   * \return the records (input range)
   *
   * \throw CsvError invalid header, \c UNKNOWN_COLUMN , \c MISSING_COLUMN
   *
   * \since 0.3
   */
  template<typename T>
  anch::csv::Records<T> records(std::istream& input, const anch::csv::Options& options = anch::csv::Options());

  /*!
   * Read every object of a CSV input (small volumes, see \ref records for large ones)
   *
   * \tparam T the described type
   *
   * \param values the objects (replaced)
   * \param input the input stream
   * \param options the options
   *
   * \throw CsvError invalid record, conversion error or constraint violation
   *
   * \since 0.3
   */
  template<typename T>
  void deserialize(std::vector<T>& values, std::istream& input, const anch::csv::Options& options = anch::csv::Options());

  /*!
   * Write objects: header (with \c Options::header , also when there is no object), then one record per object
   *
   * \tparam R the range type (its values are described objects)
   *
   * \param values the objects
   * \param out the output stream
   * \param options the options
   *
   * \throw CsvError constraint violation (with \c Options::writeCheckConstraints )
   *
   * \since 0.3
   */
  template<std::ranges::input_range R>
  void serialize(const R& values, std::ostream& out, const anch::csv::Options& options = anch::csv::Options());

}

#include "csv/impl/csv.hpp"
