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
#pragma once

#include <string>
#include <optional>
#include <map>
#include <set>
#include <functional>

#include "json/readerContext.hpp"
#include "json/mappingError.hpp"

namespace anch::json {

  // Types' definitions +
  /*! Parse and add item to array function (return \c true when ']' has been found, \c false otherwise) */
  using AddItem = std::function<bool(anch::json::ReaderContext&)>;

  /*! Parse and insert item to map */
  using PushItem = std::function<void(const std::string&, anch::json::ReaderContext&)>;

  /*! Maximum signed integer precision declaration */
  using jint = int64_t;

  /*! Maximum unsigned integer precision declaration */
  using juint = uint64_t;

  /*! Maximum float precision declaration */
  using jfloat = long double;
  // Types' definitions -

  // Boolean lexer functions +
  /*!
   * 'true' value lexer.
   *
   * \param context the parser context
   *
   * \throw anch::json::MappingError parsing error
   */
  void lexTrue(anch::json::ReaderContext& context);

  /*!
   * 'false' value lexer.
   *
   * \param context the parser context
   *
   * \throw anch::json::MappingError parsing error
   */
  void lexFalse(anch::json::ReaderContext& context);

  /*!
   * Boolean value lexer.\n
   * Possible values are: \c null, \c true and \c false
   *
   * \param value the boolean value to store in (not set when \c null has been found)
   * \param context the parser context
   *
   * \return \c false when \c null , \c true otherwise
   *
   * \throw anch::json::MappingError parsing error
   */
  bool lexBoolean(bool& value, anch::json::ReaderContext& context);
  // Boolean lexer functions -

  // Integer lexer functions +
  /*!
   * Parse decimale digits and store them to context value's buffer
   *
   * \param context the context
   */
  void parseDecimal(anch::json::ReaderContext& context);

  /*!
   * Signed integer value lexer.\n
   * Possible values are: \c null and signed integer
   *
   * \param value the signed value to store in (not set when \c null has been found)
   * \param context the parser context
   *
   * \return \c false when \c null , \c true otherwise
   *
   * \throw anch::json::MappingError parsing error
   */
  bool lexInteger(anch::json::jint& value, anch::json::ReaderContext& context);

  /*!
   * Unsigned integer value lexer.\n
   * Possible values are: \c null and unsigned integer (min value is always 0)
   *
   * \param value the unsigned value to store in (not set when \c null has been found)
   * \param context the parser context
   *
   * \return \c false when \c null , \c true otherwise
   *
   * \throw anch::json::MappingError parsing error
   */
  bool lexUInteger(anch::json::juint& value, anch::json::ReaderContext& context);
  // Integer lexer functions -

  // Number lexer functions +
  /*!
   * Parse float.\n
   * End parsing at first unrecognized character.
   *
   * \param context the parser context
   */
  void parseNumber(anch::json::ReaderContext& context);

  /*!
   * Floating integer (double) value lexer.\n
   * Possible values are: \c null and floating integer value (can parse exponential representation).
   *
   * \param value the value to write in (not set when \c null has been found)
   * \param context the parser context
   *
   * \return \c false when \c null , \c true otherwise
   *
   * \throw anch::json::MappingError parsing error
   */
  bool lexNumber(double& value, anch::json::ReaderContext& context);
  // Number lexer functions -

  // String lexer functions +
  /*!
   * Lex string content (between the '"').\n
   * Escape sequences are decoded (\c \\uXXXX sequences, including UTF-16 surrogate pairs, are converted to UTF-8).\n
   * The opening '"' has to be already consumed.
   *
   * \param value the string to write in (cleared before)
   * \param context the JSON context
   *
   * \throw anch::json::MappingError invalid escape sequence or end of data before end of string
   */
  void lexStringContent(std::string& value, anch::json::ReaderContext& context);

  /*!
   * Lex string content (between the '"').\n
   * The result will be stored in the context buffer (see \ref lexStringContent(std::string&, anch::json::ReaderContext&) ).
   *
   * \param context the JSON context
   *
   * \throw anch::json::MappingError invalid escape sequence or end of data before end of string
   */
  void lexStringContent(anch::json::ReaderContext& context);

  /*!
   * String lexer function.\n
   * This lexer will look for " character to begin and end with ".\n
   * If begin character is not found, field will be empty.\n
   * If " end character is not found (according to options and stream), it will raised exception
   *
   * \param value the string value to fill when found
   * \param context the parser context
   *
   * \return \c false when \c null has been found, \c true otherwise
   *
   * \throw anch::json::MappingError parsing error
   */
  bool lexString(std::string& value, anch::json::ReaderContext& context);

  /*!
   * Field lexer function.\n
   * This lexer will look for " character to begin and end with ".\n
   * If begin character is not found, field will be empty.\n
   * If " end character is not found (according to options and stream), it will raised exception
   *
   * \param field the field to fill when found
   * \param context the parser context
   *
   * \return \c true when found, \c false otherwise
   *
   * \throw anch::json::MappingError parsing error
   */
  bool lexField(std::string& field, anch::json::ReaderContext& context);
  // String lexer functions -

  // Generic lexer functions +
  /*!
   * Fail on any discardable character
   *
   * \param context the parser context
   *
   * \return the not discardable character
   *
   * \throw anch::json::MappingError discardable charater has been found
   */
  char discardFail(anch::json::ReaderContext& context);

  /*!
   * Consume stream until unescapable character
   *
   * \param context the parser context
   *
   * \return the not discardable character
   *
   * \throw anch::json::MappingError maximum number of discardable charater has been reached
   */
  char lexDiscard(anch::json::ReaderContext& context);

  /*!
   * 'null' value lexer.\n
   *
   * \param context the parser context
   *
   * \throw anch::json::MappingError parsing error
   */
  void lexNull(anch::json::ReaderContext& context);

  /*!
   * Check if object has value (start with '{') or is \c null
   *
   * \param context the parser context
   *
   * \return object has value
   *
   * \throw anch::json::MappingError parsing error
   */
  bool objectHasValueLex(anch::json::ReaderContext& context);

  /*!
   * Object lexer function.\n
   * The opening \c '{' has to be already consumed (see \ref objectHasValueLex).\n
   * \c onField is called for each found field with the field name and the context positioned
   * just after \c ':'. It has to consume the field value.
   *
   * \tparam F the field handler type (callable as \c void(const std::string&, anch::json::ReaderContext&))
   *
   * \param onField the field handler
   * \param context the parser context
   *
   * \throw anch::json::MappingError parsing error
   */
  template<typename F>
  void lexObject(F&& onField, anch::json::ReaderContext& context);
  // Generic lexer functions -

  // Container lexer functions +
  /*!
   * Parse array
   *
   * \tparam F the item parser type
   *
   * \param addFunc parse and add elem to array function (callable as \c bool(anch::json::ReaderContext&))
   * \param context the context
   *
   * \throw anch::json::MappingError parsing error
   */
  template<typename F>
  void lexArray(F&& addFunc, anch::json::ReaderContext& context);

  /*!
   * Parse map
   *
   * \tparam F the item parser type
   *
   * \param pushFunc parse and add elem to map function (callable as \c void(const std::string&, anch::json::ReaderContext&))
   * \param context the context
   *
   * \throw anch::json::MappingError parsing error
   */
  template<typename F>
  void lexMap(F&& pushFunc, anch::json::ReaderContext& context);
  // Container lexer functions -

  // Unknown lexer functions +
  /*!
   * Parse unknown field value
   *
   * \param context the context
   *
   * \throw anch::json::MappingError parsing error
   */
  void lexUnknown(anch::json::ReaderContext& context);

  /*!
   * Raise \c anch::json::MappingError
   *
   * \param context the context
   *
   * \throw anch::json::MappingError parsing error
   */
  void lexUnknownFail(anch::json::ReaderContext& context);
  // Unknown lexer functions +

}

#include "json/impl/lexer.hpp"
