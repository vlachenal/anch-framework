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
#include "rest/multipartCodec.hpp"

#include <map>
#include <optional>
#include <random>
#include <stdexcept>

#include "rest/headers.hpp"
#include "rest/request.hpp"

using anch::rest::Form;
using anch::rest::FormLimits;
using anch::rest::MediaType;
using anch::rest::Multipart;
using anch::rest::Part;
using anch::rest::Side;

namespace {

  constexpr std::string_view CRLF = "\r\n";

  constexpr std::string_view OWS = " \t";

  std::string_view
  trim(std::string_view value) noexcept {
    const std::size_t first = value.find_first_not_of(OWS);
    if(first == std::string_view::npos) {
      return std::string_view();
    }
    return value.substr(first, value.find_last_not_of(OWS) - first + 1);
  }

  bool
  equalsIgnoreCase(std::string_view left, std::string_view right) noexcept {
    const anch::rest::CaseInsensitiveLess less;
    return !less(left, right) && !less(right, left);
  }

  std::string
  toLower(std::string_view value) {
    std::string res(value);
    for(char& c : res) {
      if(c >= 'A' && c <= 'Z') {
	c = static_cast<char>(c - 'A' + 'a');
      }
    }
    return res;
  }

  /*! Check boundary (RFC 2046: 1 to 70 characters, not ending with space) */
  std::string_view
  checkBoundary(const MediaType& contentType) {
    std::optional<std::string_view> boundary = contentType.getParameter("boundary");
    if(!boundary || boundary->empty()) {
      throw std::invalid_argument("Missing multipart boundary");
    }
    if(boundary->size() > 70 || boundary->back() == ' ' || boundary->find_first_of("\r\n") != std::string_view::npos) {
      throw std::invalid_argument("Invalid multipart boundary");
    }
    return *boundary;
  }

  /*! Content-Disposition header: type and parameters (lower case names) */
  struct Disposition {
    std::string type;
    std::map<std::string, std::string, std::less<>> parameters;
  };

  /*! Parse Content-Disposition header value: type *( ";" name "=" ( token / quoted-string ) ) */
  std::optional<Disposition>
  parseDisposition(std::string_view value) {
    Disposition res;
    std::size_t pos = value.find(';');
    res.type = trim(value.substr(0, pos));
    if(res.type.empty()) {
      return std::nullopt;
    }
    while(pos != std::string_view::npos) {
      ++pos; // ';'
      while(pos < value.size() && OWS.find(value[pos]) != std::string_view::npos) {
	++pos;
      }
      if(pos == value.size()) {
	break;
      }
      const std::size_t equal = value.find('=', pos);
      if(equal == std::string_view::npos) {
	return std::nullopt;
      }
      std::string name = toLower(trim(value.substr(pos, equal - pos)));
      if(name.empty() || name.find_first_of(" \t\";") != std::string::npos) {
	return std::nullopt;
      }
      pos = equal + 1;
      while(pos < value.size() && OWS.find(value[pos]) != std::string_view::npos) {
	++pos;
      }
      std::string parameter;
      if(pos < value.size() && value[pos] == '"') {
	// Quoted string with backslash escapes
	++pos;
	bool closed = false;
	while(pos < value.size()) {
	  char c = value[pos++];
	  if(c == '"') {
	    closed = true;
	    break;
	  }
	  if(c == '\\' && pos < value.size()) {
	    c = value[pos++];
	  }
	  parameter += c;
	}
	if(!closed) {
	  return std::nullopt;
	}
	while(pos < value.size() && OWS.find(value[pos]) != std::string_view::npos) {
	  ++pos;
	}
	if(pos < value.size() && value[pos] != ';') {
	  return std::nullopt;
	}
	if(pos == value.size()) {
	  pos = std::string_view::npos;
	}
      } else {
	const std::size_t semicolon = value.find(';', pos);
	parameter = trim(value.substr(pos, semicolon == std::string_view::npos ? std::string_view::npos : semicolon - pos));
	pos = semicolon;
      }
      res.parameters.insert_or_assign(std::move(name), std::move(parameter));
    }
    return res;
  }

  /*! Decode RFC 5987 extended value (UTF-8 only) */
  std::optional<std::string>
  decodeExtended(std::string_view value) {
    const std::size_t quote = value.find('\'');
    if(quote == std::string_view::npos || !equalsIgnoreCase(value.substr(0, quote), "utf-8")) {
      return std::nullopt;
    }
    const std::size_t language = value.find('\'', quote + 1);
    if(language == std::string_view::npos) {
      return std::nullopt;
    }
    return anch::rest::percentDecode(value.substr(language + 1));
  }

  /*! Build part from its headers block (header lines ended by CRLF) and its content */
  Part
  readPart(std::string_view headers, std::string_view content) {
    std::optional<Disposition> disposition;
    std::optional<MediaType> mediaType;
    anch::rest::Headers others;
    while(!headers.empty()) {
      const std::size_t eol = headers.find(CRLF);
      std::string_view line = headers.substr(0, eol);
      headers = (eol == std::string_view::npos) ? std::string_view() : headers.substr(eol + CRLF.size());
      const std::size_t colon = line.find(':');
      if(colon == std::string_view::npos || colon == 0 || line.substr(0, colon).find_first_of(" \t") != std::string_view::npos) {
	throw std::invalid_argument("Invalid multipart part header '" + std::string(line) + "'");
      }
      const std::string_view name = line.substr(0, colon);
      const std::string_view value = trim(line.substr(colon + 1));
      if(equalsIgnoreCase(name, anch::rest::header::CONTENT_DISPOSITION)) {
	if(disposition) {
	  throw std::invalid_argument("Duplicate multipart Content-Disposition header");
	}
	disposition = parseDisposition(value);
	if(!disposition || !equalsIgnoreCase(disposition->type, "form-data")) {
	  throw std::invalid_argument("Invalid multipart Content-Disposition '" + std::string(value) + "'");
	}
      } else if(equalsIgnoreCase(name, anch::rest::header::CONTENT_TYPE)) {
	if(mediaType) {
	  throw std::invalid_argument("Duplicate multipart part Content-Type header");
	}
	mediaType = MediaType::parse(value);
	if(!mediaType) {
	  throw std::invalid_argument("Invalid multipart part Content-Type '" + std::string(value) + "'");
	}
      } else {
	others.add(name, value);
      }
    }
    if(!disposition) {
      throw std::invalid_argument("Missing multipart Content-Disposition header");
    }
    auto name = disposition->parameters.find("name");
    if(name == disposition->parameters.end()) {
      throw std::invalid_argument("Missing multipart part name");
    }
    std::optional<std::string> filename;
    if(auto extended = disposition->parameters.find("filename*"); extended != disposition->parameters.end()) {
      filename = decodeExtended(extended->second);
    }
    if(auto plain = disposition->parameters.find("filename"); !filename && plain != disposition->parameters.end()) {
      filename = plain->second;
    }
    Part part = filename
      ? Part(name->second, *filename, mediaType.value_or(MediaType("application", "octet-stream")), std::string(content))
      : Part(name->second, std::string(content));
    if(mediaType && !filename) {
      part.setMediaType(std::move(*mediaType));
    }
    for(const auto& [header, values] : others) {
      for(const std::string& value : values) {
	part.getHeaders().add(header, value);
      }
    }
    return part;
  }

  /*! Escape name or file name as browsers do (HTML multipart/form-data encoding algorithm) */
  std::string
  escape(std::string_view value) {
    std::string res;
    res.reserve(value.size());
    for(char c : value) {
      switch(c) {
      case '"': res += "%22"; break;
      case '\r': res += "%0D"; break;
      case '\n': res += "%0A"; break;
      default: res += c;
      }
    }
    return res;
  }

  void
  checkHeader(std::string_view value) {
    if(value.find_first_of("\r\n") != std::string_view::npos) {
      throw std::invalid_argument("Multipart part header can not contain CR or LF");
    }
  }

  /*! Random boundary: "anch-" and 32 alphanumeric characters */
  std::string
  randomBoundary() {
    static constexpr std::string_view CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    thread_local std::mt19937_64 generator(std::random_device{}());
    std::uniform_int_distribution<std::size_t> distribution(0, CHARS.size() - 1);
    std::string res = "anch-";
    for(int i = 0 ; i < 32 ; ++i) {
      res += CHARS[distribution(generator)];
    }
    return res;
  }

  bool
  contains(const Form& form, std::string_view value) {
    for(const Part& part : form) {
      if(part.content().find(value) != std::string_view::npos) {
	return true;
      }
    }
    return false;
  }

}

std::string_view
Multipart::mediaType() noexcept {
  return anch::rest::media::MULTIPART_FORM_DATA;
}

FormLimits&
Multipart::limits(Side side) {
  static FormLimits server{16 * 1024 * 1024, 1000, 16 * 1024 * 1024, 16 * 1024};
  static FormLimits client{16 * 1024 * 1024, 1000, 16 * 1024 * 1024, 16 * 1024};
  return (side == Side::SERVER) ? server : client;
}

MediaType
Multipart::contentType(const Form& form) {
  std::string boundary = randomBoundary();
  while(contains(form, boundary)) {
    boundary = randomBoundary();
  }
  MediaType res("multipart", "form-data");
  res.setParameter("boundary", boundary);
  return res;
}

void
Multipart::serialize(const Form& form, std::ostream& out, const MediaType& contentType, Side) {
  const std::string delimiter = "--" + std::string(checkBoundary(contentType));
  const std::string inner = std::string(CRLF) + delimiter;
  const MediaType text("text", "plain");
  for(const Part& part : form) {
    if(part.content().find(inner) != std::string_view::npos) {
      throw std::invalid_argument("Multipart boundary appears in part '" + part.getName() + "'");
    }
    out << delimiter << CRLF << "Content-Disposition: form-data; name=\"" << escape(part.getName()) << '"';
    if(part.isFile()) {
      out << "; filename=\"" << escape(*part.getFilename()) << '"';
    }
    out << CRLF;
    if(part.isFile() || !(part.getMediaType() == text)) {
      out << "Content-Type: " << part.getMediaType().toString() << CRLF;
    }
    for(const auto& [name, values] : part.getHeaders()) {
      checkHeader(name);
      for(const std::string& value : values) {
	checkHeader(value);
	out << name << ": " << value << CRLF;
      }
    }
    out << CRLF << part.content() << CRLF;
  }
  out << delimiter << "--" << CRLF;
}

void
Multipart::deserialize(Form& form, std::istream& input, const MediaType& contentType, Side side) {
  const FormLimits& limits = Multipart::limits(side);
  const std::string delimiter = "--" + std::string(checkBoundary(contentType));
  const std::string inner = std::string(CRLF) + delimiter;
  const std::string body = anch::rest::readContent(input, limits.maxSize);
  const std::string_view view = body;

  // First delimiter: at body start or after preamble
  std::size_t pos = 0;
  if(!view.starts_with(delimiter)) {
    pos = view.find(inner);
    if(pos == std::string_view::npos) {
      throw std::invalid_argument("Missing multipart delimiter");
    }
    pos += CRLF.size();
  }
  while(true) {
    pos += delimiter.size();
    if(view.substr(pos).starts_with("--")) {
      return; // close delimiter, epilogue is ignored
    }
    while(pos < view.size() && OWS.find(view[pos]) != std::string_view::npos) {
      ++pos; // transport padding
    }
    if(pos >= view.size()) {
      throw std::invalid_argument("Missing multipart close delimiter");
    }
    if(!view.substr(pos).starts_with(CRLF)) {
      throw std::invalid_argument("Invalid multipart delimiter line");
    }
    pos += CRLF.size();
    if(form.size() == limits.maxParts) {
      throw anch::rest::ContentTooLarge("Multipart body has more than " + std::to_string(limits.maxParts) + " parts");
    }

    // Headers: lines until an empty line (CRLF of delimiter line starts the search)
    const std::size_t headersEnd = view.find("\r\n\r\n", pos - CRLF.size());
    const std::size_t headersSize = (headersEnd == std::string_view::npos ? view.size() : headersEnd + CRLF.size()) - pos;
    if(headersSize > limits.maxHeadersSize) {
      throw anch::rest::ContentTooLarge("Multipart part headers are larger than " + std::to_string(limits.maxHeadersSize) + " bytes");
    }
    if(headersEnd == std::string_view::npos) {
      throw std::invalid_argument("Unterminated multipart part headers");
    }
    const std::string_view headers = view.substr(pos, headersSize);
    const std::size_t contentStart = headersEnd + 2 * CRLF.size();

    // Content: until next delimiter
    const std::size_t contentEnd = view.find(inner, contentStart);
    if(contentEnd == std::string_view::npos) {
      if(view.size() - contentStart > limits.maxPartSize) {
	throw anch::rest::ContentTooLarge("Multipart part is larger than " + std::to_string(limits.maxPartSize) + " bytes");
      }
      throw std::invalid_argument("Missing multipart close delimiter");
    }
    if(contentEnd - contentStart > limits.maxPartSize) {
      throw anch::rest::ContentTooLarge("Multipart part is larger than " + std::to_string(limits.maxPartSize) + " bytes");
    }
    form.add(readPart(headers, view.substr(contentStart, contentEnd - contentStart)));
    pos = contentEnd + CRLF.size();
  }
}
