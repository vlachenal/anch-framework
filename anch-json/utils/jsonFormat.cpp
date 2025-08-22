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
   */
  JSONFormatter(std::ostream& output, const MappingOptions& options):
    _writer(output, options), _currentField(), _array(0) {
    // Nothing to do
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
      _currentField = std::any_cast<std::string>(event.body.getValue());
      break;
    case EventType::VNULL:
      addField();
      _currentField.reset();
      break;
    case EventType::TRUE:
      addField();
      _writer.output << "true";
      _currentField.reset();
      break;
    case EventType::FALSE:
      addField();
      _writer.output << "false";
      _currentField.reset();
      break;
    case EventType::NUMBER:
      addField();
      _writer.output << std::any_cast<double>(event.body.getValue());
      _currentField.reset();
      break;
    case EventType::STRING:
      addField();
      _writer.output << anch::json::STRING_DELIMITER << std::any_cast<std::string>(event.body.getValue()) << anch::json::STRING_DELIMITER;
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
parseArgs(int argc, char** argv, FormatterOptions& opts, MappingOptions jsonOpts) {
  std::vector<anch::cli::Arg> args = {
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
  JSONFormatter formatter(*args.output, options);
  reader.itemObs().addObserver(formatter);
  ErrorObserver error;
  reader.errorObs().addObserver(error);
  reader.parse();
  std::cout << std::endl;
  return 0;
}
