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
#include "csv/writer.hpp"

#include <stdexcept>

using anch::csv::Options;
using anch::csv::Writer;

Writer::Writer(std::ostream& output, const Options& options): _output(output), _options(options), _start(true) {
  if(_options.separator == _options.quote) {
    throw std::invalid_argument("CSV separator and quote have to be different");
  }
  for(char c : {_options.separator, _options.quote}) {
    if(c == '\r' || c == '\n') {
      throw std::invalid_argument("CSV separator and quote can not be CR or LF");
    }
  }
  if(_options.bom) {
    _output << "\xEF\xBB\xBF";
  }
}

void
Writer::write(std::span<const std::string_view> fields) {
  for(std::size_t i = 0; i < fields.size(); ++i) {
    if(i != 0) {
      _output.put(_options.separator);
    }
    writeField(fields[i], _start && i == 0 && fields[i].starts_with("\xEF\xBB\xBF"));
  }
  _start = false;
  _output << (_options.crlf ? "\r\n" : "\n");
}

void
Writer::write(std::span<const std::string_view> fields, const std::vector<bool>& quoted) {
  if(quoted.size() != fields.size()) {
    throw std::invalid_argument("CSV quoted fields flags have to be given for every field");
  }
  for(std::size_t i = 0; i < fields.size(); ++i) {
    if(i != 0) {
      _output.put(_options.separator);
    }
    writeField(fields[i], quoted[i] || (_start && i == 0 && fields[i].starts_with("\xEF\xBB\xBF")));
  }
  _start = false;
  _output << (_options.crlf ? "\r\n" : "\n");
}

const Options&
Writer::options() const noexcept {
  return _options;
}

void
Writer::write(std::initializer_list<std::string_view> fields) {
  write(std::span<const std::string_view>(fields.begin(), fields.size()));
}

void
Writer::writeField(std::string_view field, bool quote) {
  const char special[] = {_options.separator, _options.quote, '\r', '\n'};
  if(!quote && field.find_first_of(std::string_view(special, sizeof(special))) == std::string_view::npos) {
    _output << field;
    return;
  }
  _output.put(_options.quote);
  std::size_t begin = 0;
  for(std::size_t pos = field.find(_options.quote); pos != std::string_view::npos; pos = field.find(_options.quote, begin)) {
    _output << field.substr(begin, pos + 1 - begin);
    _output.put(_options.quote);
    begin = pos + 1;
  }
  _output << field.substr(begin);
  _output.put(_options.quote);
}
