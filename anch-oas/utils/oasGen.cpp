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
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>

#include "cli/args.hpp"
#include "cli/utils.hpp"
#include "json/mappingError.hpp"
#include "oas/modelGenerator.hpp"
#include "oas/spec.hpp"

#ifndef ANCH_OAS_VERSION
#define ANCH_OAS_VERSION "0.2"
#endif


/*!
 * \c anch-oas-gen options
 *
 * \author Vincent Lachenal
 */
struct GeneratorOptions {
  /*! OpenAPI document */
  std::shared_ptr<std::istream> input;

  /*! Output directory */
  std::filesystem::path output = ".";

  /*! Models options */
  anch::oas::ModelOptions models;
};

/*!
 * Parse command line arguments
 *
 * \param argc the number of arguments
 * \param argv the arguments
 * \param opts the options to fill
 */
void
parseArgs(int argc, char** argv, GeneratorOptions& opts) {
  anch::cli::App app = {
    .name = "anch-oas-gen",
    .description = "Generate C++ models and their anch-json deserializers from OpenAPI 3.0 document schemas (JSON format), "
                   "and anch-rest endpoints (server and client) from its paths",
    .version = ANCH_OAS_VERSION,
    .author = "Vincent Lachenal",
    .licence = "LGPLv3"
  };
  std::vector<anch::cli::Arg> args = {
    {.handler = anch::cli::bindIFS(opts.input), .sopt = 'i', .lopt = "input", .value = true, .name = "FILE",
     .pipe = anch::cli::bindPipe(opts.input), .description = "OpenAPI 3.0 document in JSON format (default to stdin)"},
    {.handler = anch::cli::bindPath(opts.output), .sopt = 'o', .lopt = "output", .value = true, .name = "DIR",
     .description = "Output directory, created when needed (default to current directory)"},
    {.handler = anch::cli::bindStr(opts.models.ns), .sopt = 'n', .lopt = "namespace", .value = true, .name = "NS",
     .description = "Models namespace (default to model)", .example = "petstore::model"},
    {.handler = anch::cli::bindStr(opts.models.name), .sopt = 'f', .lopt = "file", .value = true, .name = "NAME",
     .description = "Generated files base name: NAME.hpp and NAME.cpp (default to models)"},
    {.handler = anch::cli::bindTrue(opts.models.server), .lopt = "server",
     .description = "Generate server endpoints: service interface and its registration in anch-rest API (NAME-server.hpp and NAME-server.cpp)"},
    {.handler = anch::cli::bindTrue(opts.models.client), .lopt = "client",
     .description = "Generate typed anch-rest client (NAME-client.hpp and NAME-client.cpp)"},
    {.handler = anch::cli::bindStr(opts.models.api), .lopt = "api-name", .value = true, .name = "API",
     .description = "Service and client names prefix: APIService and APIClient (default to document title in PascalCase)", .example = "PetStore"},
    {.handler = anch::cli::bindFalse(opts.models.uuid), .lopt = "no-uuid",
     .description = "Map uuid format to std::string instead of anch::UUID"},
    {.handler = anch::cli::bindFalse(opts.models.date), .lopt = "no-date",
     .description = "Map date-time and date formats to std::string instead of anch::date::Date and std::chrono::year_month_day"},
    {.handler = anch::cli::bindFalse(opts.models.constraints), .lopt = "no-constraints",
     .description = "Do not generate value constraints (minLength, minimum, minItems, ...): required properties are still checked"},
    {.handler = anch::cli::bindTrue(opts.models.strict), .lopt = "strict",
     .description = "Fail on unsupported schema instead of skipping it with a warning"}
  };
  anch::cli::ArgHandler handler(app, args);
  handler.handle(argc, argv);
}

/*!
 * Write file when its content has changed (generated sources are not rebuilt when nothing has changed)
 *
 * \param path the file path
 * \param content the file content
 *
 * \throw std::runtime_error write error
 */
void
writeFile(const std::filesystem::path& path, const std::string& content) {
  {
    std::ifstream current(path, std::ios::binary);
    if(current && std::string(std::istreambuf_iterator<char>(current), std::istreambuf_iterator<char>()) == content) {
      return;
    }
  }
  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  out << content;
  out.close();
  if(!out) {
    throw std::runtime_error("Unable to write " + path.string());
  }
}

/*!
 * Generate models from OpenAPI document
 *
 * \param argc the number of arguments
 * \param argv the arguments
 *
 * \return \c 0 on success, \c 1 on error
 */
int
main(int argc, char** argv) {
  try {
    GeneratorOptions opts;
    parseArgs(argc, argv, opts);
    if(!opts.input) {
      std::cerr << "No OpenAPI document: use -i or pipe it on standard input" << std::endl;
      return 1;
    }
    anch::oas::OpenAPI api = anch::oas::parse(*opts.input);
    anch::oas::GeneratedModels models = anch::oas::generateModels(api, opts.models);
    for(const std::string& warning : models.warnings) {
      std::cerr << "warning: " << warning << std::endl;
    }
    std::filesystem::create_directories(opts.output);
    writeFile(opts.output / (opts.models.name + ".hpp"), models.header);
    writeFile(opts.output / (opts.models.name + ".cpp"), models.source);
    if(opts.models.server) {
      writeFile(opts.output / (opts.models.name + "-server.hpp"), models.serverHeader);
      writeFile(opts.output / (opts.models.name + "-server.cpp"), models.serverSource);
    }
    if(opts.models.client) {
      writeFile(opts.output / (opts.models.name + "-client.hpp"), models.clientHeader);
      writeFile(opts.output / (opts.models.name + "-client.cpp"), models.clientSource);
    }
  } catch(const anch::json::MappingError& error) {
    std::cerr << "Invalid OpenAPI document: " << error.what() << std::endl;
    return 1;
  } catch(const std::exception& error) {
    std::cerr << "Error: " << error.what() << std::endl;
    return 1;
  }
  return 0;
}
