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
// anch-csv documentation examples (anch-csv.dox)
#include "csv/csvError.hpp"
#include "csv/reader.hpp"
#include "csv/writer.hpp"

#include <iostream>
#include <sstream>
#include <string>

int
main() {
  std::stringstream file;

  //! [writer]
  anch::csv::Writer writer(file);
  writer.write({"id", "name", "comment"});
  writer.writeRow(1, "Rex", "good, dog");          // "good, dog" is quoted
  writer.writeRow(2, "Médor", "says \"woof\"");    // quotes are doubled
  //! [writer]

  //! [reader]
  try {
    anch::csv::Reader reader(file);                 // the header is read (Options::header)
    const std::size_t name = reader.column("name").value();
    while(reader.next()) {                          // one record at a time
      std::cout << reader.line() << ": " << reader[name] << std::endl;
    }
  } catch(const anch::csv::CsvError& error) {
    std::cerr << error.what() << std::endl;       // line 3, column 2: quote in an unquoted field
    return 1;
  }
  //! [reader]

  //! [options]
  anch::csv::Options options;
  options.separator = ';';     // spreadsheets with a comma as decimal separator
  options.crlf = false;
  options.bom = true;          // UTF-8 BOM for spreadsheets
  options.header = false;      // reader: no header, the first record is data
  std::ostringstream out;
  anch::csv::Writer(out, options).writeRow("1,5", true);
  //! [options]
  return out.str() == "\xEF\xBB\xBF" "1,5;true\n" ? 0 : 1;
}
