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
#include "csv/csv.hpp"
#include "csv/csvError.hpp"
#include "csv/mapper.hpp"
#include "csv/reader.hpp"
#include "csv/writer.hpp"

#include <cstdint>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

//! [mapping]
struct Pet {
  uint64_t id;
  std::string name;
  std::optional<std::string> nickname;
};

// Description shared by formats (anch-utils): fields are columns
template<>
struct anch::Mapping<Pet> {
  static void
  registerObject(anch::ObjectMapper<Pet> auto& mapper) {
    mapper
      .registerField("id", &Pet::id, anch::val::required)
      .registerField("name", &Pet::name, anch::val::required, anch::val::minLength(1))
      .registerField("nickname", &Pet::nickname);
  }
};
//! [mapping]

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

  //! [objects writer]
  std::stringstream pets;
  anch::csv::Writer petsWriter(pets);
  anch::csv::ObjectWriter<Pet> objectsWriter(petsWriter); // header, then one record per object
  objectsWriter.write(Pet{1, "Rex", std::nullopt});        // 1,Rex,     (absent nickname)
  objectsWriter.write(Pet{2, "Médor", std::string()});     // 2,Médor,"" (empty nickname)
  //! [objects writer]

  //! [objects reader]
  anch::csv::Reader petsReader(pets);
  anch::csv::ObjectReader<Pet> objectsReader(petsReader);  // columns associated with fields by name
  Pet pet;
  while(objectsReader.read(pet)) {
    std::cout << pet.id << ' ' << pet.name << (pet.nickname ? " has a nickname" : "") << std::endl;
  }
  //! [objects reader]

  //! [records]
  // Objects read one by one, without loading the whole input
  std::stringstream input("id,name\n1,Rex\n2,Médor\n");
  std::size_t count = 0;
  for(const Pet& record : anch::csv::records<Pet>(input)) {
    std::cout << record.name << std::endl;
    ++count;
  }

  // Small volumes: vectors
  std::vector<Pet> all;
  std::stringstream again("id,name\n1,Rex\n2,Médor\n");
  anch::csv::deserialize(all, again);
  std::ostringstream written;
  anch::csv::serialize(all, written); // header, then one record per object
  //! [records]

  //! [options]
  anch::csv::Options options;
  options.separator = ';';     // spreadsheets with a comma as decimal separator
  options.crlf = false;
  options.bom = true;          // UTF-8 BOM for spreadsheets
  options.header = false;      // reader: no header, the first record is data
  std::ostringstream out;
  anch::csv::Writer(out, options).writeRow("1,5", true);
  //! [options]
  return (out.str() == "\xEF\xBB\xBF" "1,5;true\n" && count == 2 && all.size() == 2) ? 0 : 1;
}
