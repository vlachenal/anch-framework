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
#include "rest/mediaType.hpp"

#include "convert.hpp"

#include <algorithm>

using anch::rest::MediaType;

namespace {

  constexpr char
  lower(char c) noexcept {
    return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
  }

  std::string
  toLower(std::string_view value) {
    std::string res(value);
    std::transform(res.begin(), res.end(), res.begin(), lower);
    return res;
  }

  std::string_view
  trim(std::string_view value) noexcept {
    const std::size_t first = value.find_first_not_of(" \t");
    if(first == std::string_view::npos) {
      return std::string_view();
    }
    const std::size_t last = value.find_last_not_of(" \t");
    return value.substr(first, last - first + 1);
  }

  // RFC 9110 token
  bool
  isToken(std::string_view value) noexcept {
    if(value.empty()) {
      return false;
    }
    for(char c : value) {
      const bool alnum = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
      if(!alnum && std::string_view("!#$%&'*+-.^_`|~").find(c) == std::string_view::npos) {
	return false;
      }
    }
    return true;
  }

  // Split on separator outside quoted strings
  std::vector<std::string_view>
  split(std::string_view value, char separator) {
    std::vector<std::string_view> res;
    bool quoted = false;
    std::size_t start = 0;
    for(std::size_t i = 0 ; i < value.size() ; ++i) {
      const char c = value[i];
      if(quoted && c == '\\') {
	++i;
      } else if(c == '"') {
	quoted = !quoted;
      } else if(!quoted && c == separator) {
	res.push_back(value.substr(start, i - start));
	start = i + 1;
      }
    }
    res.push_back(value.substr(start));
    return res;
  }

  // Parameter value: token or quoted string
  std::optional<std::string>
  parseValue(std::string_view value) {
    if(value.size() >= 2 && value.front() == '"' && value.back() == '"') {
      std::string res;
      for(std::size_t i = 1 ; i + 1 < value.size() ; ++i) {
	if(value[i] == '\\' && i + 2 < value.size()) {
	  ++i;
	}
	res += value[i];
      }
      return res;
    }
    if(!isToken(value)) {
      return std::nullopt;
    }
    return std::string(value);
  }

  // RFC 9110 qvalue: 0[.ddd] or 1[.000]
  std::optional<double>
  parseQuality(std::string_view value) {
    if(value.empty() || value.size() > 5 || (value[0] != '0' && value[0] != '1')
       || (value.size() > 1 && value[1] != '.')) {
      return std::nullopt;
    }
    std::expected<double, std::errc> res = anch::tryConvert<double>(value);
    if(!res || *res > 1.0) {
      return std::nullopt;
    }
    return *res;
  }

}

MediaType::MediaType(std::string_view type, std::string_view subtype):
  _type(toLower(type)),
  _subtype(toLower(subtype)),
  _parameters(),
  _quality(1.0) {
  // Nothing to do
}

std::optional<MediaType>
MediaType::parse(std::string_view value) {
  std::vector<std::string_view> parts = split(value, ';');
  std::string_view essence = trim(parts.front());
  const std::size_t slash = essence.find('/');
  if(slash == std::string_view::npos) {
    return std::nullopt;
  }
  std::string_view type = essence.substr(0, slash);
  std::string_view subtype = essence.substr(slash + 1);
  if(!isToken(type) || !isToken(subtype) || (type == "*" && subtype != "*")) {
    return std::nullopt;
  }
  MediaType res(type, subtype);
  for(std::size_t i = 1 ; i < parts.size() ; ++i) {
    std::string_view param = trim(parts[i]);
    if(param.empty()) {
      continue;
    }
    const std::size_t equal = param.find('=');
    if(equal == std::string_view::npos || !isToken(param.substr(0, equal))) {
      return std::nullopt;
    }
    std::optional<std::string> paramValue = parseValue(param.substr(equal + 1));
    if(!paramValue) {
      return std::nullopt;
    }
    res._parameters[toLower(param.substr(0, equal))] = std::move(*paramValue);
  }
  return res;
}

std::vector<MediaType>
MediaType::parseAccept(std::string_view value) {
  std::vector<MediaType> res;
  for(std::string_view range : split(value, ',')) {
    if(trim(range).empty()) {
      continue;
    }
    std::optional<MediaType> mediaType = parse(range);
    if(!mediaType) {
      continue;
    }
    auto quality = mediaType->_parameters.find("q");
    if(quality != mediaType->_parameters.end()) {
      std::optional<double> q = parseQuality(quality->second);
      if(!q) {
	continue;
      }
      mediaType->_quality = *q;
      mediaType->_parameters.erase(quality);
    }
    res.push_back(std::move(*mediaType));
  }
  std::stable_sort(res.begin(), res.end(), [](const MediaType& left, const MediaType& right) {
    return left._quality > right._quality;
  });
  return res;
}

bool
MediaType::includes(const MediaType& other) const noexcept {
  if(_type == "*") {
    return true;
  }
  if(_type != other._type) {
    return false;
  }
  return _subtype == "*" || _subtype == other._subtype;
}

int
MediaType::specificity() const noexcept {
  if(_type == "*") {
    return 0;
  }
  return _subtype == "*" ? 1 : 2;
}

std::string
MediaType::essence() const {
  return _type + '/' + _subtype;
}

std::string
MediaType::toString() const {
  std::string res = essence();
  for(const auto& [name, value] : _parameters) {
    res += ';';
    res += name;
    res += '=';
    if(isToken(value)) {
      res += value;
    } else {
      res += '"';
      for(char c : value) {
	if(c == '"' || c == '\\') {
	  res += '\\';
	}
	res += c;
      }
      res += '"';
    }
  }
  return res;
}

bool
MediaType::operator==(const MediaType& other) const noexcept {
  return _type == other._type && _subtype == other._subtype && _parameters == other._parameters;
}

const std::string&
MediaType::getType() const noexcept {
  return _type;
}

const std::string&
MediaType::getSubtype() const noexcept {
  return _subtype;
}

const std::map<std::string, std::string>&
MediaType::getParameters() const noexcept {
  return _parameters;
}

std::optional<std::string_view>
MediaType::getParameter(std::string_view name) const {
  auto iter = _parameters.find(std::string(name));
  if(iter == _parameters.end()) {
    return std::nullopt;
  }
  return std::string_view(iter->second);
}

void
MediaType::setParameter(std::string_view name, std::string_view value) {
  _parameters[toLower(name)] = std::string(value);
}

double
MediaType::getQuality() const noexcept {
  return _quality;
}

std::optional<std::size_t>
anch::rest::negotiate(const std::vector<MediaType>& accept, const std::vector<MediaType>& available) {
  if(available.empty()) {
    return std::nullopt;
  }
  if(accept.empty()) {
    return 0;
  }
  std::optional<std::size_t> selected;
  double selectedQuality = 0.0;
  for(std::size_t i = 0 ; i < available.size() ; ++i) {
    // Quality of the most specific media range which includes this media type
    int specificity = -1;
    double quality = 0.0;
    for(const MediaType& range : accept) {
      if(range.includes(available[i]) && range.specificity() > specificity) {
	specificity = range.specificity();
	quality = range.getQuality();
      }
    }
    if(quality > selectedQuality) {
      selected = i;
      selectedQuality = quality;
    }
  }
  return selected;
}
