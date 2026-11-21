/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "val/constraints.hpp"

#include <algorithm>
#include <map>
#include <mutex>
#include <regex>
#include <shared_mutex>
#include <stdexcept>

#include <arpa/inet.h>

#ifdef ANCH_UUID
#include "uuid.hpp"
#endif

#ifdef ANCH_DATE
#include "date/rfc3339.hpp"
#endif

using anch::val::FormatValidator;
using anch::val::PatternCompiler;
using anch::val::PatternMatcher;


namespace {

  // Patterns +
  /*! Default pattern compiler: std::regex (ECMAScript grammar) */
  PatternMatcher
  compileRegex(std::string_view source) {
    auto regex = std::make_shared<const std::regex>(source.begin(), source.end(), std::regex::ECMAScript);
    return [regex](std::string_view value) -> bool {
      return std::regex_search(value.begin(), value.end(), *regex);
    };
  }

  /*! Unsupported XSD pattern construct */
  [[noreturn]] void
  unsupported(std::string_view source, std::string_view construct) {
    throw std::invalid_argument("XSD pattern '" + std::string(source) + "': " + std::string(construct)
				+ " is not supported by the default compiler (see anch::val::setPatternCompiler)");
  }

  /*!
   * Translate XSD regular expression into ECMAScript one, matched on the whole value: only constructs which keep their
   * meaning are translated
   */
  std::string
  translateXsd(std::string_view source) {
    std::string res;
    bool inClass = false;
    for(std::size_t i = 0 ; i < source.size() ; ++i) {
      const char c = source[i];
      const char next = (i + 1 < source.size()) ? source[i + 1] : '\0';
      if(c == '\\') {
	if(i + 1 == source.size()) {
	  throw std::invalid_argument("XSD pattern '" + std::string(source) + "': '\\' at the end of the pattern");
	}
	++i;
	switch(next) {
	case 'n': case 'r': case 't': case '\\': case '|': case '.': case '?': case '*': case '+': case '(': case ')':
	case '{': case '}': case '-': case '[': case ']': case '^':
	  res += '\\';
	  res += next;
	  break;
	case 's':
	  res += inClass ? " \\t\\n\\r" : "[ \\t\\n\\r]";
	  break;
	case 'd':
	  res += inClass ? "0-9" : "[0-9]";
	  break;
	case 'S':
	case 'D':
	  if(inClass) {
	    unsupported(source, std::string("'\\") + next + "' in a character class");
	  }
	  res += (next == 'S') ? "[^ \\t\\n\\r]" : "[^0-9]";
	  break;
	case 'p': case 'P': case 'i': case 'I': case 'c': case 'C': case 'w': case 'W':
	  unsupported(source, std::string("'\\") + next + "' (Unicode properties)");
	default:
	  throw std::invalid_argument("XSD pattern '" + std::string(source) + "': invalid escape '\\" + std::string(1, next) + "'");
	}
      } else if(inClass) {
	if(c == '[' || (c == '-' && next == '[')) {
	  unsupported(source, "character class subtraction");
	}
	if(c == ']') {
	  inClass = false;
	}
	res += c;
      } else if(c == '^' || c == '$') { // ordinary characters in XSD
	res += '\\';
	res += c;
      } else if(c == '[') {
	inClass = true;
	res += c;
	if(next == '^') {
	  res += next;
	  ++i;
	}
      } else if(c == '(' && next == '?') {
	throw std::invalid_argument("XSD pattern '" + std::string(source) + "': '(?' is not XSD syntax");
      } else if((c == '?' || c == '*' || c == '+' || c == '}') && next == '?') {
	throw std::invalid_argument("XSD pattern '" + std::string(source) + "': lazy quantifiers are not XSD syntax");
      } else {
	res += c;
      }
    }
    if(inClass) {
      throw std::invalid_argument("XSD pattern '" + std::string(source) + "': unterminated character class");
    }
    return res;
  }

  /*! Default XSD pattern compiler: std::regex on the translated pattern, matched on the whole value */
  PatternMatcher
  compileXsd(std::string_view source) {
    const std::string translated = translateXsd(source);
    auto regex = std::make_shared<const std::regex>(translated, std::regex::ECMAScript);
    return [regex](std::string_view value) -> bool {
      return std::regex_match(value.begin(), value.end(), *regex);
    };
  }

  /*! Pattern compilers mutex */
  std::mutex PATTERN_MUTEX;

  /*! Pattern compilers by syntax (default compiler when empty) */
  PatternCompiler PATTERN_COMPILERS[2];

  /*! Compile pattern */
  anch::val::Pattern
  compile(std::string_view source, anch::val::PatternSyntax syntax) {
    PatternMatcher matcher;
    {
      std::lock_guard<std::mutex> lock(PATTERN_MUTEX);
      const PatternCompiler& compiler = PATTERN_COMPILERS[static_cast<std::size_t>(syntax)];
      if(compiler) {
	matcher = compiler(source);
      } else {
	matcher = (syntax == anch::val::PatternSyntax::XSD) ? compileXsd(source) : compileRegex(source);
      }
    }
    return anch::val::Pattern{std::make_shared<const PatternMatcher>(std::move(matcher)), std::string(source), syntax};
  }
  // Patterns -

  // Formats +
  /*! Alphanumeric ASCII character */
  bool
  isAlnum(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
  }

  /*! RFC 1123 host name */
  bool
  isHostname(std::string_view value) {
    if(value.empty() || value.size() > 253) {
      return false;
    }
    std::size_t begin = 0;
    while(begin <= value.size()) {
      std::size_t end = value.find('.', begin);
      if(end == std::string_view::npos) {
	end = value.size();
      }
      std::string_view label = value.substr(begin, end - begin);
      if(label.empty() || label.size() > 63 || label.front() == '-' || label.back() == '-') {
	return false;
      }
      for(char c : label) {
	if(!isAlnum(c) && c != '-') {
	  return false;
	}
      }
      begin = end + 1;
    }
    return true;
  }

  /*! E-mail address: dot-atom local part and host name domain */
  bool
  isEmail(std::string_view value) {
    std::size_t at = value.rfind('@');
    if(at == std::string_view::npos || at == 0 || at > 64) {
      return false;
    }
    std::string_view local = value.substr(0, at);
    if(local.front() == '.' || local.back() == '.' || local.find("..") != std::string_view::npos) {
      return false;
    }
    static constexpr std::string_view ATEXT = "!#$%&'*+-/=?^_`{|}~.";
    for(char c : local) {
      if(!isAlnum(c) && ATEXT.find(c) == std::string_view::npos) {
	return false;
      }
    }
    return isHostname(value.substr(at + 1));
  }

  /*! IP address */
  bool
  isIp(std::string_view value, int family) {
    if(value.size() > 45) { // longest IPv6 address with IPv4 suffix
      return false;
    }
    std::string address(value); // inet_pton needs a null terminated string
    unsigned char buffer[16];
    return inet_pton(family, address.data(), buffer) == 1;
  }

  /*! Hexadecimal digit */
  bool
  isHex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
  }

  /*! RFC 3986 URI: scheme and URI characters (percent encoding is checked) */
  bool
  isUri(std::string_view value) {
    std::size_t colon = value.find(':');
    if(colon == std::string_view::npos || colon == 0 || !((value[0] >= 'a' && value[0] <= 'z') || (value[0] >= 'A' && value[0] <= 'Z'))) {
      return false;
    }
    for(char c : value.substr(0, colon)) {
      if(!isAlnum(c) && c != '+' && c != '-' && c != '.') {
	return false;
      }
    }
    static constexpr std::string_view URI_CHARS = "-._~:/?#[]@!$&'()*+,;=";
    for(std::size_t i = colon + 1 ; i < value.size() ; ++i) {
      char c = value[i];
      if(c == '%') {
	if(i + 2 >= value.size() || !isHex(value[i + 1]) || !isHex(value[i + 2])) {
	  return false;
	}
	i += 2;
      } else if(!isAlnum(c) && URI_CHARS.find(c) == std::string_view::npos) {
	return false;
      }
    }
    return true;
  }

  /*! Base64 with padding (RFC 4648) */
  bool
  isBase64(std::string_view value) {
    if(value.size() % 4 != 0) {
      return false;
    }
    std::size_t padding = 0;
    while(padding < 2 && padding < value.size() && value[value.size() - 1 - padding] == '=') {
      ++padding;
    }
    for(std::size_t i = 0 ; i < value.size() - padding ; ++i) {
      char c = value[i];
      if(!isAlnum(c) && c != '+' && c != '/') {
	return false;
      }
    }
    return true;
  }

  /*! Formats registry */
  struct Formats {
    /*! Registry mutex */
    std::shared_mutex mutex;
    /*! Validators */
    std::map<std::string,std::shared_ptr<const FormatValidator>,std::less<>> validators;

    Formats() {
      add("email", isEmail);
      add("hostname", isHostname);
      add("ipv4", [](std::string_view value) -> bool { return isIp(value, AF_INET); });
      add("ipv6", [](std::string_view value) -> bool { return isIp(value, AF_INET6); });
      add("uri", isUri);
      add("byte", isBase64);
#ifdef ANCH_UUID
      add("uuid", [](std::string_view value) -> bool {
	try {
	  anch::UUID uuid;
	  uuid.parse(std::string(value));
	  return true;
	} catch(const std::invalid_argument&) {
	  return false;
	}
      });
#endif
#ifdef ANCH_DATE
      add("date-time", [](std::string_view value) -> bool {
	try {
	  anch::date::rfc3339::parseDateTime(value);
	  return true;
	} catch(const std::invalid_argument&) {
	  return false;
	}
      });
      add("date", [](std::string_view value) -> bool {
	try {
	  anch::date::rfc3339::parseDate(value);
	  return true;
	} catch(const std::invalid_argument&) {
	  return false;
	}
      });
#endif
    }

    void
    add(const std::string& name, FormatValidator validator) {
      validators[name] = std::make_shared<const FormatValidator>(std::move(validator));
    }
  };

  /*! Formats registry instance (built on first use) */
  Formats&
  formats() {
    static Formats instance;
    return instance;
  }
  // Formats -

}

void
anch::val::setPatternCompiler(PatternCompiler compiler) {
  anch::val::setPatternCompiler(anch::val::PatternSyntax::ECMASCRIPT, std::move(compiler));
}

void
anch::val::setPatternCompiler(anch::val::PatternSyntax syntax, PatternCompiler compiler) {
  std::lock_guard<std::mutex> lock(PATTERN_MUTEX);
  PATTERN_COMPILERS[static_cast<std::size_t>(syntax)] = std::move(compiler);
}

anch::val::Pattern
anch::val::pattern(std::string_view source) {
  return compile(source, anch::val::PatternSyntax::ECMASCRIPT);
}

anch::val::Pattern
anch::val::xsdPattern(std::string_view source) {
  return compile(source, anch::val::PatternSyntax::XSD);
}

void
anch::val::registerFormat(const std::string& name, FormatValidator validator) {
  Formats& registry = formats();
  std::unique_lock<std::shared_mutex> lock(registry.mutex);
  if(validator) {
    registry.add(name, std::move(validator));
  } else {
    registry.validators.erase(name);
  }
}

bool
anch::val::detail::matchesFormat(const std::string& name, std::string_view value) {
  std::shared_ptr<const FormatValidator> validator;
  {
    Formats& registry = formats();
    std::shared_lock<std::shared_mutex> lock(registry.mutex);
    auto iter = registry.validators.find(name);
    if(iter == registry.validators.end()) { // unknown format: annotation only
      return true;
    }
    validator = iter->second;
  }
  return std::invoke(*validator, value);
}


std::size_t
anch::val::detail::codePoints(std::string_view str) {
  std::size_t count = 0;
  for(char c : str) {
    if((static_cast<unsigned char>(c) & 0xC0) != 0x80) { // UTF-8 continuation bytes are not counted
      ++count;
    }
  }
  return count;
}

std::optional<std::pair<std::size_t, std::size_t>>
anch::val::detail::decimalDigits(std::string_view decimal) {
  std::size_t pos = 0;
  if(pos < decimal.size() && (decimal[pos] == '+' || decimal[pos] == '-')) {
    ++pos;
  }
  const std::size_t intBegin = pos;
  while(pos < decimal.size() && decimal[pos] >= '0' && decimal[pos] <= '9') {
    ++pos;
  }
  std::string_view integer = decimal.substr(intBegin, pos - intBegin);
  std::string_view fraction;
  if(pos < decimal.size() && decimal[pos] == '.') {
    const std::size_t fracBegin = ++pos;
    while(pos < decimal.size() && decimal[pos] >= '0' && decimal[pos] <= '9') {
      ++pos;
    }
    fraction = decimal.substr(fracBegin, pos - fracBegin);
  }
  if(pos != decimal.size() || (integer.empty() && fraction.empty())) {
    return std::nullopt;
  }
  const std::size_t lead = integer.find_first_not_of('0');
  integer = (lead == std::string_view::npos) ? std::string_view() : integer.substr(lead);
  const std::size_t trail = fraction.find_last_not_of('0');
  fraction = (trail == std::string_view::npos) ? std::string_view() : fraction.substr(0, trail + 1);
  const std::size_t total = integer.size() + fraction.size();
  return std::make_pair(total == 0 ? std::size_t{1} : total, fraction.size());
}
