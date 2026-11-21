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
#include "csv/reader.hpp"

#include <stdexcept>
#include <string>

using anch::csv::CsvError;
using anch::csv::ErrorCode;
using anch::csv::Options;
using anch::csv::Reader;

namespace {

  constexpr int CR = '\r';
  constexpr int LF = '\n';
  constexpr int END = std::char_traits<char>::eof();

  constexpr int
  code(char c) noexcept {
    return static_cast<unsigned char>(c);
  }

  void
  check(const Options& options) {
    if(options.separator == options.quote) {
      throw std::invalid_argument("CSV separator and quote have to be different");
    }
    for(char c : {options.separator, options.quote}) {
      if(c == '\r' || c == '\n') {
	throw std::invalid_argument("CSV separator and quote can not be CR or LF");
      }
    }
  }

}

Reader::Reader(std::istream& input, const Options& options):
  _input(input.rdbuf()), _stream(input), _options(options), _pending(), _buffer(), _ends(), _quoted(), _fields(),
  _header(), _line(1), _recordLine(1), _recordSize(0), _size(0), _columns(0) {
  check(_options);
  if(_input == nullptr) {
    throw std::invalid_argument("CSV input stream has no buffer");
  }
  // UTF-8 BOM: skipped ; other bytes are read again
  if(_input->sgetc() == 0xEF) {
    _pending.push_back(static_cast<char>(_input->sbumpc()));
    if(_input->sgetc() == 0xBB) {
      _pending.push_back(static_cast<char>(_input->sbumpc()));
      if(_input->sgetc() == 0xBF) {
	_input->sbumpc();
	_pending.clear();
      }
    }
  }
  if(_options.header && read()) {
    _header.assign(_fields.begin(), _fields.end());
    _columns = _header.size();
  }
}

bool
Reader::next() {
  if(!read()) {
    return false;
  }
  if(_options.sameColumnCount) {
    if(_columns == 0) {
      _columns = _fields.size();
    } else if(_fields.size() != _columns) {
      throw CsvError(ErrorCode::COLUMN_COUNT, "record has " + std::to_string(_fields.size()) + " columns instead of "
		     + std::to_string(_columns), _recordLine, 0);
    }
  }
  return true;
}

std::optional<std::size_t>
Reader::column(std::string_view name) const noexcept {
  for(std::size_t i = 0; i < _header.size(); ++i) {
    if(_header[i] == name) {
      return i;
    }
  }
  return std::nullopt;
}

std::span<const std::string_view>
Reader::fields() const noexcept {
  return _fields;
}

std::size_t
Reader::size() const noexcept {
  return _fields.size();
}

std::string_view
Reader::operator[](std::size_t index) const {
  return _fields.at(index);
}

bool
Reader::quoted(std::size_t index) const {
  return _quoted.at(index);
}

std::size_t
Reader::line() const noexcept {
  return _recordLine;
}

const std::vector<std::string>&
Reader::header() const noexcept {
  return _header;
}

const Options&
Reader::options() const noexcept {
  return _options;
}

int
Reader::get() {
  int c;
  if(!_pending.empty()) {
    c = code(_pending.front());
    _pending.erase(0, 1);
  } else {
    c = _input->sbumpc();
    if(c == std::char_traits<char>::eof()) {
      _stream.setstate(std::ios::eofbit);
      return END;
    }
  }
  if(_options.maxSize != 0 && ++_size > _options.maxSize) {
    fail(ErrorCode::TOO_LARGE, "input is larger than " + std::to_string(_options.maxSize) + " bytes");
  }
  if(_options.maxRecordSize != 0 && ++_recordSize > _options.maxRecordSize) {
    fail(ErrorCode::TOO_LARGE, "record is larger than " + std::to_string(_options.maxRecordSize) + " bytes");
  }
  return c;
}

bool
Reader::accept(char expected) {
  const int c = _pending.empty() ? _input->sgetc() : code(_pending.front());
  if(c != code(expected)) {
    return false;
  }
  get();
  return true;
}

void
Reader::fail(ErrorCode errorCode, const std::string& message) const {
  throw CsvError(errorCode, message, _line, _ends.size() + 1);
}

bool
Reader::read() {
  _buffer.clear();
  _ends.clear();
  _quoted.clear();
  _fields.clear();
  _recordSize = 0;
  const int separator = code(_options.separator);
  const int quote = code(_options.quote);
  int c = get();
  if(c == END) {
    return false;
  }
  _recordLine = _line;
  while(true) {
    // Field +
    bool quoted = false;
    if(c == quote) {
      quoted = true;
      while(true) {
	c = get();
	if(c == END) {
	  fail(ErrorCode::INVALID_FORMAT, "unterminated quoted field");
	}
	if(c == quote) {
	  if(!accept(_options.quote)) {
	    c = get();
	    break;
	  }
	} else if(c == LF) {
	  ++_line;
	}
	_buffer.push_back(static_cast<char>(c));
      }
      if(c != separator && c != CR && c != LF && c != END) {
	fail(ErrorCode::INVALID_FORMAT, "unexpected character after a closing quote");
      }
    } else {
      while(c != separator && c != CR && c != LF && c != END) {
	if(c == quote) {
	  fail(ErrorCode::INVALID_FORMAT, "quote in an unquoted field");
	}
	_buffer.push_back(static_cast<char>(c));
	c = get();
      }
    }
    if(_options.maxColumns != 0 && _ends.size() == _options.maxColumns) {
      fail(ErrorCode::TOO_LARGE, "record has more than " + std::to_string(_options.maxColumns) + " columns");
    }
    _ends.push_back(_buffer.size());
    _quoted.push_back(quoted);
    // Field -
    if(c == separator) {
      c = get();
      continue;
    }
    if(c == CR) {
      if(!accept('\n')) {
	fail(ErrorCode::INVALID_FORMAT, "CR without LF");
      }
      ++_line;
    } else if(c == LF) {
      ++_line;
    }
    break;
  }
  std::size_t begin = 0;
  for(std::size_t end : _ends) {
    _fields.emplace_back(_buffer.data() + begin, end - begin);
    begin = end;
  }
  return true;
}
