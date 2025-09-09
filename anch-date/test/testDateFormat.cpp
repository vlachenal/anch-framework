#include <iostream>
#include <sstream>
#include <iomanip>

#include "date/dateFormatter.hpp"
#include "date/fmt/iDatePartFormatter.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::date::Date;
using anch::date::DateFormatter;
using anch::date::IDatePartFormatter;


// Custom formatter +
class MicroFormatter : public IDatePartFormatter {
public:
  static const std::string PATTERN;
  MicroFormatter() {}
  virtual ~MicroFormatter() {}
  void format(const Date& date, std::ostream& output) const noexcept {
    output << std::setfill('0') << std::setw(3) << getMicrosecond(date);
  }
  size_t getSize() const noexcept {
    return 3;
  }
  bool setValue(Date& date, const std::string& value) const noexcept {
    std::istringstream iss(value);
    uint16_t val;
    iss >> std::dec >> val;
    if(iss.fail()) {
      return false;
    } else {
      setMicrosecond(date, val);
      return true;
    }
  }
  const std::string& getPattern() const noexcept {
    return MicroFormatter::PATTERN;
  }
  static IDatePartFormatter* getInstance() {
    return new MicroFormatter();
  }
};
const std::string MicroFormatter::PATTERN = "%v";

class NanoFormatter : public IDatePartFormatter {
public:
  static const std::string PATTERN;
  NanoFormatter() {}
  virtual ~NanoFormatter() {}
  void format(const Date& date, std::ostream& output) const noexcept {
    output << std::setfill('0') << std::setw(3) << getNanosecond(date);
  }
  size_t getSize() const noexcept {
    return 3;
  }
  bool setValue(Date& date, const std::string& value) const noexcept {
    std::istringstream iss(value);
    uint16_t val;
    iss >> std::dec >> val;
    if(iss.fail()) {
      return false;
    } else {
      setNanosecond(date, val);
      return true;
    }
  }
  const std::string& getPattern() const noexcept {
    return NanoFormatter::PATTERN;
  }
  static IDatePartFormatter* getInstance() {
    return new NanoFormatter();
  }
};
// Custom formatter -

const std::string NanoFormatter::PATTERN = "%N";
DateFormatter formatter("%Y-%m-%d %H:%M:%S.%s");

void
testFormatDate() {
  std::cout << "Enter in test date format" << std::endl;

  std::string nowStr;
  Date now = Date();
  formatter.format(now, nowStr);
  std::cout << "Date from scratch: " << nowStr << std::endl;

  std::cout << "Exit test date format" << std::endl;
}

void
testFormatDateTimeT() {
  std::cout << "Enter in test date format from std::time_t" << std::endl;

  std::string timetStr;
  formatter.format(Date(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), timetStr);
  std::cout << "Date from time_t: " << timetStr << std::endl;

  std::cout << "Exit test date format from std::time_t" << std::endl;
}

Date
parseDate(const std::string& strDate) {
  Date parsedDate(false);
  formatter.parse(strDate, parsedDate);
  return parsedDate;
}

void
testParseDate() {
  std::cout << "Enter in test date parse" << std::endl;

  std::string strDate = "2012-10-23 10:02:59.998";
  Date parsedDate = parseDate(strDate);
  std::string parsedDateStr;
  formatter.format(parsedDate, parsedDateStr);
  anch::ut::assert(strDate == parsedDateStr, "Parse {} instrad of {}", parsedDateStr, strDate);
  std::cout << "Parsed date: " << parsedDateStr << std::endl;

  std::cout << "Exit test date parse" << std::endl;
}

void
testComparison() {
  std::cout << "Enter in test comparison" << std::endl;

  Date now = Date();
  std::string nowStr;
  formatter.format(now, nowStr);
  std::string strDate = "2012-10-23 10:02:59.998";
  Date parsedDate = parseDate(strDate);
  anch::ut::assert(now > parsedDate, "{} should be after {}", nowStr, strDate);
  std::cout << nowStr << " is after " << strDate << std::endl;

  std::cout << "Exit test comparison" << std::endl;
}

void
testFormatDateCustom() {
  std::cout << "Enter in custom formatter test" << std::endl;

  std::string nowStr;
  Date now = Date();
  DateFormatter::registerFormatterPart(NanoFormatter::PATTERN, &NanoFormatter::getInstance);
  DateFormatter::registerFormatterPart(MicroFormatter::PATTERN, &MicroFormatter::getInstance);
  DateFormatter formatter2("%Y-%m-%d %H:%M:%S.%s.%v.%N");
  formatter2.format(now, nowStr);
  std::cout << "Date from custom parser: " << nowStr << std::endl;

  std::cout << "Exit custom formatter test" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH date unit tests")
    .description("Test AnCH date library")
    .add("date-format", testFormatDate)
    .add("date-format-time_t", testFormatDateTimeT)
    .add("date-parse", testParseDate)
    .add("date-comparison", testComparison)
    .add("date-format-custom", testFormatDateCustom)
    ;
}
