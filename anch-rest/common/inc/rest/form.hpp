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

#include "anch-rest-common-export.hpp"

#include <filesystem>
#include <istream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "codec/mediaType.hpp"

#include "rest/headers.hpp"

namespace anch::rest {

  /*!
   * Form codecs limits, by side ( \ref FormUrlEncoded::limits , \ref Multipart::limits ): a body over a
   * limit raises \ref anch::codec::ContentTooLarge ( \c 413 on server side).
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_REST_COMMON_EXPORT FormLimits {
    /*! Maximum body size in bytes */
    std::size_t maxSize;

    /*! Maximum number of parts (fields and files) */
    std::size_t maxParts;

    /*! Maximum size of a part content in bytes */
    std::size_t maxPartSize;

    /*! Maximum size of a part headers in bytes ( \c multipart/form-data ) */
    std::size_t maxHeadersSize;

    /*! Maximum size of a part kept in memory ( \c multipart/form-data ): larger parts are written in temporary
     * files, deleted with their last \ref Part copy */
    std::size_t maxMemoryPartSize;

    /*! Directory of temporary files (default: \c std::filesystem::temp_directory_path() ) */
    std::filesystem::path temporaryDirectory;
  };

  /*!
   * Form part: a field ( \c text/plain by default) or a file (with a file name,
   * \c application/octet-stream by default).\n
   * \n
   * Content is in memory or in a file ( \ref fromFile , \ref fromTemporaryFile , large parts read by
   * \ref Multipart ). It is read with \ref open (stream) or with \ref content , which loads a file
   * content in memory on first call: use \ref open for files. Copies of a part share its file.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class ANCH_REST_COMMON_EXPORT Part {

    /*! File content */
    struct FileContent;

    // Attributes +
  private:
    /*! Part name */
    std::string _name;

    /*! File name */
    std::optional<std::string> _filename;

    /*! Content media type */
    anch::codec::MediaType _mediaType;

    /*! Other part headers */
    anch::rest::Headers _headers;

    /*! Content (in memory, or loaded file content) */
    mutable std::string _content;

    /*! File content ( \c nullptr in memory) */
    std::shared_ptr<const FileContent> _file;

    /*! File content has been loaded in \c _content */
    mutable bool _loaded;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Part default constructor: empty field without name ( \c text/plain ), to be assigned
     * (model members)
     */
    Part();

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
    Part(std::string_view name, std::string_view filename, anch::codec::MediaType mediaType, std::string content);

    /*!
     * Create part from a file: content is read from the file when it is used (the file is not deleted)
     *
     * \param name the part name
     * \param path the file path
     * \param mediaType the content media type
     * \param filename the file name (default: \c path file name)
     *
     * \return the part
     *
     * \throw std::filesystem::filesystem_error when file size can not be read
     */
    static Part fromFile(std::string_view name, const std::filesystem::path& path, anch::codec::MediaType mediaType,
			 std::optional<std::string_view> filename = std::nullopt);

    /*!
     * Create part from a temporary file: the file is deleted with the last part copy
     *
     * \param name the part name
     * \param path the file path
     * \param mediaType the content media type
     * \param filename the file name (empty for a field)
     *
     * \return the part
     *
     * \throw std::filesystem::filesystem_error when file size can not be read
     */
    static Part fromTemporaryFile(std::string_view name, const std::filesystem::path& path, anch::codec::MediaType mediaType,
				  std::optional<std::string_view> filename);
    // Constructors -

    // Methods +
  public:
    /*!
     * Get content: file content is loaded in memory on first call
     *
     * \return the content
     *
     * \throw std::runtime_error when file can not be read
     */
    std::string_view content() const;

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

    /*!
     * Check if content is in memory (not in a file)
     *
     * \return \c true when content is in memory
     */
    bool isInMemory() const noexcept;

    /*!
     * Get content file path
     *
     * \return the path, empty for content in memory
     */
    std::optional<std::filesystem::path> getPath() const;

    /*!
     * Equality operator: name, file name, media type, headers and content are compared (files are read)
     *
     * \param other the part to compare
     *
     * \return \c true when parts are equal
     */
    bool operator==(const Part& other) const;
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
     * Set part name
     *
     * \param name the name
     */
    void setName(std::string_view name);

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
    const anch::codec::MediaType& getMediaType() const noexcept;

    /*!
     * Set content media type
     *
     * \param mediaType the media type
     */
    void setMediaType(anch::codec::MediaType mediaType);

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
   * \snippet{trimleft} anch-rest/examples/server.cpp form
   * Fields are converted on server side with \ref anch::rest::formField "formField<T>()" and
   * \ref anch::rest::requiredFormField "requiredFormField<T>()" .
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class ANCH_REST_COMMON_EXPORT Form {

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
    Form& addFile(std::string_view name, std::string_view filename, anch::codec::MediaType mediaType, std::string content);

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
    std::optional<std::string_view> field(std::string_view name) const;

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
