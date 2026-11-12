/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2012 Vincent Lachenal

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
#include "log/fileWriter.hpp"

#include <charconv>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <system_error>

using anch::log::FileWriter;

namespace {

  /*! Size units (powers of 1024) */
  const std::map<char, std::uint64_t> SIZE_UNITS = {
    {'K', 1024ull},
    {'M', 1024ull * 1024},
    {'G', 1024ull * 1024 * 1024},
    {'T', 1024ull * 1024 * 1024 * 1024}
  };

  /*!
   * Parse file size: number with optional unit (K, M, G or T)
   *
   * \param value the value to parse
   *
   * \return the size in bytes
   *
   * \throw std::invalid_argument invalid size
   */
  std::uint64_t
  parseSize(const std::string& value) {
    std::uint64_t num = 0;
    const char* end = value.data() + value.size();
    auto [ptr, ec] = std::from_chars(value.data(), end, num);
    if(ec != std::errc() || (end - ptr) > 1) {
      throw std::invalid_argument("Invalid max size: " + value);
    }
    std::uint64_t factor = 1;
    if(ptr != end) {
      auto unit = SIZE_UNITS.find(*ptr);
      if(unit == SIZE_UNITS.end()) {
	throw std::invalid_argument("Invalid max size unit: " + value);
      }
      factor = unit->second;
    }
    if(num > std::numeric_limits<std::uint64_t>::max() / factor) {
      throw std::invalid_argument("Max size is too big: " + value);
    }
    return num * factor;
  }

}

FileWriter::FileWriter(const anch::conf::Section& conf):
  anch::log::Writer(conf),
  _output(),
  _path(),
  _maxSize(0),
  _maxFiles(1) {
  // Check file path +
  std::optional<std::string> strPath = conf.getValue<std::string>("path");
  if(!strPath.has_value()) {
    throw std::invalid_argument("File path not found");
  }
  _path = strPath.value();
  if(!_path.has_filename()) {
    throw std::invalid_argument("Path is not a file");
  }
  auto folder = _path.parent_path();
  if(!folder.empty() && !std::filesystem::exists(folder)) {
    throw std::invalid_argument("Directory does not exist");
  }
  // Check file path -

  // Parse rotation configuration +
  std::optional<std::string> maxSize = conf.getValue<std::string>("max-size");
  if(maxSize.has_value()) {
    _maxSize = parseSize(maxSize.value());
  }
  int maxFiles = conf.getValue<int>("max-rotate-files", 1);
  if(maxFiles < 0) {
    throw std::invalid_argument("Invalid max rotate files: " + std::to_string(maxFiles));
  }
  _maxFiles = static_cast<std::uint32_t>(maxFiles);
  // Parse rotation configuration -

  _output.open(_path, std::ios_base::app);
  if(!_output.is_open()) {
    throw std::invalid_argument("Unable to open log file " + _path.string());
  }
}

FileWriter::~FileWriter() {
  // Nothing to do: the file is flushed and closed by the stream destructor
}

void
FileWriter::write(const std::string& message) {
  writeLine(format(message));
  _output.flush(); // do not lose messages on crash
}

void
FileWriter::writeLine(const std::string& line) {
  if(!_output.is_open()) {
    return;
  }
  _output << line << '\n';
  if(rotate()) {
    rotateFiles();
  }
}

bool
FileWriter::rotate() {
  if(_maxSize == 0) {
    return false;
  }
  std::streamoff size = _output.tellp();
  return size >= 0 && static_cast<std::uint64_t>(size) >= _maxSize;
}

std::filesystem::path
FileWriter::archive(std::uint32_t index) const {
  std::filesystem::path path = _path;
  path += "." + std::to_string(index);
  return path;
}

void
FileWriter::rotateFiles() {
  _output.close();
  bool archived = false;
  if(_maxFiles > 0) {
    std::error_code error;
    // Shift archives: <path>.<max-1> is removed, <path>.<i> becomes <path>.<i+1>
    std::filesystem::remove(archive(_maxFiles - 1), error);
    for(std::uint32_t i = _maxFiles - 1 ; i > 0 ; --i) {
      std::filesystem::path previous = archive(i - 1);
      if(std::filesystem::exists(previous, error)) {
	std::filesystem::rename(previous, archive(i), error);
      }
    }
    error.clear();
    std::filesystem::rename(_path, archive(0), error);
    archived = !error;
  }
  // Truncate when the file has been archived or when there is no archive ; otherwise keep appending
  _output.open(_path, (archived || _maxFiles == 0) ? std::ios_base::trunc : std::ios_base::app);
}
