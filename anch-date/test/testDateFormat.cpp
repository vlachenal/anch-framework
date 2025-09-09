#include <iostream>
#include <sstream>
#include <iomanip>

#include "date/dateFormatter.hpp"
#include "date/fmt/iDatePartFormatter.hpp"

using anch::date::Date;
using anch::date::DateFormatter;
using anch::date::IDatePartFormatter;


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
const std::string NanoFormatter::PATTERN = "%N";


int
main(void) {
  std::cout << "Enter in test date format" << std::endl;

  DateFormatter formatter("%Y-%m-%d %H:%M:%S.%s");
  std::string nowStr;
  Date now = Date();
  formatter.format(now, nowStr);
  std::cout << "Date from scratch: " << nowStr << std::endl;

  std::string timetStr;
  formatter.format(Date(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), timetStr);
  std::cout << "Date from time_t: " << timetStr << std::endl;

  std::cout << "Exit test date format" << std::endl;


  std::cout << "Enter in test date parse" << std::endl;

  std::string strDate = "2012-10-23 10:02:59.998";
  Date parsedDate(false);
  formatter.parse(strDate, parsedDate);
  std::string parsedDateStr;
  formatter.format(parsedDate, parsedDateStr);
  if(strDate != parsedDateStr) {
    std::cerr << "Retrieve " << parsedDateStr << " instead of " << strDate << std::endl;
    return 1;
  } else {
    std::cout << "Parsed date: " << parsedDateStr << std::endl;
  }

  std::cout << "Exit test date parse" << std::endl;


  std::cout << "Enter in test comparison" << std::endl;

  if(now <= parsedDate) {
    std::cerr << nowStr << " should be after " << parsedDateStr << std::endl;
    return 2;
  } else {
    std::cout << nowStr << " is after " << parsedDateStr << std::endl;
  }

  std::cout << "Exit test comparison" << std::endl;

  std::cout << "Enter in new formatter test" << std::endl;
  DateFormatter::registerFormatterPart(NanoFormatter::PATTERN, &NanoFormatter::getInstance);
  DateFormatter::registerFormatterPart(MicroFormatter::PATTERN, &MicroFormatter::getInstance);
  DateFormatter formatter2("%Y-%m-%d %H:%M:%S.%s.%v.%N");
  formatter2.format(now, nowStr);
  std::cout << "Date from scratch: " << nowStr << std::endl;
  std::cout << "Exit new formatter test" << std::endl;

  return 0;
}
