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
#include "date/dateFormatter.hpp"

#include <array>
#include <charconv>
#include <format>
#include <iterator>
#include <map>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string_view>

#include "date/fmt/iDatePartFormatter.hpp"

using anch::date::Date;
using anch::date::DateFormatter;
using anch::date::IDatePartFormatter;
using anch::date::getInstance;

namespace {

  /*! Date fields managed by \c FieldFormatter */
  enum class Field {
    YEAR4 = 0,
    YEAR2,
    MONTH,
    DAY,
    HOUR24,
    HOUR12,
    MINUTE,
    SECOND,
    MILLISECOND
  };

  /*!
   * \brief Field description
   */
  struct FieldDesc {
    /*! Pattern */
    std::string pattern;
    /*! Number of digits */
    std::size_t size;
    /*! Minimum parsed value */
    int min;
    /*! Maximum parsed value */
    int max;
  };

  /*! Fields descriptions (indexed by \c Field ) */
  const std::array<FieldDesc,9> FIELDS = {{
      {"%Y", 4, 0, 9999},
      {"%y", 2, 0, 99},
      {"%m", 2, 1, 12},
      {"%d", 2, 1, 31},
      {"%H", 2, 0, 23},
      {"%h", 2, 1, 12},
      {"%M", 2, 0, 59},
      {"%S", 2, 0, 60},
      {"%s", 3, 0, 999}
    }};

  /*!
   * \brief Numeric date field formatter
   *
   * Formats and parses a fixed size numeric field.
   */
  class FieldFormatter: public IDatePartFormatter {
  private:
    /*! Field */
    Field _field;

  public:
    /*!
     * \c FieldFormatter constructor
     *
     * \param field the field
     */
    explicit FieldFormatter(Field field): _field(field) {
      // Nothing to do
    }

    /*!
     * Field description getter
     *
     * \return the description
     */
    const FieldDesc& desc() const noexcept {
      return FIELDS[static_cast<std::size_t>(_field)];
    }

    virtual void format(const Date& date, std::ostream& output) const noexcept override {
      int value = 0;
      switch(_field) {
      case Field::YEAR4: value = getYear(date); break;
      case Field::YEAR2: value = ((getYear(date) % 100) + 100) % 100; break;
      case Field::MONTH: value = getMonth(date) + 1; break;
      case Field::DAY: value = getDay(date); break;
      case Field::HOUR24: value = getHour(date); break;
      case Field::HOUR12: value = getHour(date) % 12 == 0 ? 12 : getHour(date) % 12; break;
      case Field::MINUTE: value = getMinute(date); break;
      case Field::SECOND: value = getSecond(date); break;
      case Field::MILLISECOND: value = getMillisecond(date); break;
      }
      // std::format does not change stream state (fill character)
      std::format_to(std::ostreambuf_iterator<char>(output), "{:0{}}", value, desc().size);
    }

    virtual std::size_t getSize() const noexcept override {
      return desc().size;
    }

    virtual bool setValue(Date& date, const std::string& str) const noexcept override {
      int value = 0;
      const char* last = str.data() + str.size();
      if(str.empty() || str.front() < '0' || str.front() > '9') { // from_chars accepts '-'
	return false;
      }
      auto res = std::from_chars(str.data(), last, value);
      if(res.ec != std::errc() || res.ptr != last || value < desc().min || value > desc().max) {
	return false;
      }
      switch(_field) {
      case Field::YEAR4: setYear(date, value); break;
      case Field::YEAR2: setYear(date, value < 69 ? 2000 + value : 1900 + value); break; // as strptime
      case Field::MONTH: setMonth(date, static_cast<uint16_t>(value - 1)); break;
      case Field::DAY: setDay(date, static_cast<uint16_t>(value)); break;
      case Field::HOUR24: setHour(date, static_cast<uint16_t>(value)); break;
      case Field::HOUR12: setHour(date, static_cast<uint16_t>(getHour(date) + value % 12)); break; // marker adds 12
      case Field::MINUTE: setMinute(date, static_cast<uint16_t>(value)); break;
      case Field::SECOND: setSecond(date, static_cast<uint16_t>(value)); break;
      case Field::MILLISECOND: setMillisecond(date, static_cast<uint16_t>(value)); break;
      }
      return true;
    }

    virtual const std::string& getPattern() const noexcept override {
      return desc().pattern;
    }
  };

  /*!
   * \brief a.m./p.m. marker formatter
   */
  class MarkerFormatter: public IDatePartFormatter {
  public:
    /*! Pattern */
    static const std::string PATTERN;

    virtual void format(const Date& date, std::ostream& output) const noexcept override {
      output << (getHour(date) < 12 ? "a.m." : "p.m.");
    }

    virtual std::size_t getSize() const noexcept override {
      return 4;
    }

    virtual bool setValue(Date& date, const std::string& value) const noexcept override {
      if(value == "p.m.") {
	if(getHour(date) < 12) { // %h value is added to (or by) marker value
	  setHour(date, static_cast<uint16_t>(getHour(date) + 12));
	}
	return true;
      }
      return value == "a.m.";
    }

    virtual const std::string& getPattern() const noexcept override {
      return PATTERN;
    }
  };
  const std::string MarkerFormatter::PATTERN = "%p";

  /*!
   * \brief Constant string formatter
   */
  class ConstantFormatter: public IDatePartFormatter {
  private:
    /*! Constant string */
    std::string _message;

  public:
    /*!
     * \c ConstantFormatter constructor
     *
     * \param message the constant string
     */
    explicit ConstantFormatter(const std::string& message): _message(message) {
      // Nothing to do
    }

    virtual void format(const Date&, std::ostream& output) const noexcept override {
      output << _message;
    }

    virtual std::size_t getSize() const noexcept override {
      return _message.size();
    }

    virtual bool setValue(Date&, const std::string& value) const noexcept override {
      return value == _message;
    }

    virtual const std::string& getPattern() const noexcept override {
      return _message;
    }
  };

  /*!
   * Built-in field formatter instance getter
   *
   * \tparam F the field
   *
   * \return the new instance
   */
  template<Field F>
  IDatePartFormatter*
  newField() {
    return new FieldFormatter(F);
  }

  /*!
   * Marker formatter instance getter
   *
   * \return the new instance
   */
  IDatePartFormatter*
  newMarker() {
    return new MarkerFormatter();
  }

  /*! Registered formatters mutex */
  std::mutex MUTEX;

  /*!
   * Registered formatters getter (\c MUTEX has to be locked)
   *
   * \return the formatters by pattern
   */
  std::map<std::string, getInstance>&
  getFormatters() {
    static std::map<std::string, getInstance> formatters = {
      { FIELDS[static_cast<std::size_t>(Field::YEAR4)].pattern, &newField<Field::YEAR4> },
      { FIELDS[static_cast<std::size_t>(Field::YEAR2)].pattern, &newField<Field::YEAR2> },
      { FIELDS[static_cast<std::size_t>(Field::MONTH)].pattern, &newField<Field::MONTH> },
      { FIELDS[static_cast<std::size_t>(Field::DAY)].pattern, &newField<Field::DAY> },
      { FIELDS[static_cast<std::size_t>(Field::HOUR24)].pattern, &newField<Field::HOUR24> },
      { FIELDS[static_cast<std::size_t>(Field::HOUR12)].pattern, &newField<Field::HOUR12> },
      { FIELDS[static_cast<std::size_t>(Field::MINUTE)].pattern, &newField<Field::MINUTE> },
      { FIELDS[static_cast<std::size_t>(Field::SECOND)].pattern, &newField<Field::SECOND> },
      { FIELDS[static_cast<std::size_t>(Field::MILLISECOND)].pattern, &newField<Field::MILLISECOND> },
      { MarkerFormatter::PATTERN, &newMarker }
    };
    return formatters;
  }

}

// Constructors +
DateFormatter::DateFormatter(const std::string& dateFormat): _formatters(), _size(0) {
  std::string constant;
  std::size_t idx = 0;
  while(idx < dateFormat.size()) {
    if(dateFormat[idx] == '%' && idx + 1 < dateFormat.size()) {
      std::string pattern = dateFormat.substr(idx, 2);
      getInstance getter = NULL;
      {
	std::lock_guard<std::mutex> lock(MUTEX);
	auto iter = getFormatters().find(pattern);
	if(iter != getFormatters().end()) {
	  getter = iter->second;
	}
      }
      if(getter == NULL) { // unknown pattern is a constant string
	constant += pattern;
      } else {
	if(!constant.empty()) {
	  addFormatter(new ConstantFormatter(constant));
	  constant.clear();
	}
	addFormatter(getter());
      }
      idx += 2;
    } else {
      constant += dateFormat[idx];
      ++idx;
    }
  }
  if(!constant.empty()) {
    addFormatter(new ConstantFormatter(constant));
  }
}
// Constructors -

// Destructor +
DateFormatter::~DateFormatter() noexcept {
  // Nothing to do
}
// Destructor -

// Methods +
void
DateFormatter::registerFormatterPart(const std::string& pattern, getInstance instGetter) noexcept {
  std::lock_guard<std::mutex> lock(MUTEX);
  getFormatters().try_emplace(pattern, instGetter);
}

void
DateFormatter::format(const Date& date, std::string& output) const noexcept {
  std::ostringstream oss;
  format(date, oss);
  output = oss.str();
}

void
DateFormatter::format(const Date& date, std::ostream& output) const noexcept {
  for(const auto& part : _formatters) {
    part->format(date, output);
  }
}

void
DateFormatter::parse(const std::string& strDate, Date& date) const {
  if(strDate.size() != _size) {
    std::ostringstream oss;
    oss << "Found " << strDate.size() << " characters instead of " << _size << " in " << strDate;
    throw std::invalid_argument(oss.str());
  }
  // Fields which are not in pattern: 1970-01-01 00:00:00.000 +
  date._years = 1970;
  date._months = 0;
  date._mdays = 1;
  date._hours = 0;
  date._minutes = 0;
  date._seconds = 0;
  date._milliseconds = 0;
  date._microseconds = 0;
  date._nanoseconds = 0;
  // Fields which are not in pattern: 1970-01-01 00:00:00.000 -
  std::size_t offset = 0;
  for(const auto& part : _formatters) {
    if(!part->setValue(date, strDate.substr(offset, part->getSize()))) {
      throw std::invalid_argument("Fail to parse " + strDate);
    }
    offset += part->getSize();
  }
  std::chrono::year_month_day ymd{std::chrono::year(date._years),
				  std::chrono::month(static_cast<unsigned>(date._months + 1)),
				  std::chrono::day(date._mdays)};
  if(!ymd.ok()) {
    throw std::invalid_argument("Invalid date " + strDate);
  }
  date.computeTimestamp();
}

Date
DateFormatter::parse(const std::string& strDate) const {
  Date date(false);
  parse(strDate, date);
  return date;
}

void
DateFormatter::addFormatter(IDatePartFormatter* formatter) {
  _formatters.emplace_back(formatter);
  _size += formatter->getSize();
}
// Methods -
