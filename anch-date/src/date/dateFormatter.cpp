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

#include <ostream>
#include <iomanip>
#include <sstream>

#include "date/formatter/constantFormatter.hpp"
#include "date/formatter/dayFormatter.hpp"
#include "date/formatter/hour12Formatter.hpp"
#include "date/formatter/hour24Formatter.hpp"
#include "date/formatter/iDatePartFormatter.hpp"
#include "date/formatter/markerFormatter.hpp"
#include "date/formatter/millisecondFormatter.hpp"
#include "date/formatter/minuteFormatter.hpp"
#include "date/formatter/monthFormatter.hpp"
#include "date/formatter/secondFormatter.hpp"
#include "date/formatter/year2dFormatter.hpp"
#include "date/formatter/year4dFormatter.hpp"

using std::string;
using std::ostream;
using std::setfill;
using std::setw;
using std::istringstream;
using std::ostringstream;
using std::map;
using std::pair;

using boost::regex;
using boost::smatch;
using boost::regex_search;

using anch::date::Date;
using anch::date::DateFormatter;
using anch::date::getInstance;

using namespace anch::date::formatter;


// Static initialization +
const regex DateFormatter::DATE_PATTERN = regex(R"(^((%.)|([^%]+)))");

map<string, getInstance> DateFormatter::FORMATTERS = {
  {Year4DFormatter::PATTERN, &Year4DFormatter::getInstance},
  {Year2DFormatter::PATTERN, &Year2DFormatter::getInstance},
  {MonthFormatter::PATTERN, &MonthFormatter::getInstance},
  {DayFormatter::PATTERN, &DayFormatter::getInstance},
  {Hour24Formatter::PATTERN, &Hour24Formatter::getInstance},
  {Hour12Formatter::PATTERN, &Hour12Formatter::getInstance},
  {MarkerFormatter::PATTERN, &MarkerFormatter::getInstance},
  {MinuteFormatter::PATTERN, &MinuteFormatter::getInstance},
  {SecondFormatter::PATTERN, &SecondFormatter::getInstance},
  {MillisecondFormatter::PATTERN, &MillisecondFormatter::getInstance}
};
// Static initialization -


// Constructors +
/*!
 * \ref DateFormatter constructor
 *
 * \param dateFormat The date format
 */
DateFormatter::DateFormatter(const string& dateFormat) {
  string line = dateFormat;
  _size = 0;
  bool ok = true;
  smatch match;
  while(!line.empty() && ok) {
    if(regex_search(line, match, DATE_PATTERN)) {
      addFormatter(string(match[0].first, match[0].second));
      line = string(match.suffix().first, match.suffix().second);

    } else {
      ok = false;
    }
  }
  _formatters.shrink_to_fit();
}
// Constructors -

// Destructor +
/*!
 * \ref DateFormatter destructor
 */
DateFormatter::~DateFormatter() {
  for(const IDatePartFormatter* part : _formatters) {
    delete part;
  }
}
// Destructor -


// Methods +
/*!
 * Register a new formatter part
 *
 * \param pattern The formatter part pattern
 * \param instGetter The formatter part new instance getter
 */
void
DateFormatter::registerFormatterPart(const string& pattern,
				     getInstance instGetter) {
  if(FORMATTERS.find(pattern) == FORMATTERS.end()) {
    FORMATTERS.insert(pair<string, getInstance>(pattern, instGetter));
    FORMATTERS[pattern] = instGetter;
  }
}

/*!
 * Format date
 *
 * \param date The date to format
 * \param output The output string
 */
void
DateFormatter::format(const Date& date, string& output) const {
  ostringstream oss;
  format(date, oss);
  output.clear();
  output = oss.str();
}

/*!
 * Format date.
 *
 * \param date The date to format
 * \param output The output string
 */
void
DateFormatter::format(const Date& date, ostream& output) const {
  for(const IDatePartFormatter* part : _formatters) {
    part->format(date, output);
  }
  output.flush();
}

/*!
 * Build date from string.\n
 * You can build the output \ref Date with Date(false) constructor.
 *
 * \param strDate String formatted date
 * \param date The output date
 */
void
DateFormatter::parse(const string& strDate, Date& date) const {
  if(strDate.size() != _size) {
    // TODO throw exception
  }
  size_t offset = 0;
  bool ok = false;
  for(const IDatePartFormatter* part : _formatters) {
    ok = part->setValue(date, strDate.substr(offset, part->getSize()));
    if(!ok) {
      // TODO throw an exception
    }
    offset += part->getSize();
  }
  date.computeTimestamp();
}

/*!
 * Build date from string.\n
 * You had to delete \ref Date to not make memory leaks.
 *
 * \param strDate String formatted date
 *
 * \return A date
 */
Date*
DateFormatter::parse(const std::string& strDate) const {
  Date* date = new Date(false);
  parse(strDate, *date);
  return date;
}

/*!
 * Add formatter to formatter list
 *
 * \param strFormatter The formatter part to instanciate
 */
void
DateFormatter::addFormatter(const string& strFormatter) {
  IDatePartFormatter* form;
  if(strFormatter.length() == 1) {
    form = new ConstantFormatter(strFormatter);

  } else {
    const string pattern = strFormatter.substr(0,2);
    auto iter = FORMATTERS.find(pattern);
    if(iter == FORMATTERS.end()) {
      form = new ConstantFormatter(strFormatter);
    } else {
      form = (*(iter->second))();
    }

  }
  _formatters.push_back(form);
  _size += form->getSize();
}
// Methods -
