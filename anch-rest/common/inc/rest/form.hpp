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
#pragma once

#include <istream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "rest/headers.hpp"
#include "rest/mediaType.hpp"

namespace anch::rest {

  /*!
   * Form part: a field ( \c text/plain by default) or a file (with a file name,
   * \c application/octet-stream by default).\n
   * \n
   * Content is kept in memory. It is read with \ref content or \ref open : use \ref open to read
   * file contents, it will not need the whole content in memory in later versions.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class Part {

    // Attributes +
  private:
    /*! Part name */
    std::string _name;

    /*! File name */
    std::optional<std::string> _filename;

    /*! Content media type */
    anch::rest::MediaType _mediaType;

    /*! Other part headers */
    anch::rest::Headers _headers;

    /*! Content */
    std::string _content;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Field \ref Part constructor ( \c text/plain )
     *
     * \param name the part name
     * \param content the content
     */
    Part(std::string_view name, std::string content);

    /*!
     * File \ref Part constructor
     *
     * \param name the part name
     * \param filename the file name
     * \param mediaType the content media type
     * \param content the content
     */
    Part(std::string_view name, std::string_view filename, anch::rest::MediaType mediaType, std::string content);
    // Constructors -

    // Methods +
  public:
    /*!
     * Get content
     *
     * \return the content
     */
    std::string_view content() const noexcept;

    /*!
     * Open content stream
     *
     * \return the content stream
     */
    std::unique_ptr<std::istream> open() const;

    /*!
     * Get content size
     *
     * \return the content size in bytes
     */
    std::size_t size() const noexcept;

    /*!
     * Check if part is a file (it has a file name)
     *
     * \return \c true when part is a file
     */
    bool isFile() const noexcept;
    // Methods -

    // Accessors +
  public:
    /*!
     * Get part name
     *
     * \return the name
     */
    const std::string& getName() const noexcept;

    /*!
     * Get file name
     *
     * \return the file name, empty for fields
     */
    std::optional<std::string_view> getFilename() const noexcept;

    /*!
     * Get content media type
     *
     * \return the media type
     */
    const anch::rest::MediaType& getMediaType() const noexcept;

    /*!
     * Set content media type
     *
     * \param mediaType the media type
     */
    void setMediaType(anch::rest::MediaType mediaType);

    /*!
     * Get other part headers ( \c Content-Disposition and \c Content-Type are given by name, file
     * name and media type)
     *
     * \return the headers
     */
    anch::rest::Headers& getHeaders() noexcept;

    /*!
     * Get other part headers
     *
     * \return the headers
     */
    const anch::rest::Headers& getHeaders() const noexcept;
    // Accessors -

  };

  /*!
   * Form: ordered parts ( \c application/x-www-form-urlencoded or \c multipart/form-data bodies).
   * A name can be repeated (arrays).
   * \code{.cpp}
anch::rest::Form form;
form.add("name", "Rex").add("tags", "dog").add("tags", "brown");
std::optional<std::string_view> name = form.field("name");
std::vector<std::string_view> tags = form.fields("tags");
   * \endcode
   * Fields are converted on server side with \ref anch::rest::formField "formField<T>()" and
   * \ref anch::rest::requiredFormField "requiredFormField<T>()" .
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class Form {

    // Attributes +
  private:
    /*! Parts, in order */
    std::vector<anch::rest::Part> _parts;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Form constructor
     */
    Form();
    // Constructors -

    // Methods +
  public:
    /*!
     * Add field
     *
     * \param name the field name
     * \param value the field value
     *
     * \return \c this
     */
    Form& add(std::string_view name, std::string value);

    /*!
     * Add file
     *
     * \param name the part name
     * \param filename the file name
     * \param mediaType the content media type
     * \param content the content
     *
     * \return \c this
     */
    Form& addFile(std::string_view name, std::string_view filename, anch::rest::MediaType mediaType, std::string content);

    /*!
     * Add part
     *
     * \param part the part
     *
     * \return \c this
     */
    Form& add(anch::rest::Part part);

    /*!
     * Check if form has a part
     *
     * \param name the part name
     *
     * \return \c true when form has a part with this name
     */
    bool has(std::string_view name) const noexcept;

    /*!
     * Get first part content
     *
     * \param name the part name
     *
     * \return the content, empty when there is no part with this name
     */
    std::optional<std::string_view> field(std::string_view name) const noexcept;

    /*!
     * Get every part content with name
     *
     * \param name the part name
     *
     * \return the contents, in order
     */
    std::vector<std::string_view> fields(std::string_view name) const;

    /*!
     * Get first part
     *
     * \param name the part name
     *
     * \return the part, \c nullptr when there is no part with this name
     */
    const anch::rest::Part* part(std::string_view name) const noexcept;

    /*!
     * Get every part with name
     *
     * \param name the part name
     *
     * \return the parts, in order
     */
    std::vector<const anch::rest::Part*> parts(std::string_view name) const;

    /*!
     * Get number of parts
     *
     * \return the number of parts
     */
    std::size_t size() const noexcept;

    /*!
     * Check if form is empty
     *
     * \return \c true when form has no part
     */
    bool empty() const noexcept;

    /*!
     * Get parts begin iterator
     *
     * \return the begin iterator
     */
    std::vector<anch::rest::Part>::const_iterator begin() const noexcept;

    /*!
     * Get parts end iterator
     *
     * \return the end iterator
     */
    std::vector<anch::rest::Part>::const_iterator end() const noexcept;
    // Methods -

    // Accessors +
  public:
    /*!
     * Get parts
     *
     * \return the parts, in order
     */
    const std::vector<anch::rest::Part>& getParts() const noexcept;
    // Accessors -

  };

}
