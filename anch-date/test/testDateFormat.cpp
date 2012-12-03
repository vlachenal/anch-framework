#include <iostream>
#include <sstream>
#include <iomanip>

#include "date/dateFormatter.hpp"
#include "date/formatter/iDatePartFormatter.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::ostream;
using std::istringstream;
using std::setfill;
using std::setw;

using anch::date::Date;
using anch::date::DateFormatter;
using anch::date::formatter::IDatePartFormatter;


class MicroFormatter : public IDatePartFormatter {
public:
  static const string PATTERN;
  MicroFormatter() {}
  virtual ~MicroFormatter() {}
  void format(const Date& date, ostream& output) const noexcept {
    output << setfill('0') << setw(3) << getMicrosecond(date);
  }
  size_t getSize() const noexcept {
    return 3;
  }
  bool setValue(Date& date, const string& value) const noexcept {
    istringstream iss(value);
    uint16_t val;
    iss >> std::dec >> val;
    if(iss.fail()) {
      return false;
    } else {
      setMicrosecond(date, val);
      return true;
    }
  }
  const string& getPattern() const noexcept {
    return MicroFormatter::PATTERN;
  }
  static IDatePartFormatter* getInstance() {
    return new MicroFormatter();
  }
};
const string MicroFormatter::PATTERN = "%v";

class NanoFormatter : public IDatePartFormatter {
public:
  static const string PATTERN;
  NanoFormatter() {}
  virtual ~NanoFormatter() {}
  void format(const Date& date, ostream& output) const noexcept {
    output << setfill('0') << setw(3) << getNanosecond(date);
  }
  size_t getSize() const noexcept {
    return 3;
  }
  bool setValue(Date& date, const string& value) const noexcept {
    istringstream iss(value);
    uint16_t val;
    iss >> std::dec >> val;
    if(iss.fail()) {
      return false;
    } else {
      setNanosecond(date, val);
      return true;
    }
  }
  const string& getPattern() const noexcept {
    return NanoFormatter::PATTERN;
  }
  static IDatePartFormatter* getInstance() {
    return new NanoFormatter();
  }
};
const string NanoFormatter::PATTERN = "%N";


int
main(void) {
  cout << "Enter in test date format" << endl;

  DateFormatter formatter("%Y-%m-%d %H:%M:%S.%s");
  string nowStr;
  Date now = Date();
  formatter.format(now, nowStr);
  cout << "Date from scratch: " << nowStr << endl;

  string timetStr;
  formatter.format(Date(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), timetStr);
  cout << "Date from time_t: " << timetStr << endl;

  cout << "Exit test date format" << endl;


  cout << "Enter in test date parse" << endl;

  string strDate = "2012-10-23 10:02:59.998";
  Date parsedDate(false);
  formatter.parse(strDate, parsedDate);
  string parsedDateStr;
  formatter.format(parsedDate, parsedDateStr);
  if(strDate != parsedDateStr) {
    cerr << "Retrieve " << parsedDateStr << " instead of " << strDate << endl;
    return 1;
  } else {
    cout << "Parsed date: " << parsedDateStr << endl;
  }

  cout << "Exit test date parse" << endl;


  cout << "Enter in test comparison" << endl;

  if(now <= parsedDate) {
    cerr << nowStr << " should be after " << parsedDateStr << endl;
    return 2;
  } else {
    cout << nowStr << " is after " << parsedDateStr << endl;
  }

  cout << "Exit test comparison" << endl;

  cout << "Enter in new formatter test" << endl;
  DateFormatter::registerFormatterPart(NanoFormatter::PATTERN, &NanoFormatter::getInstance);
  DateFormatter::registerFormatterPart(MicroFormatter::PATTERN, &MicroFormatter::getInstance);
  DateFormatter formatter2("%Y-%m-%d %H:%M:%S.%s.%v.%N");
  formatter2.format(now, nowStr);
  cout << "Date from scratch: " << nowStr << endl;
  cout << "Exit new formatter test" << endl;

  return 0;
}
