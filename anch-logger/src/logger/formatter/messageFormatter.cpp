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
#include "logger/formatter/messageFormatter.hpp"

#include <sstream>

#include "logger/formatter/levelFormatter.hpp"
#include "logger/formatter/constFormatter.hpp"
#include "logger/formatter/stringFormatter.hpp"
#include "logger/formatter/categoryFormatter.hpp"
#include "logger/formatter/threadIdFormatter.hpp"
#include "logger/formatter/dateFormatter.hpp"
#include "logger/formatter/anchDateFormatter.hpp"

using std::string;
using std::ostringstream;

#ifdef ANCH_BOOST_REGEX
using boost::regex;
using boost::smatch;
using boost::regex_search;
#else
using std::regex;
using std::smatch;
using std::regex_search;
#endif

using anch::logger::formatter::MessageFormatter;
using anch::logger::formatter::IFormatter;
using anch::logger::formatter::FormatterType;
using anch::logger::formatter::LevelFormatter;
using anch::logger::formatter::ConstFormatter;
using anch::logger::formatter::StringFormatter;
using anch::logger::formatter::CategoryFormatter;
using anch::logger::formatter::ThreadIdFormatter;
using anch::logger::formatter::DateFormatter;
using anch::logger::formatter::AnchDateFormatter;


// Static initialization +
const regex MessageFormatter::CONFIG_PATTERN = regex(R"(^((\$d\{[^\}]+\})|(\$D\{[^\}]+\})|(\$m)|(\$c)|(\$p)|(\$t)|([^\$]+)))");
// Static initialization -

MessageFormatter::MessageFormatter(const string& linePattern): _formatters() {
  smatch match;
  string line = linePattern;
  bool ok = true;
  while(!line.empty() && ok) {
    if(regex_search(line, match, CONFIG_PATTERN)) {
      addFormatter(string(match[0].first, match[0].second));
      line = string(match.suffix().first, match.suffix().second);

    } else {
      ok = false;
    }
  }
  _formatters.shrink_to_fit();
}

MessageFormatter::~MessageFormatter() {
  for(auto item : _formatters) {
    delete item;
  }
}


void
MessageFormatter::addFormatter(const string& strFormatter) {
  if(strFormatter.length() == 1) {
    _formatters.push_back(new ConstFormatter(strFormatter));

  } else {
    const string pattern = strFormatter.substr(0,2);
    if(pattern == "$d") {
      _formatters.push_back(new DateFormatter(strFormatter.substr(3, strFormatter.length() - 4)));

    } else if(pattern == "$D") {
      _formatters.push_back(new AnchDateFormatter(strFormatter.substr(3, strFormatter.length() - 4)));

    } else if(pattern == "$m") {
      _formatters.push_back(new StringFormatter());

    } else if(pattern == "$c") {
      _formatters.push_back(new CategoryFormatter());

    } else if(pattern == "$p") {
      _formatters.push_back(new LevelFormatter());

    } else if(pattern == "$t") {
      _formatters.push_back(new ThreadIdFormatter());

    } else {
      _formatters.push_back(new ConstFormatter(strFormatter));

    }
  }
}

const std::string
MessageFormatter::formatMessage(const string& category,
				const Level& level,
				const string& message) const {
  ostringstream out;
  for(size_t i = 0 ; i < _formatters.size() ; i++) {
    const IFormatter* formatter = _formatters[i];
    switch(formatter->getType()) {
    case FormatterType::LEVEL:
      formatter->formatValue(&level, out);
      break;

    case FormatterType::STRING:
      formatter->formatValue(&message, out);
      break;

    case FormatterType::CATEGORY:
      formatter->formatValue(&category, out);
      break;

    case FormatterType::THREAD_ID:
    case FormatterType::CONST:
    case FormatterType::DATE:
    case FormatterType::ANCH_DATE:
      formatter->formatValue(NULL, out);
      break;

    default:
      // We do nothing
      break;
    }
  }

  return out.str();
}
