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
#include "codec/csv.hpp"

#include "codec/format.hpp"

#include "csv/csvError.hpp"

using anch::codec::Csv;
using anch::codec::FormatPolicy;

template struct anch::codec::FormatCodec<anch::csv::CsvFormat>;

namespace {

  // Registered when the library is loaded: a program which uses Csv links this file (options)
  [[maybe_unused]] const bool REGISTERED = anch::codec::registerFormat(anch::codec::Format{"csv", {std::string(Csv::mediaType())}});

}

void
FormatPolicy<anch::csv::CsvFormat>::configure(anch::csv::Options& options, anch::codec::Side side) {
  if(side == anch::codec::Side::SERVER) {
    options.readCheckConstraints = true;
    options.writeCheckConstraints = false;
    if(options.maxSize == 0) {
      options.maxSize = 2 * 1024 * 1024; // request bodies are bounded, as JSON ones
    }
  } else {
    options.readCheckConstraints = false;
    options.writeCheckConstraints = true;
    options.ignoreUnknownColumns = true; // server responses are trusted: newer columns are ignored
  }
}

void
FormatPolicy<anch::csv::CsvFormat>::translate(const std::exception& error) {
  const auto* csv = dynamic_cast<const anch::csv::CsvError*>(&error);
  if(csv != nullptr && csv->getErrorCode() == anch::csv::ErrorCode::TOO_LARGE) {
    throw anch::codec::ContentTooLarge(csv->what());
  }
}
