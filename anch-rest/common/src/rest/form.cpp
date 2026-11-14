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
#include "rest/form.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <system_error>

using anch::rest::Form;
using anch::rest::Headers;
using anch::rest::MediaType;
using anch::rest::Part;

// File content +
struct Part::FileContent {
  /*! File path */
  std::filesystem::path path;

  /*! File size */
  std::size_t size;

  /*! Temporary file: deleted with this content */
  bool temporary;

  FileContent(const std::filesystem::path& file, bool deleted):
    path(file),
    size(static_cast<std::size_t>(std::filesystem::file_size(file))),
    temporary(deleted) {
    // Nothing to do
  }

  FileContent(const FileContent&) = delete;

  FileContent& operator=(const FileContent&) = delete;

  ~FileContent() {
    if(temporary) {
      std::error_code ignored;
      std::filesystem::remove(path, ignored);
    }
  }
};
// File content -

// Part +
Part::Part(): Part(std::string_view(), std::string()) {
  // Nothing to do
}

Part::Part(std::string_view name, std::string content):
  _name(name),
  _filename(),
  _mediaType("text", "plain"),
  _headers(),
  _content(std::move(content)),
  _file(),
  _loaded(false) {
  // Nothing to do
}

Part::Part(std::string_view name, std::string_view filename, MediaType mediaType, std::string content):
  _name(name),
  _filename(std::string(filename)),
  _mediaType(std::move(mediaType)),
  _headers(),
  _content(std::move(content)),
  _file(),
  _loaded(false) {
  // Nothing to do
}

Part
Part::fromFile(std::string_view name, const std::filesystem::path& path, MediaType mediaType, std::optional<std::string_view> filename) {
  // Size is read before the part is built: an unknown file is not deleted
  auto file = std::make_shared<const FileContent>(path, false);
  Part part(name, filename ? *filename : std::string_view(path.filename().native()), std::move(mediaType), std::string());
  part._file = std::move(file);
  return part;
}

Part
Part::fromTemporaryFile(std::string_view name, const std::filesystem::path& path, MediaType mediaType,
			std::optional<std::string_view> filename) {
  std::shared_ptr<const FileContent> file;
  try {
    file = std::make_shared<const FileContent>(path, true);
  } catch(...) {
    std::error_code ignored;
    std::filesystem::remove(path, ignored);
    throw;
  }
  Part part = filename ? Part(name, *filename, std::move(mediaType), std::string()) : Part(name, std::string());
  if(!filename) {
    part.setMediaType(std::move(mediaType));
  }
  part._file = std::move(file);
  return part;
}

std::string_view
Part::content() const {
  if(_file && !_loaded) {
    std::ifstream input(_file->path, std::ios::binary);
    if(!input) {
      throw std::runtime_error("Unable to read part file '" + _file->path.string() + "'");
    }
    _content.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    _loaded = true;
  }
  return _content;
}

std::unique_ptr<std::istream>
Part::open() const {
  if(_file) {
    auto input = std::make_unique<std::ifstream>(_file->path, std::ios::binary);
    if(!*input) {
      throw std::runtime_error("Unable to read part file '" + _file->path.string() + "'");
    }
    return input;
  }
  return std::make_unique<std::istringstream>(_content);
}

std::size_t
Part::size() const noexcept {
  return _file ? _file->size : _content.size();
}

bool
Part::isInMemory() const noexcept {
  return !_file;
}

std::optional<std::filesystem::path>
Part::getPath() const {
  if(!_file) {
    return std::nullopt;
  }
  return _file->path;
}

bool
Part::operator==(const Part& other) const {
  if(_name != other._name || _filename != other._filename || !(_mediaType == other._mediaType) || _headers != other._headers
     || size() != other.size()) {
    return false;
  }
  if(isInMemory() && other.isInMemory()) {
    return _content == other._content;
  }
  if(_file && _file == other._file) {
    return true;
  }
  // Compare streams by blocks
  std::unique_ptr<std::istream> left = open();
  std::unique_ptr<std::istream> right = other.open();
  std::array<char, 8192> leftBuffer;
  std::array<char, 8192> rightBuffer;
  while(*left && *right) {
    left->read(leftBuffer.data(), leftBuffer.size());
    right->read(rightBuffer.data(), rightBuffer.size());
    if(left->gcount() != right->gcount()
       || !std::equal(leftBuffer.begin(), leftBuffer.begin() + left->gcount(), rightBuffer.begin())) {
      return false;
    }
  }
  return true;
}

bool
Part::isFile() const noexcept {
  return _filename.has_value();
}

const std::string&
Part::getName() const noexcept {
  return _name;
}

void
Part::setName(std::string_view name) {
  _name = name;
}

std::optional<std::string_view>
Part::getFilename() const noexcept {
  if(!_filename) {
    return std::nullopt;
  }
  return std::string_view(*_filename);
}

const MediaType&
Part::getMediaType() const noexcept {
  return _mediaType;
}

void
Part::setMediaType(MediaType mediaType) {
  _mediaType = std::move(mediaType);
}

Headers&
Part::getHeaders() noexcept {
  return _headers;
}

const Headers&
Part::getHeaders() const noexcept {
  return _headers;
}
// Part -

// Form +
Form::Form(): _parts() {
  // Nothing to do
}

Form&
Form::add(std::string_view name, std::string value) {
  _parts.emplace_back(name, std::move(value));
  return *this;
}

Form&
Form::addFile(std::string_view name, std::string_view filename, MediaType mediaType, std::string content) {
  _parts.emplace_back(name, filename, std::move(mediaType), std::move(content));
  return *this;
}

Form&
Form::add(Part part) {
  _parts.push_back(std::move(part));
  return *this;
}

bool
Form::has(std::string_view name) const noexcept {
  return part(name) != nullptr;
}

std::optional<std::string_view>
Form::field(std::string_view name) const {
  const Part* found = part(name);
  if(found == nullptr) {
    return std::nullopt;
  }
  return found->content();
}

std::vector<std::string_view>
Form::fields(std::string_view name) const {
  std::vector<std::string_view> res;
  for(const Part& current : _parts) {
    if(current.getName() == name) {
      res.push_back(current.content());
    }
  }
  return res;
}

const Part*
Form::part(std::string_view name) const noexcept {
  for(const Part& current : _parts) {
    if(current.getName() == name) {
      return &current;
    }
  }
  return nullptr;
}

std::vector<const Part*>
Form::parts(std::string_view name) const {
  std::vector<const Part*> res;
  for(const Part& current : _parts) {
    if(current.getName() == name) {
      res.push_back(&current);
    }
  }
  return res;
}

std::size_t
Form::size() const noexcept {
  return _parts.size();
}

bool
Form::empty() const noexcept {
  return _parts.empty();
}

std::vector<Part>::const_iterator
Form::begin() const noexcept {
  return _parts.begin();
}

std::vector<Part>::const_iterator
Form::end() const noexcept {
  return _parts.end();
}

const std::vector<Part>&
Form::getParts() const noexcept {
  return _parts;
}
// Form -
