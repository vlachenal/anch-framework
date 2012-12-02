#include <iostream>

#include "date/dateFormatter.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

using anch::date::Date;
using anch::date::DateFormatter;


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

  return 0;
}
