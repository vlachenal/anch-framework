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
#ifndef _ANCH_DATE_DATE_H_
#define _ANCH_DATE_DATE_H_

#include <memory>
#include <mutex>
#include <chrono>
#include <ctime>


namespace anch {
  namespace date {

    class DateFormatter;
    namespace formatter {
      class IDatePartFormatter;
    }

    /*!
     * Date and time utility class.
     *
     * The class implementation is thread safe (when using this class).\n
     * POSIX time management is not thread safe. This class uses POSIX implementation.\n
     * Be aware that if you use the POSIX API, you will loose the thread safe support
     * provided in this implementation.
     *
     * \author Vincent Lachenal
     */
    class Date {

      friend class formatter::IDatePartFormatter;
      friend class DateFormatter;

      // Attributes +
    private:
      /*! Mutex for thread-safe implementation */
      static std::mutex _mutex;

      /*! The timestamp */
      std::int64_t _timestamp;

      /*! The years */
      int32_t _years = 0;

      /*! The months (0-11 => 4 bits) */
      uint16_t _months = 0;

      /*! The days of the year (0-365 => 9 bits) */
      uint16_t _ydays = 0;

      /*! The days of the year (1-31 => 5 bits) */
      uint16_t _mdays = 0;

      /*! The days of the week (0-6 => 3 bits) */
      uint16_t _wdays = 0;

      /*! The minutes (0-23 => 5 bits) */
      uint16_t _hours = 0;

      /*! The minutes (0-59 => 6 bits) */
      uint16_t _minutes = 0;

      /*! The seconds (0-60 => 6 bits) */
      uint16_t _seconds = 0;

      /*! The milliseconds (0-999 => 10 bits) */
      uint16_t _milliseconds = 0;

      /*! The microseconds (0-999 => 10 bits) */
      uint16_t _microseconds = 0;

      /*! The nanoseconds (0-999 => 10 bits) */
      uint16_t _nanoseconds = 0;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref Date default constructor.
       *
       * \param init Initialize the \ref Date to now (default true)
       */
      Date(bool init = true);

      /*!
       * \ref Date private copy constructor
       *
       * \param date The \ref Date to copy
       */
      Date(const Date& date);

      /*!
       * \ref Date constructor.
       *
       * \param time The time to set
       */
      Date(const std::time_t& time);

      /*!
       * \ref Date constructor.
       *
       * \param time The time to set
       */
      Date(const std::tm* const time);
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref Date destructor
       */
      virtual ~Date();
      // Destructor -

      // Methods +
    private:
      /*!
       * Intialize ms, µs and ns date fields
       *
       * \param duration The duration
       */
      template<typename R, typename P>
      void initialize(const std::chrono::duration<R,P>& duration) noexcept {
	_nanoseconds = static_cast<uint16_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() % 1000);
	_microseconds = static_cast<uint16_t>(std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000);
	_milliseconds = static_cast<uint16_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000);
      }

      /*!
       * Initialize date fields
       *
       * \param time The current time
       */
      void initialize(const std::tm* const time);

      /*!
       * Compute timestamp from internal members
       */
      void computeTimestamp();

      /*!
       * Compute std::tm from internal members
       */
      inline void computeTm(std::tm& time) const {
	time.tm_sec = _seconds;
	time.tm_min = _minutes;
	time.tm_hour = _hours;
	time.tm_mday = _mdays;
	time.tm_mon = _months;
	time.tm_year = _years - 1900;
      }

    public:
      /*!
       * Check if current \ref Date is after another one.
       *
       * \param date The other \ref Date
       *
       * \return \c true if current \ref Date is after the other one, \c false otherwise.
       */
      inline bool after(const Date& date) const noexcept {
	return _timestamp > date._timestamp;
      }

      /*!
       * Check if current \ref Date is before another one.
       *
       * \param date The other \ref Date
       *
       * \return \c true if current \ref Date is before the other one, \c false otherwise.
       */
      inline bool before(const Date& date) const noexcept {
	return _timestamp < date._timestamp;
      }

      /*!
       * Check if current \ref Date is equals to another one.
       *
       * \param date The other \ref Date
       *
       * \return \c true if current \ref Date is equals to the other one, \c false otherwise.
       */
      inline bool equals(const Date& date) const noexcept {
	return _timestamp == date._timestamp;
      }
      // Methods -


      // Operators +
    public:
      /*!
       * '>' operator definition.
       * Calls after method
       *
       * \param date The date to compare to
       */
      inline bool operator > (const Date& date) const noexcept {
	return after(date);
      }

      /*!
       * '>=' operator definition.
       * Calls after and equals methods
       *
       * \param date The date to compare to
       */
      inline bool operator >= (const Date& date) const noexcept {
	return (after(date) || equals(date));
      }

      /*!
       * '<' operator definition.
       * Calls before method
       *
       * \param date The date to compare to
       */
      inline bool operator < (const Date& date) const noexcept {
	return before(date);
      }

      /*!
       * '<=' operator definition.
       * Calls before and equals methods
       *
       * \param date The date to compare to
       */
      inline bool operator <= (const Date& date) const noexcept {
	return (before(date) || equals(date));
      }

      /*!
       * '==' operator definition.
       * Calls equals method
       *
       * \param date The date to compare to
       */
      inline bool operator == (const Date& date) const noexcept {
	return equals(date);
      }

      /*!
       * '!=' operator definition.
       * Calls equals method
       *
       * \param date The date to compare to
       */
      inline bool operator != (const Date& date) const noexcept {
	return !equals(date);
      }

      /*!
       * Cast operator to std::time_t definition.
       *
       * \return The converted std::time_t
       */
      inline operator std::time_t() const noexcept {
	std::tm time;
	computeTm(time);
	return std::mktime(&time);
      }

      /*!
       * Cast operator to std::tm definition.
       *
       * \return The converted std::tm
       */
      inline operator std::tm() const noexcept {
	std::tm time;
	computeTm(time);
	return time;
      }
      // Operators -

    };

  }
}

#endif // _ANCH_DATE_DATE_H_
