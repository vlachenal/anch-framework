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
#include "rest/formCodec.hpp"

#include <iterator>
#include <optional>
#include <stdexcept>

#include "rest/headers.hpp"
#include "rest/request.hpp"

using anch::rest::Form;
using anch::rest::FormUrlEncoded;
using anch::rest::MediaType;
using anch::rest::Part;
using anch::rest::Side;

namespace {

  std::string
  decode(std::string_view value) {
    std::optional<std::string> res = anch::rest::percentDecode(value, true);
    if(!res) {
      throw std::invalid_argument("Invalid form encoding '" + std::string(value) + "'");
    }
    return std::move(*res);
  }

}

std::string_view
FormUrlEncoded::mediaType() noexcept {
  return anch::rest::media::APP_FORM_URLENCODED;
}

std::string
FormUrlEncoded::encode(std::string_view value) {
  static constexpr std::string_view HEX = "0123456789ABCDEF";
  std::string res;
  res.reserve(value.size());
  for(char c : value) {
    const bool kept = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')
      || c == '*' || c == '-' || c == '.' || c == '_';
    if(kept) {
      res += c;
    } else if(c == ' ') {
      res += '+';
    } else {
      const auto byte = static_cast<unsigned char>(c);
      res += '%';
      res += HEX[byte >> 4];
      res += HEX[byte & 0x0F];
    }
  }
  return res;
}

anch::rest::FormLimits&
FormUrlEncoded::limits(Side side) {
  static anch::rest::FormLimits server{2 * 1024 * 1024, 1000, 2 * 1024 * 1024, 0, 2 * 1024 * 1024, {}};
  static anch::rest::FormLimits client{2 * 1024 * 1024, 1000, 2 * 1024 * 1024, 0, 2 * 1024 * 1024, {}};
  return (side == Side::SERVER) ? server : client;
}

void
FormUrlEncoded::serialize(const Form& form, std::ostream& out) {
  bool first = true;
  for(const Part& part : form) {
    if(part.isFile()) {
      throw std::invalid_argument("File part '" + part.getName() + "' can not be URL-encoded (use multipart/form-data)");
    }
    if(!first) {
      out << '&';
    }
    first = false;
    out << encode(part.getName()) << '=' << encode(part.content());
  }
}

void
FormUrlEncoded::deserialize(Form& form, std::istream& input, const MediaType& contentType, Side side) {
  const anch::rest::FormLimits& limits = FormUrlEncoded::limits(side);
  std::optional<std::string_view> charset = contentType.getParameter("charset");
  const anch::rest::CaseInsensitiveLess less;
  if(charset && (less(*charset, "utf-8") || less("utf-8", *charset))) {
    throw std::invalid_argument("Unsupported form charset '" + std::string(*charset) + "'");
  }
  const std::string content = anch::rest::readContent(input, limits.maxSize);
  std::string_view rest = content;
  while(!rest.empty()) {
    const std::size_t amp = rest.find('&');
    std::string_view pair = rest.substr(0, amp);
    rest = (amp == std::string_view::npos) ? std::string_view() : rest.substr(amp + 1);
    if(pair.empty()) {
      continue;
    }
    if(form.size() == limits.maxParts) {
      throw anch::rest::ContentTooLarge("Form has more than " + std::to_string(limits.maxParts) + " fields");
    }
    if(pair.size() > limits.maxPartSize) {
      throw anch::rest::ContentTooLarge("Form field is larger than " + std::to_string(limits.maxPartSize) + " bytes");
    }
    const std::size_t equal = pair.find('=');
    std::string name = decode(pair.substr(0, equal));
    std::string value = (equal == std::string_view::npos) ? std::string() : decode(pair.substr(equal + 1));
    form.add(Part(name, std::move(value)));
  }
}
