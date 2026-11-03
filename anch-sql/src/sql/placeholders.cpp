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
#include "sql/placeholders.hpp"

#include <cctype>

using anch::sql::SqlSyntax;

const SqlSyntax SqlSyntax::STANDARD = {};

const SqlSyntax SqlSyntax::POSTGRESQL = {
  .escapeString = true,
  .dollarQuote = true,
  .nestedComment = true
};

const SqlSyntax SqlSyntax::MYSQL = {
  .backslashEscape = true,
  .doubleQuoteString = true,
  .backquoteIdentifier = true,
  .hashComment = true,
  .dashCommentNeedsSpace = true
};

const SqlSyntax SqlSyntax::SQLITE = {
  .backquoteIdentifier = true,
  .bracketIdentifier = true
};

namespace {

  /*!
   * \brief SQL query scanner
   */
  class Scanner {
  private:
    /*! Query */
    std::string_view _query;

    /*! Syntax */
    const SqlSyntax& _syntax;

    /*! Current position */
    std::size_t _pos;

  public:
    /*!
     * \c Scanner constructor
     *
     * \param query the query
     * \param syntax the syntax
     */
    Scanner(std::string_view query, const SqlSyntax& syntax): _query(query), _syntax(syntax), _pos(0) {
      // Nothing to do
    }

    /*!
     * Scan query
     *
     * \return the placeholders positions
     */
    std::vector<std::size_t> scan() {
      std::vector<std::size_t> positions;
      while(_pos < _query.size()) {
	char c = _query[_pos];
	switch(c) {
	case '?':
	  positions.push_back(_pos);
	  ++_pos;
	  break;
	case '\'':
	  skipQuoted('\'', _syntax.backslashEscape || isEscapeString());
	  break;
	case '"':
	  skipQuoted('"', _syntax.doubleQuoteString && _syntax.backslashEscape);
	  break;
	case '`':
	  skipQuotedIf(_syntax.backquoteIdentifier, '`');
	  break;
	case '[':
	  if(_syntax.bracketIdentifier) {
	    skipUntil("]");
	  } else {
	    ++_pos;
	  }
	  break;
	case '-':
	  if(at(1) == '-' && (!_syntax.dashCommentNeedsSpace || _pos + 2 >= _query.size() || std::isspace(static_cast<unsigned char>(at(2))))) {
	    skipUntil("\n");
	  } else {
	    ++_pos;
	  }
	  break;
	case '#':
	  if(_syntax.hashComment) {
	    skipUntil("\n");
	  } else {
	    ++_pos;
	  }
	  break;
	case '/':
	  if(at(1) == '*') {
	    skipBlockComment();
	  } else {
	    ++_pos;
	  }
	  break;
	case '$':
	  if(!_syntax.dollarQuote || !skipDollarQuoted()) {
	    ++_pos;
	  }
	  break;
	default:
	  if(isIdentifier(c)) { // whole word: '$' and quotes inside identifiers are not delimiters
	    while(_pos < _query.size() && (isIdentifier(_query[_pos]) || _query[_pos] == '$')) {
	      ++_pos;
	    }
	  } else {
	    ++_pos;
	  }
	}
      }
      return positions;
    }

  private:
    /*!
     * Character at offset from current position
     *
     * \param offset the offset
     *
     * \return the character, \c '\\0' after the end
     */
    char at(std::size_t offset) const {
      return _pos + offset < _query.size() ? _query[_pos + offset] : '\0';
    }

    /*!
     * Check if character can be part of an identifier
     *
     * \param c the character
     *
     * \return \c true when character is a letter, a digit, \c '_' or a non ASCII character
     */
    static bool isIdentifier(char c) {
      return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || static_cast<unsigned char>(c) >= 0x80;
    }

    /*!
     * Check if current quote starts an escape string (\c E'...' )
     *
     * \return \c true for an escape string
     */
    bool isEscapeString() const {
      return _syntax.escapeString && _pos > 0 && (_query[_pos - 1] == 'E' || _query[_pos - 1] == 'e')
	&& (_pos < 2 || !isIdentifier(_query[_pos - 2]));
    }

    /*!
     * Skip quoted string or identifier (a doubled quote is an escaped quote)
     *
     * \param quote the quote character
     * \param backslash \c true when backslash escapes next character
     */
    void skipQuoted(char quote, bool backslash) {
      ++_pos;
      while(_pos < _query.size()) {
	char c = _query[_pos];
	if(backslash && c == '\\') {
	  _pos += 2;
	} else if(c == quote) {
	  if(at(1) != quote) {
	    ++_pos;
	    return;
	  }
	  _pos += 2;
	} else {
	  ++_pos;
	}
      }
    }

    /*!
     * Skip quoted identifier when syntax allows it
     *
     * \param allowed \c true when the quote is a delimiter
     * \param quote the quote character
     */
    void skipQuotedIf(bool allowed, char quote) {
      if(allowed) {
	skipQuoted(quote, false);
      } else {
	++_pos;
      }
    }

    /*!
     * Skip until end delimiter (included)
     *
     * \param end the end delimiter
     */
    void skipUntil(std::string_view end) {
      std::size_t found = _query.find(end, _pos + 1);
      _pos = found == std::string_view::npos ? _query.size() : found + end.size();
    }

    /*!
     * Skip block comment (nested when syntax allows it)
     */
    void skipBlockComment() {
      std::size_t depth = 0;
      while(_pos < _query.size()) {
	if(at(0) == '/' && at(1) == '*' && (depth == 0 || _syntax.nestedComment)) {
	  ++depth;
	  _pos += 2;
	} else if(at(0) == '*' && at(1) == '/') {
	  _pos += 2;
	  if(--depth == 0) {
	    return;
	  }
	} else {
	  ++_pos;
	}
      }
    }

    /*!
     * Skip dollar quoted string (\c $tag$...$tag$ ) ; \c $1 is not a dollar quote
     *
     * \return \c true when a dollar quoted string has been skipped
     */
    bool skipDollarQuoted() {
      std::size_t end = _pos + 1;
      if(end < _query.size() && std::isdigit(static_cast<unsigned char>(_query[end]))) {
	return false; // positional parameter
      }
      while(end < _query.size() && isIdentifier(_query[end])) {
	++end;
      }
      if(end >= _query.size() || _query[end] != '$') {
	return false;
      }
      std::string_view tag = _query.substr(_pos, end - _pos + 1);
      std::size_t close = _query.find(tag, end + 1);
      _pos = close == std::string_view::npos ? _query.size() : close + tag.size();
      return true;
    }
  };

}

std::vector<std::size_t>
anch::sql::findPlaceholders(std::string_view query, const SqlSyntax& syntax) {
  return Scanner(query, syntax).scan();
}

std::string
anch::sql::numberPlaceholders(std::string_view query, const std::vector<std::size_t>& positions, char prefix) {
  std::string result;
  result.reserve(query.size() + positions.size() * 2);
  std::size_t offset = 0;
  std::size_t idx = 0;
  for(std::size_t pos : positions) {
    result.append(query.substr(offset, pos - offset));
    result += prefix;
    result += std::to_string(++idx);
    offset = pos + 1;
  }
  result.append(query.substr(offset));
  return result;
}
