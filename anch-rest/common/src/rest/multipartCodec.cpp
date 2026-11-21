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

#include <array>
#include <cstdlib>
#include <fstream>
#include <map>
#include <optional>
#include <random>
#include <stdexcept>
#include <system_error>
#include <variant>

#include <unistd.h>

#include "rest/headers.hpp"
#include "rest/request.hpp"

using anch::rest::Form;
using anch::rest::FormLimits;
using anch::codec::MediaType;
using anch::rest::Multipart;
using anch::rest::Part;
using anch::codec::Side;

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
	// Quoted string
	++pos;
	bool closed = false;
	while(pos < value.size()) {
	  char c = value[pos++];
	  if(c == '"') {
	    closed = true;
	    break;
	  }
	  // Only \" and \\ are escapes: browsers do not escape backslashes (C:\dir\file.txt is sent as is)
	  if(c == '\\' && pos < value.size() && (value[pos] == '"' || value[pos] == '\\')) {
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

  /*!
   * Decode name or file name as written by browsers and by \ref escape : %22, %0D and %0A are '"', CR and LF (HTML
   * multipart/form-data encoding algorithm), other characters are kept as is
   */
  std::string
  unescape(std::string_view value) {
    std::string res;
    res.reserve(value.size());
    for(std::size_t i = 0 ; i < value.size() ; ++i) {
      if(value[i] == '%' && i + 2 < value.size()) {
	const std::string code = toLower(value.substr(i + 1, 2));
	if(code == "22" || code == "0d" || code == "0a") {
	  res += (code == "22") ? '"' : (code == "0d" ? '\r' : '\n');
	  i += 2;
	  continue;
	}
      }
      res += value[i];
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

  /*! Part headers: name, file name, media type and other headers */
  struct PartHeaders {
    std::string name;
    std::optional<std::string> filename;
    std::optional<MediaType> mediaType;
    anch::rest::Headers others;
  };

  /*! Parse part headers block (header lines ended by CRLF) */
  PartHeaders
  parsePartHeaders(std::string_view headers) {
    std::optional<Disposition> disposition;
    PartHeaders res;
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
	if(res.mediaType) {
	  throw std::invalid_argument("Duplicate multipart part Content-Type header");
	}
	res.mediaType = MediaType::parse(value);
	if(!res.mediaType) {
	  throw std::invalid_argument("Invalid multipart part Content-Type '" + std::string(value) + "'");
	}
      } else {
	res.others.add(name, value);
      }
    }
    if(!disposition) {
      throw std::invalid_argument("Missing multipart Content-Disposition header");
    }
    auto name = disposition->parameters.find("name");
    if(name == disposition->parameters.end()) {
      throw std::invalid_argument("Missing multipart part name");
    }
    res.name = unescape(name->second);
    if(auto extended = disposition->parameters.find("filename*"); extended != disposition->parameters.end()) {
      res.filename = decodeExtended(extended->second);
    }
    if(auto plain = disposition->parameters.find("filename"); !res.filename && plain != disposition->parameters.end()) {
      res.filename = unescape(plain->second);
    }
    return res;
  }

  /*! Body reader: sliding buffer over input stream, body size limit */
  class Reader {
  private:
    /*! Read block size */
    static constexpr std::size_t BLOCK = 64 * 1024;

    std::istream& _input;
    std::size_t _maxSize;
    std::string _buffer;
    std::size_t _pos = 0;
    std::size_t _total = 0;
    bool _eof = false;

  public:
    Reader(std::istream& input, std::size_t maxSize): _input(input), _maxSize(maxSize) {
    }

    /*! Available data */
    std::string_view
    view() const noexcept {
      return std::string_view(_buffer).substr(_pos);
    }

    void
    consume(std::size_t count) noexcept {
      _pos += count;
    }

    /*! Read until \c count bytes are available: \c false at end of input with less bytes */
    bool
    fill(std::size_t count) {
      while(_buffer.size() - _pos < count && !_eof) {
	if(_pos > 0) {
	  _buffer.erase(0, _pos);
	  _pos = 0;
	}
	const std::size_t size = _buffer.size();
	_buffer.resize(size + BLOCK);
	_input.read(_buffer.data() + size, static_cast<std::streamsize>(BLOCK));
	const auto read = static_cast<std::size_t>(_input.gcount());
	_buffer.resize(size + read);
	if(read == 0) {
	  _eof = true;
	  break;
	}
	if(read > _maxSize - _total) {
	  throw anch::codec::ContentTooLarge("Content is larger than " + std::to_string(_maxSize) + " bytes");
	}
	_total += read;
      }
      return _buffer.size() - _pos >= count;
    }

    /*! Read more data: \c false at end of input */
    bool
    more() {
      return fill(view().size() + 1);
    }
  };

  /*! Temporary file being written, removed when it is not handed over to a part */
  class TemporaryFile {
  private:
    std::filesystem::path _path;
    std::ofstream _out;
    bool _owned = true;

  public:
    explicit TemporaryFile(const std::filesystem::path& directory) {
      std::string pattern = ((directory.empty() ? std::filesystem::temp_directory_path() : directory) / "anch-part-XXXXXX").string();
      const int fd = ::mkstemp(pattern.data());
      if(fd < 0) {
	throw std::system_error(errno, std::generic_category(), "Unable to create multipart temporary file in '" + pattern + "'");
      }
      ::close(fd);
      _path = pattern;
      _out.open(_path, std::ios::binary | std::ios::trunc);
      if(!_out) {
	std::error_code ignored;
	std::filesystem::remove(_path, ignored);
	throw std::runtime_error("Unable to write multipart temporary file '" + _path.string() + "'");
      }
    }

    TemporaryFile(const TemporaryFile&) = delete;

    TemporaryFile& operator=(const TemporaryFile&) = delete;

    ~TemporaryFile() {
      if(_owned) {
	_out.close();
	std::error_code ignored;
	std::filesystem::remove(_path, ignored);
      }
    }

    void
    write(std::string_view data) {
      _out.write(data.data(), static_cast<std::streamsize>(data.size()));
      if(!_out) {
	throw std::runtime_error("Unable to write multipart temporary file '" + _path.string() + "'");
      }
    }

    /*! Close file and hand it over (to a part) */
    std::filesystem::path
    release() {
      _out.close();
      if(!_out) {
	throw std::runtime_error("Unable to write multipart temporary file '" + _path.string() + "'");
      }
      _owned = false;
      return _path;
    }
  };

  /*! Part content: in memory, then in a temporary file over memory limit */
  class PartContent {
  private:
    const FormLimits& _limits;
    std::string _memory;
    std::unique_ptr<TemporaryFile> _file;
    std::size_t _size = 0;

  public:
    explicit PartContent(const FormLimits& limits): _limits(limits) {
    }

    void
    write(std::string_view data) {
      if(data.size() > _limits.maxPartSize - _size) {
	throw anch::codec::ContentTooLarge("Multipart part is larger than " + std::to_string(_limits.maxPartSize) + " bytes");
      }
      _size += data.size();
      if(!_file && _memory.size() + data.size() > _limits.maxMemoryPartSize) {
	_file = std::make_unique<TemporaryFile>(_limits.temporaryDirectory);
	_file->write(_memory);
	_memory.clear();
	_memory.shrink_to_fit();
      }
      if(_file) {
	_file->write(data);
      } else {
	_memory.append(data);
      }
    }

    Part
    toPart(PartHeaders&& headers) {
      Part part;
      if(_file) {
	MediaType type = headers.mediaType.value_or(headers.filename ? MediaType("application", "octet-stream") : MediaType("text", "plain"));
	part = Part::fromTemporaryFile(headers.name, _file->release(), std::move(type), headers.filename);
      } else if(headers.filename) {
	part = Part(headers.name, *headers.filename, headers.mediaType.value_or(MediaType("application", "octet-stream")), std::move(_memory));
      } else {
	part = Part(headers.name, std::move(_memory));
	if(headers.mediaType) {
	  part.setMediaType(std::move(*headers.mediaType));
	}
      }
      for(const auto& [header, values] : headers.others) {
	for(const std::string& value : values) {
	  part.getHeaders().add(header, value);
	}
      }
      return part;
    }
  };

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

  /*! Check if a part in memory contains value (files are not read) */
  bool
  contains(const Form& form, std::string_view value) {
    for(const Part& part : form) {
      if(part.isInMemory() && part.content().find(value) != std::string_view::npos) {
	return true;
      }
    }
    return false;
  }

  /*! Part headers block written before its content */
  std::string
  partHeaders(const Part& part, std::string_view delimiter) {
    static const MediaType TEXT("text", "plain");
    std::string res(delimiter);
    res += CRLF;
    res += "Content-Disposition: form-data; name=\"" + escape(part.getName()) + '"';
    if(part.isFile()) {
      res += "; filename=\"" + escape(*part.getFilename()) + '"';
    }
    res += CRLF;
    if(part.isFile() || !(part.getMediaType() == TEXT)) {
      res += "Content-Type: " + part.getMediaType().toString();
      res += CRLF;
    }
    for(const auto& [name, values] : part.getHeaders()) {
      checkHeader(name);
      for(const std::string& value : values) {
	checkHeader(value);
	res += name + ": " + value;
	res += CRLF;
      }
    }
    res += CRLF;
    return res;
  }

  /*! Check that boundary does not appear in parts in memory */
  void
  checkParts(const Form& form, std::string_view inner) {
    for(const Part& part : form) {
      if(part.isInMemory() && part.content().find(inner) != std::string_view::npos) {
	throw std::invalid_argument("Multipart boundary appears in part '" + part.getName() + "'");
      }
    }
  }

  /*! Multipart body stream: generated headers and parts contents, read on demand */
  class BodyBuffer: public std::streambuf {
  private:
    /*! Body segments: text or part index */
    std::vector<std::variant<std::string, std::size_t>> _segments;
    /*! Form copy (parts files are shared) */
    Form _form;
    /*! Current segment */
    std::size_t _index = 0;
    /*! Current segment is started */
    bool _started = false;
    /*! Current part stream */
    std::unique_ptr<std::istream> _part;
    /*! Part read buffer */
    std::array<char, 64 * 1024> _buffer;

  public:
    BodyBuffer(Form form, std::string_view delimiter): _form(std::move(form)) {
      std::size_t index = 0;
      for(const Part& part : _form) {
	_segments.emplace_back(partHeaders(part, delimiter));
	_segments.emplace_back(index++);
	_segments.emplace_back(std::string(CRLF));
      }
      _segments.emplace_back(std::string(delimiter) + "--" + std::string(CRLF));
    }

    /*! Body size */
    std::uint64_t
    size() const {
      std::uint64_t res = 0;
      for(const auto& segment : _segments) {
	res += std::holds_alternative<std::string>(segment) ? std::get<std::string>(segment).size() : _form.getParts()[std::get<std::size_t>(segment)].size();
      }
      return res;
    }

  protected:
    virtual int_type
    underflow() override {
      while(_index < _segments.size()) {
	auto& segment = _segments[_index];
	if(std::holds_alternative<std::string>(segment)) {
	  std::string& text = std::get<std::string>(segment);
	  if(!_started && !text.empty()) {
	    _started = true;
	    setg(text.data(), text.data(), text.data() + text.size());
	    return traits_type::to_int_type(*gptr());
	  }
	} else {
	  const Part& part = _form.getParts()[std::get<std::size_t>(segment)];
	  if(part.isInMemory()) {
	    if(!_started && part.size() != 0) {
	      _started = true;
	      std::string_view content = part.content();
	      char* data = const_cast<char*>(content.data()); // read only get area
	      setg(data, data, data + content.size());
	      return traits_type::to_int_type(*gptr());
	    }
	  } else {
	    if(!_part) {
	      _part = part.open();
	    }
	    _part->read(_buffer.data(), static_cast<std::streamsize>(_buffer.size()));
	    const std::streamsize read = _part->gcount();
	    if(read > 0) {
	      setg(_buffer.data(), _buffer.data(), _buffer.data() + read);
	      return traits_type::to_int_type(*gptr());
	    }
	    if(_part->bad()) {
	      throw std::runtime_error("Unable to read multipart part '" + part.getName() + "'");
	    }
	  }
	}
	++_index;
	_started = false;
	_part.reset();
      }
      return traits_type::eof();
    }
  };

  /*! Input stream which owns its buffer */
  class BodyInput: public std::istream {
  private:
    std::unique_ptr<BodyBuffer> _body;

  public:
    explicit BodyInput(std::unique_ptr<BodyBuffer> body): std::istream(body.get()), _body(std::move(body)) {
    }
  };

}

std::string
Multipart::randomBoundary() {
  static constexpr std::string_view CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  thread_local std::mt19937_64 generator(std::random_device{}());
  std::uniform_int_distribution<std::size_t> distribution(0, CHARS.size() - 1);
  std::string res = "anch-";
  for(int i = 0 ; i < 32 ; ++i) {
    res += CHARS[distribution(generator)];
  }
  return res;
}

std::string_view
Multipart::mediaType() noexcept {
  return anch::codec::media::MULTIPART_FORM_DATA;
}

FormLimits&
Multipart::limits(Side side) {
  static FormLimits server{16 * 1024 * 1024, 1000, 16 * 1024 * 1024, 16 * 1024, 1024 * 1024, {}};
  static FormLimits client{16 * 1024 * 1024, 1000, 16 * 1024 * 1024, 16 * 1024, 1024 * 1024, {}};
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
  checkParts(form, std::string(CRLF) + delimiter);
  for(const Part& part : form) {
    out << partHeaders(part, delimiter);
    if(part.isInMemory()) {
      out << part.content();
    } else if(part.size() != 0) {
      std::unique_ptr<std::istream> input = part.open();
      out << input->rdbuf();
    }
    out << CRLF;
  }
  out << delimiter << "--" << CRLF;
}

anch::rest::BodyStream
Multipart::stream(const Form& form, const MediaType& contentType, Side) {
  const std::string delimiter = "--" + std::string(checkBoundary(contentType));
  checkParts(form, std::string(CRLF) + delimiter);
  auto body = std::make_unique<BodyBuffer>(form, delimiter);
  const std::uint64_t size = body->size();
  return anch::rest::BodyStream{std::make_unique<BodyInput>(std::move(body)), size};
}

void
Multipart::deserialize(Form& form, std::istream& input, const MediaType& contentType, Side side) {
  const FormLimits& limits = Multipart::limits(side);
  const std::string delimiter = "--" + std::string(checkBoundary(contentType));
  const std::string inner = std::string(CRLF) + delimiter;
  Reader reader(input, limits.maxSize);

  // First delimiter: at body start or after preamble (ignored)
  reader.fill(delimiter.size());
  if(!reader.view().starts_with(delimiter)) {
    while(true) {
      const std::size_t found = reader.view().find(inner);
      if(found != std::string_view::npos) {
	reader.consume(found + CRLF.size());
	break;
      }
      if(reader.view().size() >= inner.size()) {
	reader.consume(reader.view().size() - inner.size() + 1);
      }
      if(!reader.more()) {
	throw std::invalid_argument("Missing multipart delimiter");
      }
    }
  }
  while(true) {
    reader.consume(delimiter.size());
    reader.fill(2);
    if(reader.view().starts_with("--")) {
      return; // close delimiter, epilogue is ignored
    }
    while(reader.fill(1) && OWS.find(reader.view().front()) != std::string_view::npos) {
      reader.consume(1); // transport padding
    }
    if(!reader.fill(1)) {
      throw std::invalid_argument("Missing multipart close delimiter");
    }
    reader.fill(CRLF.size());
    if(!reader.view().starts_with(CRLF)) {
      throw std::invalid_argument("Invalid multipart delimiter line");
    }
    reader.consume(CRLF.size());
    if(form.size() == limits.maxParts) {
      throw anch::codec::ContentTooLarge("Multipart body has more than " + std::to_string(limits.maxParts) + " parts");
    }

    // Headers: lines until an empty line
    std::string headers;
    reader.fill(CRLF.size());
    if(reader.view().starts_with(CRLF)) {
      reader.consume(CRLF.size());
    } else {
      while(true) {
	const std::size_t found = reader.view().find("\r\n\r\n");
	const std::size_t size = (found == std::string_view::npos) ? reader.view().size() : found + CRLF.size();
	if(size > limits.maxHeadersSize) {
	  throw anch::codec::ContentTooLarge("Multipart part headers are larger than " + std::to_string(limits.maxHeadersSize) + " bytes");
	}
	if(found != std::string_view::npos) {
	  headers = reader.view().substr(0, size);
	  reader.consume(found + 2 * CRLF.size());
	  break;
	}
	if(!reader.more()) {
	  throw std::invalid_argument("Unterminated multipart part headers");
	}
      }
    }
    PartHeaders parsed = parsePartHeaders(headers);

    // Content: until next delimiter (the end of the available data can be the start of a delimiter)
    PartContent content(limits);
    while(true) {
      const std::size_t found = reader.view().find(inner);
      if(found != std::string_view::npos) {
	content.write(reader.view().substr(0, found));
	reader.consume(found + CRLF.size());
	break;
      }
      if(reader.view().size() >= inner.size()) {
	const std::size_t size = reader.view().size() - inner.size() + 1;
	content.write(reader.view().substr(0, size));
	reader.consume(size);
      }
      if(!reader.more()) {
	content.write(reader.view()); // size limit before missing delimiter error
	throw std::invalid_argument("Missing multipart close delimiter");
      }
    }
    form.add(content.toPart(std::move(parsed)));
  }
}
