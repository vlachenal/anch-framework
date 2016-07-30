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

#ifdef ANCH_BOOST_REGEX
using boost::regex;
using boost::smatch;
using boost::regex_search;
#else
using std::regex;
using std::smatch;
using std::regex_search;
#endif

using anch::date::Date;
using anch::date::DateFormatter;
using anch::date::getInstance;


// Static initialization +
const regex DateFormatter::DATE_PATTERN = regex(R"(^((%.)|([^%]+)))");
// Static initialization -

std::map<std::string, getInstance>&
getFormatters() {
  static std::map<std::string, getInstance> formatters = {
    { anch::date::formatter::Year4DFormatter::PATTERN, &anch::date::formatter::Year4DFormatter::getInstance },
    { anch::date::formatter::Year2DFormatter::PATTERN, &anch::date::formatter::Year2DFormatter::getInstance },
    { anch::date::formatter::MonthFormatter::PATTERN, &anch::date::formatter::MonthFormatter::getInstance },
    { anch::date::formatter::DayFormatter::PATTERN, &anch::date::formatter::DayFormatter::getInstance },
    { anch::date::formatter::Hour24Formatter::PATTERN, &anch::date::formatter::Hour24Formatter::getInstance },
    { anch::date::formatter::Hour12Formatter::PATTERN, &anch::date::formatter::Hour12Formatter::getInstance },
    { anch::date::formatter::MarkerFormatter::PATTERN, &anch::date::formatter::MarkerFormatter::getInstance },
    { anch::date::formatter::MinuteFormatter::PATTERN, &anch::date::formatter::MinuteFormatter::getInstance },
    { anch::date::formatter::SecondFormatter::PATTERN, &anch::date::formatter::SecondFormatter::getInstance },
    { anch::date::formatter::MillisecondFormatter::PATTERN, &anch::date::formatter::MillisecondFormatter::getInstance }
  };
  return formatters;
}


// Constructors +
DateFormatter::DateFormatter(const std::string& dateFormat): _formatters(), _size(0) {
  std::string remain = dateFormat;
  bool ok = true;
  smatch match;
  while(!remain.empty() && ok) {
    if(regex_search(remain, match, DATE_PATTERN)) {
      addFormatter(std::string(match[0].first, match[0].second));
      remain = std::string(match.suffix().first, match.suffix().second);

    } else {
      ok = false;
    }
  }
  _formatters.shrink_to_fit();
}
// Constructors -


// Destructor +
DateFormatter::~DateFormatter() {
  for(const anch::date::formatter::IDatePartFormatter* part : _formatters) {
    delete part;
  }
}
// Destructor -


// Methods +
void
DateFormatter::registerFormatterPart(const std::string& pattern,
				     getInstance instGetter) {
  if(getFormatters().find(pattern) == getFormatters().end()) {
    getFormatters()[pattern] = instGetter;
  }
}

void
DateFormatter::format(const Date& date, std::string& output) const {
  std::ostringstream oss;
  format(date, oss);
  output.clear();
  output = oss.str();
}

void
DateFormatter::format(const Date& date, std::ostream& output) const {
  for(const anch::date::formatter::IDatePartFormatter* part : _formatters) {
    part->format(date, output);
  }
  output.flush();
}

void
DateFormatter::parse(const std::string& strDate, Date& date) const throw(InvalidFormatException) {
  if(strDate.size() != _size) {
    std::stringstream oss;
    oss << "Found " << strDate.size() << " characters instead of " << _size;
    throw InvalidFormatException(oss.str());
  }
  std::size_t offset = 0;
  bool ok = false;
  for(const anch::date::formatter::IDatePartFormatter* part : _formatters) {
    ok = part->setValue(date, strDate.substr(offset, part->getSize()));
    if(!ok) {
      throw InvalidFormatException("Fail to parse " + strDate);
    }
    offset += part->getSize();
  }
  date.computeTimestamp();
}

Date*
DateFormatter::parse(const std::string& strDate) const throw(InvalidFormatException) {
  Date* date = new Date(false);
  parse(strDate, *date);
  return date;
}

void
DateFormatter::addFormatter(const std::string& strFormatter) {
  anch::date::formatter::IDatePartFormatter* form;
  if(strFormatter.length() == 1) {
    form = new anch::date::formatter::ConstantFormatter(strFormatter);

  } else {
    const std::string pattern = strFormatter.substr(0,2);
    auto iter = getFormatters().find(pattern);
    if(iter == getFormatters().end()) {
      form = new anch::date::formatter::ConstantFormatter(strFormatter);
    } else {
      form = (*(iter->second))();
    }

  }
  _formatters.push_back(form);
  _size += form->getSize();
}
// Methods -
