/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2020 Vincent Lachenal

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
#include <iostream>

#include "json/reader.hpp"
#include "json/writerContext.hpp"
#include "json/constants.hpp"

#include "cli/args.hpp"
#include "cli/utils.hpp"
#include "cli/formatter.hpp"
#include "events/observer.hpp"

using anch::json::Reader;
using anch::json::ReaderContext;
using anch::json::WriterContext;
using anch::json::MappingOptions;
using anch::json::MappingError;
using anch::json::JSONItem;
using anch::json::EventType;

/*!
 * JSON formatter on event
 *
 * \author Vincent Lachenal
 */
class JSONFormatter: public anch::events::Observer<JSONItem> {

private:
  /*! The context writer */
  anch::json::WriterContext _writer;

  /*! Current field */
  std::optional<std::string> _currentField;

  /*! In array flag */
  uint64_t _array;

public:
  /*!
   * Forbids \c JSONFormatter default constructor
   */
  JSONFormatter() = delete;

  /*!
   * Forbids \c JSONFormatter copy constructor
   *
   * \param formatter the JSON formatter not to copy
   */
  JSONFormatter(const JSONFormatter& formatter) = delete;

  /*!
   * Forbids \c JSONFormatter move constructor
   *
   * \param formatter the JSON formatter not to move
   */
  JSONFormatter(JSONFormatter&& formatter) = delete;

  /*!
   * \c JSONFormatter constructor
   *
   * \param output the ouput stream
   * \param options the mapping options
   * \param color the output color options
   */
  JSONFormatter(std::ostream& output, const MappingOptions& options, bool color):
    _writer(output, options), _currentField(), _array(0) {
    anch::cli::Formatter::DISABLED = !color;
  }

public:
  /*!
   * Serialize field when set
   */
  inline
  void addField() {
    if(_currentField.has_value()) {
      _writer.writeField(_currentField.value());
    } else if(_array > 0) {
      _writer.next();
    }
  }

  /*!
   * Write data in output stream according to options
   *
   * \param event the event to treat
   */
  virtual void handle(const anch::events::Event<JSONItem>& event) noexcept {
    switch(event.body.getType()) {
    case EventType::FIELD:
      {
	std::ostringstream oss;
	oss << anch::cli::Formatter::format().bold() << std::any_cast<std::string>(event.body.getValue()) << anch::cli::RESET;
	_currentField = oss.str();
      }
      break;
    case EventType::VNULL:
      if(_currentField.has_value()) {
	_writer.writeNull(_currentField.value());
	_currentField.reset();
      } else {
	_writer.next();
      }
      break;
    case EventType::TRUE:
      addField();
      _writer.output << anch::cli::Formatter::format().fgColor(anch::cli::Color::GREEN) << "true" << anch::cli::RESET;
      _currentField.reset();
      break;
    case EventType::FALSE:
      addField();
      _writer.output << anch::cli::Formatter::format().fgColor(anch::cli::Color::RED) << "false" << anch::cli::RESET;
      _currentField.reset();
      break;
    case EventType::NUMBER:
      addField();
      _writer.output << anch::cli::Formatter::format().fgColor(anch::cli::Color::YELLOW) << std::any_cast<double>(event.body.getValue()) << anch::cli::RESET;
      _currentField.reset();
      break;
    case EventType::STRING:
      addField();
      _writer.output << anch::json::STRING_DELIMITER
		     << anch::cli::Formatter::format().fgColor(anch::cli::Color::BLUE) << std::any_cast<std::string>(event.body.getValue()) << anch::cli::RESET
		     << anch::json::STRING_DELIMITER;
      _currentField.reset();
      break;
    case EventType::BEGIN_OBJECT:
      addField();
      _writer.beginObject();
      _currentField.reset();
      break;
    case EventType::END_OBJECT:
      _writer.endObject();
      _currentField.reset();
      break;
    case EventType::BEGIN_ARRAY:
      addField();
      _writer.beginArray();
      _currentField.reset();
      ++_array;
      break;
    case EventType::END_ARRAY:
      _writer.endArray();
      --_array;
      _currentField.reset();
      break;
    }
  }

};

/*!
 * Mapping error handler
 *
 * \author Vincent Lachenal
 */
class ErrorObserver: public anch::events::Observer<anch::json::MappingError> {

  /*!
   * Handle JSON parser error\n
   * Print and exit
   *
   * \param event the error event
   */
  virtual void handle(const anch::events::Event<anch::json::MappingError>& event) noexcept {
    std::cerr << "Parsing error: " << event.body.what() << std::endl;
    std::exit(1);
  }

};

/*!
 * CLI JSON formatter options
 *
 * \autor Vincent Lachenal
 */
struct FormatterOptions {

  /*! The input stream to read */
  std::shared_ptr<std::istream> input;

  /*! The output stream to write in */
  std::shared_ptr<std::ostream> output;

  /*! Color output */
  bool color = false;

};

/*!
 * Parse JSON format application options
 *
 * \param argc the number of CLI arguments
 * \param argv the arguments values
 * \param opts the data options (input and output)
 * \param jsonOpts the JSON mapper options
 */
void
parseArgs(int argc, char** argv, FormatterOptions& opts, MappingOptions& jsonOpts) {
  std::vector<anch::cli::Arg> args = {
    {.handler = anch::cli::bindTrue(opts.color), .sopt = 'c', .lopt = "color", .description = "Activate console color (default to false)"},
    {.handler = anch::cli::bindTrue(jsonOpts.serialize_null), .lopt = "null", .description = "Serialize null value (default to false)"},
    {.handler = anch::cli::bindNum(jsonOpts.prettify_nbs), .sopt = 'n', .lopt = "nb-indent", .value = true, .name = "N", .description = "Number of spaces for indentation (default to 0)"},
    {.handler = anch::cli::bindIFS(opts.input), .sopt = 'i', .lopt = "input", .value = true, .name = "IS", .pipe = anch::cli::bindPipe(opts.input), .description = "Input file (default to stdin)"},
    {.handler = anch::cli::bindOFS(opts.output), .sopt = 'o', .lopt = "output", .value = true, .name = "OS", .cout = anch::cli::bindCout(opts.output), .description = "Output file (default to stdout)"}
  };
  anch::cli::ArgHandler handler(args);
  handler.handle(argc, argv);
}

/*!
 * Initalize parse and writer and let's go !
 *
 * \param argc the number of arguments
 * \param argv the arguments' value
 */
int
main(int argc, char** argv) {
  FormatterOptions args;
  MappingOptions options;
  options.deserialize_max_discard_char = -1;
  parseArgs(argc, argv, args, options);
  Reader reader(*args.input, options);
  JSONFormatter formatter(*args.output, options, args.color);
  reader.itemObs().addObserver(formatter);
  ErrorObserver error;
  reader.errorObs().addObserver(error);
  reader.parse();
  std::cout << std::endl;
  return 0;
}
