/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "log/fileWriter.hpp"

#include <stdexcept>
#include <regex>

using anch::log::FileWriter;


std::map<std::string, uint32_t> SIZES = std::map<std::string, uint32_t>({
    {"K", 1024},
    {"M", 1024*1024},
    {"G", 1024*1024*1024}
  });
const std::regex UNITS_REGEX = std::regex("^([0-9]+)(K|M|G|T)?$");


FileWriter::FileWriter(const anch::conf::Section& conf):
  anch::log::Writer(conf),
  _output(NULL),
  _path(),
  _maxSize(0),
  _maxIdx(-1),
  _fileIdx(-1) {
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
  if(!std::filesystem::exists(folder)) {
    throw std::invalid_argument("Directory does not exist");
  }
  _output = new std::ofstream(_path, std::ios_base::app);
  // Check file path -

  _fileIdx = 0;
  // \todo retrieve current file index

  // Parse maximum file size +
  std::optional<std::string> maxSize = conf.getValue<std::string>("max-size");
  if(maxSize.has_value()) {
    std::smatch items;
    if(!std::regex_match(maxSize.value(), items, UNITS_REGEX)) {
      throw std::invalid_argument("Invalid max size");
    }
    uint32_t num = static_cast<uint32_t>(std::atoi(items[1].str().data()));
    uint32_t factor = 1;
    if(items.length() > 2) {
      factor = SIZES[items[2].str()];
    }
    _maxSize = num * factor;
  }
  // Parse maximum file size -

  // Parse maximum file index +
  std::optional<int> maxIdx = conf.getValue<int>("max-idx");
  if(maxIdx.has_value()) {
    _maxIdx = maxIdx.value();
  }
  // Parse maximum file index -
}

FileWriter::~FileWriter() {
  if(_output != NULL) { // release file when open
    _output->flush();
    delete _output; // it will close the file
    _output = NULL;
  }
}

void
FileWriter::write(const std::string& message) {
  if(_output == NULL) {
    return;
  }
  *_output << format(message) << '\n';
  if(rotate()) {
    rotateFiles();
  }
}

bool
FileWriter::rotate() const {
  return (_maxSize > 0 && _output->tellp() >= _maxSize);
}

void
FileWriter::rotateFiles() {
  if(_output != NULL) {
    _output->flush();
    delete _output; // It will close output
    _output = NULL;
  }
  std::ostringstream ostr;
  bool maxReached = false;
  // Rename already rotated files +
  for(int i = _fileIdx ; i > 0 ; --i) {
    std::filesystem::path curPath = _path;
    ostr << "." << i;
    curPath += ostr.str();
    if(!std::filesystem::exists(curPath)) { // when current path does not exist, decrement file index
      --_fileIdx;
      continue;
    }
    if(i >= _maxIdx) { // when max index has been reached, remove file
      std::filesystem::remove(curPath);
      maxReached = true;
      continue;
    }
    ostr.str("");
    std::filesystem::path newPath = _path;
    ostr << "." << (i + 1);
    newPath += ostr.str();
    std::filesystem::rename(curPath, newPath);
  }
  if(maxReached) {
    _fileIdx = _maxIdx - 1;
  }
  // Rename already rotated files -
  // Rename current file +
  std::filesystem::path newPath = _path;
  newPath += ".1";
  std::filesystem::rename(_path, newPath);
  _output = new std::ofstream(_path);
  ++_fileIdx;
  // Rename current file -
}
