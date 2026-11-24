/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "sql/placeholders.hpp"

#include <algorithm>
#include <cctype>

#include "sql/sqlException.hpp"

using anch::sql::NamedQuery;
using anch::sql::ParameterStyle;
using anch::sql::SqlException;
using anch::sql::SqlSyntax;

const SqlSyntax SqlSyntax::STANDARD = {};

const SqlSyntax SqlSyntax::POSTGRESQL = {
  .escapeString = true,
  .dollarQuote = true,
  .nestedComment = true,
  .questionMarkOperator = true,
  .arraySlice = true,
  .dollarParameters = true
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
  .bracketIdentifier = true,
  .prefixedParameters = true
};

namespace {

  /*!
   * \brief Marks found by \c Scanner
   */
  struct Marks {
    /*! \c '?' positions */
    std::vector<std::size_t> questions;

    /*! Named parameters (begin: the \c ':' , end: after the name) */
    std::vector<std::pair<std::size_t, std::size_t>> names;

    /*! Native parameters of the dialect (PostgreSQL \c $1 , SQLite \c @name and \c $name ) positions */
    std::vector<std::size_t> natives;
  };

  /*!
   * Check if character can start a parameter name
   *
   * \param c the character
   *
   * \return \c true for an ASCII letter or \c '_'
   */
  bool
  isNameStart(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }

  /*!
   * Check if character can be part of a parameter name
   *
   * \param c the character
   *
   * \return \c true for an ASCII letter, a digit or \c '_'
   */
  bool
  isNameChar(char c) {
    return isNameStart(c) || (c >= '0' && c <= '9');
  }

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

    /*! Parentheses depth */
    std::size_t _parens;

    /*! Parentheses depth of each opened array bracket (PostgreSQL slices) */
    std::vector<std::size_t> _brackets;

  public:
    /*!
     * \c Scanner constructor
     *
     * \param query the query
     * \param syntax the syntax
     */
    Scanner(std::string_view query, const SqlSyntax& syntax): _query(query), _syntax(syntax), _pos(0), _parens(0), _brackets() {
      // Nothing to do
    }

    /*!
     * Scan query
     *
     * \return the marks
     */
    Marks scan() {
      Marks marks;
      while(_pos < _query.size()) {
	char c = _query[_pos];
	switch(c) {
	case '?':
	  marks.questions.push_back(_pos);
	  ++_pos;
	  break;
	case ':':
	  scanColon(marks);
	  break;
	case '(':
	  ++_parens;
	  ++_pos;
	  break;
	case ')':
	  if(_parens > 0) {
	    --_parens;
	  }
	  ++_pos;
	  break;
	case ']':
	  if(!_brackets.empty()) {
	    _brackets.pop_back();
	  }
	  ++_pos;
	  break;
	case '@':
	  if(_syntax.prefixedParameters && isNameStart(at(1))) {
	    marks.natives.push_back(_pos);
	  }
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
	    if(_syntax.arraySlice) {
	      _brackets.push_back(_parens);
	    }
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
	  if((_syntax.dollarParameters && std::isdigit(static_cast<unsigned char>(at(1))))
	     || (_syntax.prefixedParameters && isNameStart(at(1)))) {
	    marks.natives.push_back(_pos);
	    ++_pos;
	  } else if(!_syntax.dollarQuote || !skipDollarQuoted()) {
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
      return marks;
    }

  private:
    /*!
     * Scan \c ':' : named parameter, cast ( \c :: ), assignment ( \c := ) or array slice bound
     *
     * \param marks the marks
     */
    void scanColon(Marks& marks) {
      if(at(1) == ':') { // cast
	_pos += 2;
	return;
      }
      const bool slice = !_brackets.empty() && _brackets.back() == _parens;
      const bool afterIdentifier = _pos > 0 && (isIdentifier(_query[_pos - 1]) || _query[_pos - 1] == ':');
      if(slice || afterIdentifier || !isNameStart(at(1))) {
	++_pos;
	return;
      }
      const std::size_t begin = _pos;
      ++_pos;
      while(_pos < _query.size() && isNameChar(_query[_pos])) {
	++_pos;
      }
      marks.names.emplace_back(begin, _pos);
    }

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

// Named query +
namespace {

  /*! Empty list: empty subquery (IN is false, NOT IN is true, as ANY and ALL of an empty array) */
  constexpr std::string_view EMPTY_LIST = "(SELECT NULL FROM (SELECT 1) AS anch_empty WHERE 1 = 0)";

  /*! Check white space */
  bool
  isSpace(char c) {
    return std::isspace(static_cast<unsigned char>(c)) != 0;
  }

  /*! Check identifier character */
  bool
  isWordChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '$' || static_cast<unsigned char>(c) >= 0x80;
  }

  /*! Check keyword (case insensitive) which ends at position, preceded by a non identifier character */
  bool
  keywordBefore(std::string_view query, std::size_t end, std::string_view keyword) {
    if(end < keyword.size()) {
      return false;
    }
    const std::size_t begin = end - keyword.size();
    for(std::size_t i = 0; i < keyword.size(); ++i) {
      if(std::toupper(static_cast<unsigned char>(query[begin + i])) != keyword[i]) {
	return false;
      }
    }
    return begin == 0 || !isWordChar(query[begin - 1]);
  }

  /*! Skip white spaces backward */
  std::size_t
  spacesBefore(std::string_view query, std::size_t pos) {
    while(pos > 0 && isSpace(query[pos - 1])) {
      --pos;
    }
    return pos;
  }

  /*! Positional parameter error */
  [[noreturn]] void
  positionalError(std::string_view query, std::size_t pos, std::string_view kind) {
    throw SqlException("positional parameters (" + std::string(kind) + ") are not supported: use named parameters"
		       " (':name'), found at offset " + std::to_string(pos) + " of query: " + std::string(query), true);
  }

}

NamedQuery::NamedQuery(std::string_view query, const SqlSyntax& syntax): _query(query), _names(), _occurrences() {
  const Marks marks = Scanner(query, syntax).scan();
  if(!syntax.questionMarkOperator && !marks.questions.empty()) {
    positionalError(query, marks.questions.front(), "'?'");
  }
  if(!marks.natives.empty()) {
    positionalError(query, marks.natives.front(), syntax.dollarParameters ? "'$1'" : "'@name', '$name'");
  }
  for(const auto& [begin, end] : marks.names) {
    const std::string_view name = query.substr(begin + 1, end - begin - 1);
    std::optional<std::size_t> found = slot(name);
    if(!found) {
      found = _names.size();
      _names.emplace_back(name);
    }
    Occurrence occurrence{begin, end, *found, false, false, 0, 0, 0};
    // Only item of an IN list: [NOT] IN ( :name )
    std::size_t close = end;
    while(close < query.size() && isSpace(query[close])) {
      ++close;
    }
    std::size_t open = spacesBefore(query, begin);
    if(close < query.size() && query[close] == ')' && open > 0 && query[open - 1] == '(') {
      --open;
      const std::size_t keyword = spacesBefore(query, open);
      if(keywordBefore(query, keyword, "IN")) {
	occurrence.in = true;
	occurrence.open = open;
	occurrence.close = close;
	occurrence.inBegin = keyword - 2;
	const std::size_t notEnd = spacesBefore(query, occurrence.inBegin);
	if(notEnd < occurrence.inBegin && keywordBefore(query, notEnd, "NOT")) {
	  occurrence.negated = true;
	  occurrence.inBegin = notEnd - 3;
	}
      }
    }
    _occurrences.push_back(occurrence);
  }
}

const std::string&
NamedQuery::query() const noexcept {
  return _query;
}

const std::vector<std::string>&
NamedQuery::names() const noexcept {
  return _names;
}

std::optional<std::size_t>
NamedQuery::slot(std::string_view name) const noexcept {
  auto iter = std::find(_names.begin(), _names.end(), name);
  if(iter == _names.end()) {
    return std::nullopt;
  }
  return static_cast<std::size_t>(iter - _names.begin());
}

NamedQuery::Rendered
NamedQuery::render(ParameterStyle style, std::span<const std::size_t> sizes) const {
  if(!sizes.empty() && sizes.size() != _names.size()) {
    throw SqlException("parameters sizes count (" + std::to_string(sizes.size()) + ") is not the parameters count ("
		       + std::to_string(_names.size()) + ")", true);
  }
  auto sizeOf = [&sizes](std::size_t slot) {
    return sizes.empty() ? SCALAR : sizes[slot];
  };
  Rendered rendered;
  rendered.query.reserve(_query.size() + _occurrences.size() * 4);
  // Native index of each name (DOLLAR, NUMBERED: a name is bound once) ; NUMBERED lists: index of first element
  std::vector<std::size_t> indexes(_names.size(), SCALAR);
  if(style == ParameterStyle::DOLLAR) { // one parameter by name (lists are arrays)
    for(std::size_t slot = 0; slot < _names.size(); ++slot) {
      indexes[slot] = slot + 1;
      rendered.bindings.push_back(Binding{slot, SCALAR});
    }
  }
  const char prefix = (style == ParameterStyle::DOLLAR) ? '$' : '?';
  std::size_t offset = 0;
  for(const Occurrence& occurrence : _occurrences) {
    const std::size_t size = sizeOf(occurrence.slot);
    if(size == SCALAR || style == ParameterStyle::DOLLAR) {
      if(size != SCALAR && occurrence.in) { // array: [NOT] IN (:ids) -> = ANY($1) / <> ALL($1)
	rendered.query.append(_query, offset, occurrence.inBegin - offset);
	rendered.query += occurrence.negated ? "<> ALL(" : "= ANY(";
	rendered.query += prefix + std::to_string(indexes[occurrence.slot]) + ')';
	offset = occurrence.close + 1;
	continue;
      }
      rendered.query.append(_query, offset, occurrence.begin - offset);
      if(style == ParameterStyle::POSITIONAL) {
	rendered.query += '?';
	rendered.bindings.push_back(Binding{occurrence.slot, SCALAR});
      } else {
	if(indexes[occurrence.slot] == SCALAR) {
	  rendered.bindings.push_back(Binding{occurrence.slot, SCALAR});
	  indexes[occurrence.slot] = rendered.bindings.size();
	}
	rendered.query += prefix + std::to_string(indexes[occurrence.slot]);
      }
      offset = occurrence.end;
      continue;
    }
    // Expanded list
    if(!occurrence.in) {
      throw SqlException("list parameter '" + _names[occurrence.slot] + "' has to be the only item of an IN list ("
			 "IN (:" + _names[occurrence.slot] + ")) in query: " + _query, true);
    }
    rendered.query.append(_query, offset, occurrence.open - offset);
    offset = occurrence.close + 1;
    if(size == 0) {
      rendered.query += EMPTY_LIST;
      continue;
    }
    if(style == ParameterStyle::NUMBERED && indexes[occurrence.slot] == SCALAR) {
      indexes[occurrence.slot] = rendered.bindings.size() + 1;
      for(std::size_t element = 0; element < size; ++element) {
	rendered.bindings.push_back(Binding{occurrence.slot, element});
      }
    }
    rendered.query += '(';
    for(std::size_t element = 0; element < size; ++element) {
      if(element > 0) {
	rendered.query += ", ";
      }
      if(style == ParameterStyle::NUMBERED) {
	rendered.query += '?' + std::to_string(indexes[occurrence.slot] + element);
      } else {
	rendered.query += '?';
	rendered.bindings.push_back(Binding{occurrence.slot, element});
      }
    }
    rendered.query += ')';
  }
  rendered.query.append(_query, offset);
  return rendered;
}
// Named query -
