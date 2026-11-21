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
/*
 * CSV reader fuzzer: read records are written and read again to the same fields (round trip), with the default
 * options and with ';' separator and ' quote.
 */
#include "csv/csvError.hpp"
#include "csv/reader.hpp"
#include "csv/writer.hpp"

#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace {

  using Records = std::vector<std::vector<std::string>>;

  Records
  read(const std::string& content, const anch::csv::Options& options) {
    std::istringstream input(content);
    anch::csv::Reader reader(input, options);
    Records records;
    while(reader.next()) {
      records.emplace_back(reader.fields().begin(), reader.fields().end());
    }
    return records;
  }

  void
  roundTrip(const std::string& content, anch::csv::Options options) {
    options.header = false;
    options.sameColumnCount = false;
    Records records;
    try {
      records = read(content, options);
    } catch(const anch::csv::CsvError&) {
      return; // invalid CSV
    }
    std::ostringstream out;
    anch::csv::Writer writer(out, options);
    for(const auto& record : records) {
      std::vector<std::string_view> fields(record.begin(), record.end());
      writer.write(fields);
    }
    if(read(out.str(), options) != records) {
      std::abort();
    }
  }

}

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, std::size_t size) {
  const std::string content(reinterpret_cast<const char*>(data), size);
  roundTrip(content, anch::csv::Options());
  anch::csv::Options options;
  options.separator = ';';
  options.quote = '\'';
  options.crlf = false;
  roundTrip(content, options);
  return 0;
}
