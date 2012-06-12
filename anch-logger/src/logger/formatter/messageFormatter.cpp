/*
    This file is part of ANCH Framework.

    ANCH Framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ANCH Framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
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

using std::string;
using std::regex;
using std::cmatch;
using std::ostringstream;

using anch::logger::formatter::MessageFormatter;
using anch::logger::formatter::IFormatter;
using anch::logger::formatter::FormatterType;
using anch::logger::formatter::LevelFormatter;
using anch::logger::formatter::ConstFormatter;
using anch::logger::formatter::StringFormatter;
using anch::logger::formatter::CategoryFormatter;
using anch::logger::formatter::ThreadIdFormatter;
using anch::logger::formatter::DateFormatter;


// Static initialization +
//regex MessageFormatter::CONFIG_PATTERN = regex(R"((%d\{[^\}]+\})|(%m)|(%l)|([^%]+))");
regex MessageFormatter::CONFIG_PATTERN = regex("%d");
// Static initialization -

/**
 * {@link MessageFormatter} constructor
 *
 * @param linePattern The message line pattern
 */
MessageFormatter::MessageFormatter(const string& linePattern): _formatters() {
  // TODO register data when regex API will work +
  cmatch match;
  if(std::regex_search(linePattern.c_str(), match, CONFIG_PATTERN)) {
    const size_t n = match.size();
    std::cout << "Found " << n << " matching items" << std::endl;
    for(size_t i = 0 ; i < n ; i++) {
      string str(match[i].first, match[i].second);
      std::cout << str << std::endl;
    }
  }
  // TODO register data when regex API will work -
  _formatters.push_back(new DateFormatter("%Y-%m-%d %H:%M:%S"));
  _formatters.push_back(new ConstFormatter(" - "));
  _formatters.push_back(new CategoryFormatter());
  _formatters.push_back(new ConstFormatter(" - [Thread "));
  _formatters.push_back(new ThreadIdFormatter());
  _formatters.push_back(new ConstFormatter("] - "));
  _formatters.push_back(new LevelFormatter());
  _formatters.push_back(new ConstFormatter(" - "));
  _formatters.push_back(new StringFormatter());
}

/**
 * {@link MessageFormatter} destructor
 */
MessageFormatter::~MessageFormatter() {
  // Nothing to do
}

/**
 * Format message.
 *
 * @param category The logger category
 * @param level The logging level
 * @param message The logging message
 *
 * @return The formatted message
 */
const std::string
MessageFormatter::formatMessage(const string& category,
				const Level& level,
				const string& message) const {
  ostringstream out;
  for(size_t i = 0 ; i < _formatters.size() ; i++) {
    const IFormatter* formatter = _formatters[i];
    switch(formatter->getType()) {
    case FormatterType::CONST:
      formatter->formatValue(NULL, out);
      break;

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
      formatter->formatValue(NULL, out);
      break;

    case FormatterType::DATE:
      formatter->formatValue(NULL, out);
      break;

    default:
      // We do nothing
      break;
    }
  }

  return out.str();
}
