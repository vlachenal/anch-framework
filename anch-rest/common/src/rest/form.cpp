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

#include <sstream>

using anch::rest::Form;
using anch::rest::Headers;
using anch::rest::MediaType;
using anch::rest::Part;

// Part +
Part::Part(): Part(std::string_view(), std::string()) {
  // Nothing to do
}

Part::Part(std::string_view name, std::string content):
  _name(name),
  _filename(),
  _mediaType("text", "plain"),
  _headers(),
  _content(std::move(content)) {
  // Nothing to do
}

Part::Part(std::string_view name, std::string_view filename, MediaType mediaType, std::string content):
  _name(name),
  _filename(std::string(filename)),
  _mediaType(std::move(mediaType)),
  _headers(),
  _content(std::move(content)) {
  // Nothing to do
}

std::string_view
Part::content() const noexcept {
  return _content;
}

std::unique_ptr<std::istream>
Part::open() const {
  return std::make_unique<std::istringstream>(_content);
}

std::size_t
Part::size() const noexcept {
  return _content.size();
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
Form::field(std::string_view name) const noexcept {
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
