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

// JSON token event handler +
/*!
 * JSON formatter on event
 *
 * \todo manage array value more precisely
 *
 * \author Vincent Lachenal
 */
class JSONFormatter: public anch::events::Observer<JSONItem> {

  // Attributes +
private:
  /*! The context writer */
  anch::json::WriterContext _writer;

  /*! Current field */
  std::optional<std::string> _currentField;

  /*! In array flag */
  uint64_t _array;

  /*! Write null value (use for array value) */
  bool _null;
  // Attributes -

  // Constructors +
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
  JSONFormatter(std::ostream& output, const MappingOptions& options, bool color, bool null):
    anch::events::Observer<JSONItem>(), _writer(output, options), _currentField(), _array(0), _null(null) {
    anch::cli::Formatter::DISABLED = !color;
  }
  // Constructors -

  // Destructor +
public:
  virtual ~JSONFormatter() {
    // Nothing to do
  }
  // Destructor -

  // Methods +
public:
  /*!
   * Serialize field when set
   */
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
    case EventType::FIELD: // Manage field
      {
	std::ostringstream oss;
	oss << anch::cli::Formatter::format().bold() << std::any_cast<std::string>(event.body.getValue()) << anch::cli::RESET;
	_currentField = oss.str();
      }
      break;

    case EventType::VNULL: // Manage null value
      if(_currentField.has_value()) {
	_writer.writeNull(_currentField.value());
	_currentField.reset();
      } else if(_null) {
	//_writer.next();
	_writer.output << "null";
      }
      break;

    case EventType::TRUE: // Manage boolean true value
      addField();
      _writer.output << anch::cli::Formatter::format().fgColor(anch::cli::Color::GREEN) << "true" << anch::cli::RESET;
      _currentField.reset();
      break;

    case EventType::FALSE: // Manage boolean false value
      addField();
      _writer.output << anch::cli::Formatter::format().fgColor(anch::cli::Color::RED) << "false" << anch::cli::RESET;
      _currentField.reset();
      break;

    case EventType::NUMBER: // Manage number value
      addField();
      _writer.output << anch::cli::Formatter::format().fgColor(anch::cli::Color::YELLOW) << std::any_cast<double>(event.body.getValue()) << anch::cli::RESET;
      _currentField.reset();
      break;

    case EventType::STRING: // Manage character string value
      addField();
      _writer.output << anch::json::STRING_DELIMITER
		     << anch::cli::Formatter::format().fgColor(anch::cli::Color::BLUE) << std::any_cast<std::string>(event.body.getValue()) << anch::cli::RESET
		     << anch::json::STRING_DELIMITER;
      _currentField.reset();
      break;

    case EventType::BEGIN_OBJECT: // Manage begin object token ({)
      addField();
      _writer.beginObject();
      _currentField.reset();
      break;

    case EventType::END_OBJECT: // Manage end object token (})
      _writer.endObject();
      _currentField.reset();
      break;

    case EventType::BEGIN_ARRAY: // Manage begin array token ([)
      addField();
      _writer.beginArray();
      _currentField.reset();
      ++_array;
      break;

    case EventType::END_ARRAY: // Manage end array token (])
      _writer.endArray();
      --_array;
      _currentField.reset();
      break;
    }
  }
  // Methods -

};
// JSON token event handler -

// Error handling +
/*!
 * Mapping error handler
 *
 * \author Vincent Lachenal
 */
class ErrorObserver: public anch::events::Observer<anch::json::MappingError> {

  // Constructors +
public:
  /*!
   * \ref ErrorObserver default constructor
   */
  ErrorObserver(): anch::events::Observer<anch::json::MappingError>() {
    // Nothing to do
  }

  /*!
   * Forbids \c ErrorObserver copy constructor
   *
   * \param handler the JSON error handler not to copy
   */
  ErrorObserver(const ErrorObserver& handler) = delete;

  /*!
   * Forbids \c ErrorObserver move constructor
   *
   * \param handler the JSON error handler not to move
   */
  ErrorObserver(ErrorObserver&& handler) = delete;
  // Constructors -

  // Destructor +
public:
  /*!
   * \ref ErrorObserver destructor
   */
  virtual ~ErrorObserver() {
    // Nothing to do
  }
  // Destructor -

  // Methods +
public:
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
  // Methods -

};
// Error handling -

// Arguments parser declaration +
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
    {.handler = anch::cli::bindNum(jsonOpts.buffer_size), .sopt = 'b', .lopt = "buffer-size", .value = true, .name = "SIZE", .description = "Buffer size in byte (default to anch-json = 1024)"},
    {.handler = anch::cli::bindIFS(opts.input), .sopt = 'i', .lopt = "input", .value = true, .name = "IS", .pipe = anch::cli::bindPipe(opts.input), .description = "Input file (default to stdin)"},
    {.handler = anch::cli::bindOFS(opts.output), .sopt = 'o', .lopt = "output", .value = true, .name = "OS", .cout = anch::cli::bindCout(opts.output), .description = "Output file (default to stdout)"}
  };
  anch::cli::ArgHandler handler(args);
  handler.handle(argc, argv);
}
// Arguments parser declaration -

// Application +
/*!
 * Initalize reader and writer and let's go !
 *
 * \param argc the number of arguments
 * \param argv the arguments' value
 */
int
main(int argc, char** argv) {
  // Manage options +
  FormatterOptions args;
  MappingOptions options;
  options.deserialize_max_discard_char = -1;
  parseArgs(argc, argv, args, options);
  // Manage options -
  // Read and format JSON +
  Reader reader(*args.input, options);
  JSONFormatter formatter(*args.output, options, args.color, options.serialize_null);
  reader.itemObs().addObserver(formatter);
  ErrorObserver error;
  reader.errorObs().addObserver(error);
  reader.parse();
  // Read and format JSON -
  if(args.output->rdbuf() == std::cout.rdbuf()) { // add new line when output is std::cout
    std::cout << std::endl;
  }
  return 0;
}
// Application -
