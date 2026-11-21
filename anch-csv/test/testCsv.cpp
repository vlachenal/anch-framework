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
#include "csv/csvError.hpp"
#include "csv/reader.hpp"
#include "csv/writer.hpp"

#include <cstdint>
#include <functional>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::csv::CsvError;
using anch::csv::ErrorCode;
using anch::csv::Options;
using anch::csv::Reader;
using anch::csv::Writer;

namespace {

  using Records = std::vector<std::vector<std::string>>;

  Options
  noHeader() {
    Options options;
    options.header = false;
    return options;
  }

  Records
  readAll(const std::string& content, const Options& options = noHeader()) {
    std::istringstream input(content);
    Reader reader(input, options);
    Records res;
    while(reader.next()) {
      res.emplace_back(reader.fields().begin(), reader.fields().end());
    }
    return res;
  }

  // Error raised when reading the whole content
  CsvError
  readError(const std::string& content, const Options& options = noHeader()) {
    try {
      readAll(content, options);
    } catch(const CsvError& error) {
      return error;
    }
    throw std::runtime_error("CsvError expected for '" + content + "'");
  }

  std::string
  written(const std::function<void(Writer&)>& write, const Options& options = Options()) {
    std::ostringstream out;
    Writer writer(out, options);
    write(writer);
    return out.str();
  }

}

void
testRfc4180() {
  std::istringstream input("id,name,comment\r\n"
			   "1,Rex,\"good, dog\"\r\n"
			   "2,\"Médor \"\"le chien\"\"\",\"two\r\nlines\"\r\n"
			   "3,Félix,\r\n");
  Reader reader(input);
  anch::ut::assertThat(reader.header() == std::vector<std::string>{"id", "name", "comment"}, "Unexpected header");
  anch::ut::assertThat(reader.column("name") == 1u && !reader.column("age").has_value(), "Unexpected column index");
  anch::ut::assertThat(reader.next() && reader.size() == 3 && reader[0] == "1" && reader[1] == "Rex" && reader[2] == "good, dog"
		       && reader.line() == 2, "Unexpected first record");
  anch::ut::assertThat(reader.next() && reader[1] == "Médor \"le chien\"" && reader[2] == "two\r\nlines" && reader.line() == 3,
		       "Unexpected second record '{}' '{}'", reader[1], reader[2]);
  anch::ut::assertThat(reader.next() && reader[2].empty() && reader.line() == 5, "Unexpected third record (line {})", reader.line());
  anch::ut::assertThat(!reader.next() && !reader.next() && input.eof(), "End of input expected");
}

void
testLineEndings() {
  const Records expected{{"a", "b"}, {"c", "d"}, {"e", "f"}};
  anch::ut::assertThat(readAll("a,b\nc,d\ne,f\n") == expected, "LF line endings");
  anch::ut::assertThat(readAll("a,b\r\nc,d\ne,f") == expected, "Mixed line endings, last record without line ending");
  anch::ut::assertThat(readAll("").empty(), "No record in empty input");
  anch::ut::assertThat(readAll("a,b\n") == Records{{"a", "b"}}, "No record after last line ending");
  CsvError error = readError("a,b\rc,d\n");
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::INVALID_FORMAT && error.getLine() == 1, "CR alone has to be refused: {}", error.what());
  anch::ut::assertThat(readAll("\"a\rb\",c\n") == Records{{"a\rb", "c"}}, "CR is accepted in quoted fields");
}

void
testEmptyFields() {
  std::istringstream input(",\"\",x,\n");
  Reader reader(input, noHeader());
  anch::ut::assertThat(reader.next() && reader.size() == 4, "4 fields expected");
  anch::ut::assertThat(reader[0].empty() && !reader.quoted(0) && reader[1].empty() && reader.quoted(1) && !reader.quoted(2)
		       && reader[3].empty() && !reader.quoted(3), "Empty and empty quoted fields have to be distinguished");
  Options tolerant = noHeader();
  tolerant.sameColumnCount = false;
  anch::ut::assertThat(readAll("a\n\nb\n", tolerant) == Records{{"a"}, {""}, {"b"}}, "Empty line is a record with an empty field");
}

void
testBom() {
  anch::ut::assertThat(readAll("\xEF\xBB\xBF" "a,b\n") == Records{{"a", "b"}}, "BOM has to be skipped");
  anch::ut::assertThat(readAll("\xEF\xBB" "a,b\n") == Records{{"\xEF\xBB" "a", "b"}}, "Partial BOM bytes have to be kept");
  // A first field which begins with BOM bytes is quoted by the writer: it is read again
  anch::ut::assertThat(readAll("\"\xEF\xBB\xBF" "a\",b\n") == Records{{"\xEF\xBB\xBF" "a", "b"}}, "Quoted BOM bytes are field content");
  const std::string bom = written([](Writer& writer) { writer.write({"\xEF\xBB\xBF" "a", "b"}); writer.write({"\xEF\xBB\xBF"}); }, noHeader());
  anch::ut::assertThat(bom == "\"\xEF\xBB\xBF" "a\",b\r\n\xEF\xBB\xBF\r\n", "Only the first field has to be quoted");
  std::istringstream input("\xEF\xBB\xBF" "id,name\n1,Rex\n");
  Reader reader(input);
  anch::ut::assertThat(reader.header()[0] == "id", "BOM must not be part of the first column name");
}

void
testOptions() {
  Options options = noHeader();
  options.separator = ';';
  anch::ut::assertThat(readAll("a;\"b;c\";1,5\n", options) == Records{{"a", "b;c", "1,5"}}, "; separator");
  options.separator = '\t';
  options.quote = '\'';
  anch::ut::assertThat(readAll("a\t'b\t''c'''\t\"d\"\n", options) == Records{{"a", "b\t'c'", "\"d\""}}, "Tab separator and ' quote");
  Options invalid;
  invalid.quote = ',';
  bool raised = false;
  try {
    std::istringstream input("");
    Reader reader(input, invalid);
  } catch(const std::invalid_argument&) {
    raised = true;
  }
  anch::ut::assertThat(raised, "Separator and quote have to be different");
  invalid.quote = '\n';
  raised = false;
  try {
    std::ostringstream out;
    Writer writer(out, invalid);
  } catch(const std::invalid_argument&) {
    raised = true;
  }
  anch::ut::assertThat(raised, "LF quote has to be refused");
}

void
testErrors() {
  CsvError error = readError("a,b\nc,d\"e\n");
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::INVALID_FORMAT && error.getLine() == 2 && error.getColumn() == 2,
		       "Quote in unquoted field: {}", error.what());
  anch::ut::assertThat(std::string(error.what()) == "line 2, column 2: quote in an unquoted field", "Unexpected message '{}'", error.what());
  error = readError("\"a\"b,c\n");
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::INVALID_FORMAT && error.getColumn() == 1, "Character after closing quote: {}", error.what());
  error = readError("a,\"b\nc\n");
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::INVALID_FORMAT && error.getLine() == 3, "Unterminated quoted field: {}", error.what());
  error = readError("a,b\nc\n");
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::COLUMN_COUNT && error.getLine() == 2, "Column count: {}", error.what());
  error = readError("id,name\n1\n", Options());
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::COLUMN_COUNT, "Column count of header: {}", error.what());
}

void
testLimits() {
  Options options = noHeader();
  options.maxRecordSize = 8;
  anch::ut::assertThat(readAll("1234567\n1234567\n", options).size() == 2, "Records of maximum size have to be read");
  CsvError error = readError("12345678\n", options);
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::TOO_LARGE, "Record size: {}", error.what());
  error = readError("\"1234567890", options);
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::TOO_LARGE, "Quoted field size: {}", error.what());
  options.maxRecordSize = 0;
  options.maxColumns = 3;
  anch::ut::assertThat(readAll("a,b,c\n", options).size() == 1, "Records with maximum columns have to be read");
  error = readError("a,b,c,d\n", options);
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::TOO_LARGE && error.getColumn() == 4, "Columns: {}", error.what());
  Options total = noHeader();
  total.maxSize = 8;
  anch::ut::assertThat(readAll("abc\ndef\n", total).size() == 2, "Input of maximum size has to be read");
  error = readError("abc\ndef\ngh\n", total);
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::TOO_LARGE && error.getLine() == 3, "Input size: {}", error.what());
}

void
testWriter() {
  std::string csv = written([](Writer& writer) {
    writer.write({"id", "name", "comment"});
    writer.write({"1", "Rex", "good, dog"});
    writer.write({"2", "Médor \"le chien\"", "two\nlines"});
    writer.write({"3", "", "a\rb"});
  });
  anch::ut::assertThat(csv == "id,name,comment\r\n1,Rex,\"good, dog\"\r\n2,\"Médor \"\"le chien\"\"\",\"two\nlines\"\r\n3,,\"a\rb\"\r\n",
		       "Unexpected CSV '{}'", csv);
  Options options;
  options.crlf = false;
  options.bom = true;
  options.separator = ';';
  csv = written([](Writer& writer) {
    writer.writeRow("x;y", 42, -1.5, true, 'c', std::string("s"), static_cast<uint64_t>(std::numeric_limits<uint64_t>::max()));
    writer.writeRow();
  }, options);
  anch::ut::assertThat(csv == "\xEF\xBB\xBF\"x;y\";42;-1.5;true;c;s;18446744073709551615\n\n", "Unexpected CSV '{}'", csv);
}

void
testRoundTrip() {
  const Records records{{"plain", "with,separator", "with \"quotes\"", "\"", "line\r\nending", "", "   spaces  "},
			{"é", ",", "\"\"", "\n", "\r", "a\"b", ""}};
  for(char separator : {',', ';', '\t'}) {
    Options options = noHeader();
    options.separator = separator;
    std::string csv = written([&records](Writer& writer) {
      for(const auto& record : records) {
	std::vector<std::string_view> fields(record.begin(), record.end());
	writer.write(fields);
      }
    }, options);
    anch::ut::assertThat(readAll(csv, options) == records, "Round trip with '{}' separator", separator);
  }
}

void
testStream() {
  // Records are read one by one: the record buffer is reused
  std::stringstream content;
  {
    Writer writer(content);
    writer.write({"id", "name"});
    for(int i = 0; i < 100000; ++i) {
      writer.writeRow(i, "name " + std::to_string(i));
    }
  }
  Reader reader(content);
  std::size_t count = 0;
  long long sum = 0;
  while(reader.next()) {
    sum += std::stoll(std::string(reader[0]));
    ++count;
  }
  anch::ut::assertThat(count == 100000 && sum == 4999950000LL, "Unexpected records {} {}", count, sum);
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH CSV unit tests")
    .description("Test AnCH CSV reader and writer")
    .add("rfc4180", testRfc4180)
    .add("line-endings", testLineEndings)
    .add("empty-fields", testEmptyFields)
    .add("bom", testBom)
    .add("options", testOptions)
    .add("errors", testErrors)
    .add("limits", testLimits)
    .add("writer", testWriter)
    .add("round-trip", testRoundTrip)
    .add("stream", testStream)
    ;
}
