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
#include "cli/args.hpp"

#include <sstream>
#include <stdexcept>
#include <regex>
#include <iostream>
#include <fstream>

using anch::cli::ArgHandler;
using anch::cli::Arg;
using anch::cli::App;

const std::string ANCH_CLI_NOARG;
const std::regex SREGEX("^-([^-]+)$");
const std::regex LREGEX("^--(.+)$");

namespace anch::cli {

  /*!
   * Argument state (outside user declaration)
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class ArgState {
    friend anch::cli::ArgHandler;

  private:
    // Attribute +
    /*! Positional argument flag (when \c sopt and \c lopt are equals to default values) */
    bool positional;

    /*! Found state */
    bool found;
    // Attribute -

  public:
    // Constructors +
    /*!
     * \ref ArgState default constructor
     */
    ArgState(): positional(false), found(false) {
      // Nothing to do
    }
    // Constructors -

    // Destructor +
    /*!
     * \ref ArgState destructor
     */
    virtual ~ArgState() {
      // Nothing to do
    }
    // Destructor -

  };

  /*!
   * Registered argument (declaration + state)
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class RegisteredArg {
    friend anch::cli::ArgHandler;

  private:
    // Attributes +
    /*! Argument state */
    std::shared_ptr<anch::cli::ArgState> state;

    /*! The argument defintion pointer */
    std::shared_ptr<anch::cli::Arg> arg;
    // Attributes -

  public:
    // Constructors +
    /*!
     * \ref RegisteredArg constructor
     *
     * \param arg the argument to register
     */
    RegisteredArg(const anch::cli::Arg& arg) {
      this->arg = std::make_shared<anch::cli::Arg>(arg);
      this->state = std::make_shared<anch::cli::ArgState>();
    }
    // Constructors -

    // Destructor +
    /*!
     * \ref RegisteredArg destructor
     */
    virtual ~RegisteredArg() {
      // Nothing to do
    }
    // Destructor -


  };

}  // anch::cli

ArgHandler::ArgHandler(const App& app, const std::vector<Arg>& options):
  _app(app),
  _options(),
  _sopts(),
  _lopts(),
  _positionals() {
  for(auto option : options) {
    arg(option);
  }
}

ArgHandler::ArgHandler(const std::vector<Arg>& options):
  _app(),
  _options(),
  _sopts(),
  _lopts(),
  _positionals() {
  for(auto option : options) {
    arg(option);
  }
}

ArgHandler::~ArgHandler() {
  // Nothing to do
}

ArgHandler&
ArgHandler::arg(anch::cli::Arg& arg) {
  if(!arg.handler) {
    throw std::invalid_argument("Argument has to have handling function");
  }
  // \todo protect against non readable characters ...
  // Check already exist +
  if(arg.sopt != '\0' && _sopts.contains(arg.sopt)) {
    std::ostringstream oss;
    oss << '-' << arg.sopt << " has already been registered";
    throw std::invalid_argument(oss.str());
  }
  if(arg.lopt.has_value() && _lopts.contains(arg.lopt.value())) {
    std::ostringstream oss;
    oss << "--" << arg.lopt.value() << " has already been registered";
    throw std::invalid_argument(oss.str());
  }
  // Check already exist -
  std::size_t idx = _options.size();
  _options.push_back(std::make_shared<anch::cli::RegisteredArg>(arg));
  std::shared_ptr<anch::cli::RegisteredArg> option = _options.at(idx);
  // Short option +
  if(arg.sopt != '\0') {
    _sopts.insert({arg.sopt, option});
  }
  // Short option -
  // Long option +
  if(arg.lopt.has_value()) {
    _lopts[arg.lopt.value()] = option;
  }
  // Long option -
  // Set positional flag +
  if(!arg.lopt.has_value() && arg.sopt == '\0') {
    option->state->positional = true;
  } else {
    option->state->positional = false;
  }
  // Set positional flag -
  // Positional +
  if(option->state->positional) {
    option->arg->value = true;
    option->arg->mandatory = true;
    if(!_positionals.empty()) {
      for(auto opt : _positionals) {
	if(opt->arg->multi) {
	  std::ostringstream oss;
	  oss << "Multiple positional argument has already been defined.\n"
	      << "Multiple positional must be defined at last position";
	  throw std::invalid_argument(oss.str());
	}
      }
    }
    _positionals.push_back(option);
  }
  // Positional -
  return *this;
}

template<typename O>
void
hasMoreArg(int argc, int idx, const O& option) {
  if(idx == argc - 1) { // reach last argument
    std::ostringstream oss;
    oss << "No more argument for option " << option << " which needs value";
    throw std::invalid_argument(oss.str());
  }
}

std::string
formatArg(const anch::cli::Arg& arg) {
  std::ostringstream oss;
  if(arg.sopt != '\0' && arg.lopt.has_value()) {
    oss << '-' << arg.sopt << "|--" << arg.lopt.value();
  } else if(arg.sopt != '\0') {
    oss << '-' << arg.sopt;
  } else {
    oss << "--" << arg.lopt.value();
  }
  return oss.str();
}

void
ArgHandler::parse(int argc, char** argv) {
  if(!_app.name.has_value()) {
    auto path = std::filesystem::path(argv[0]);
    _app.name = path.filename();
  }
  std::size_t nbPos = 0;
  bool ignoreOpt = false;
  for(int i = 1 ; i < argc ; ++i) {
    std::smatch matches;
    std::string opt(argv[i]);
    if(opt == "--") { // trigger ignore option ('-' or '--') status
      ignoreOpt = !ignoreOpt;
      continue;
    }
    // \todo protect against non readable characters ...
    if(!ignoreOpt && std::regex_match(opt, matches, SREGEX)) {
      // Short option +
      std::string opts = matches[1];
      std::size_t optLen = opts.size();
      for(char o : opts) {
	auto iter = _sopts.find(o);
	if(iter == _sopts.end()) {
	  std::ostringstream oss;
	  oss << "Unknown argument -" << o;
	  throw std::invalid_argument(oss.str());
	}
	auto option = iter->second;
	if(option->arg->value) {
	  if(optLen > 1) { // multiple options and value to consume
	    std::ostringstream oss;
	    oss << "Argument " << o << " consumes value and is defined in multiple arguments list: -" << opts;
	    throw std::invalid_argument(oss.str());
	  }
	  if(option->state->found && !option->arg->multi) {
	    std::ostringstream oss;
	    oss << "Argument " << formatArg(*(option->arg)) << " has already been found";
	    throw std::invalid_argument(oss.str());
	  }
	  hasMoreArg(argc, i, o);
	  std::invoke(option->arg->handler, argv[++i]);
	  option->state->found = true;
	} else {
	  std::invoke(option->arg->handler, ANCH_CLI_NOARG);
	  option->state->found = true;
	}
      }
      // Short option -

    } else if(!ignoreOpt && std::regex_match(opt, matches, LREGEX)) {
      // Long option +
      std::string opt = matches[1];
      auto iter = _lopts.find(opt);
      if(iter == _lopts.end()) {
	std::ostringstream oss;
	oss << "Unknown argument --" << opt;
	throw std::invalid_argument(oss.str());
      }
      auto option = iter->second;
      if(option->state->found && !option->arg->multi) {
	std::ostringstream oss;
	oss << "Argument " << formatArg(*(option->arg)) << " has already been found";
	throw std::invalid_argument(oss.str());
      }
      if(option->arg->value) {
	hasMoreArg(argc, i, opt);
	std::invoke(option->arg->handler, std::string(argv[++i]));
	option->state->found = true;
      } else {
	std::invoke(option->arg->handler, ANCH_CLI_NOARG);
	option->state->found = true;
      }
      // Long option -

    } else {
      // Postional +
      if(_positionals.empty()) {
	std::ostringstream oss;
	oss << "Unknown argument " << opt;
	throw std::invalid_argument(oss.str());
      }
      if(nbPos >= _positionals.size()) {
	auto option = _positionals[_positionals.size() - 1];
	if(!option->arg->multi) {
	  throw std::invalid_argument("Too many positional arguments");
	} else {
	  std::invoke(option->arg->handler, opt);
	  option->state->found = true;
	}
      } else {
	auto option = _positionals[nbPos];
	std::invoke(option->arg->handler, opt);
	option->state->found = true;
      }
      ++nbPos;
      // Postional -
    }
  }
}

void
ArgHandler::check() {
  bool error = false;
  std::ostringstream oss;
  for(auto option : _options) {
    if(!option->arg->mandatory || option->state->found) {
      continue;
    }
    if(option->arg->sopt != '\0' || option->arg->lopt.has_value()) {
      if(option->arg->sopt != '\0') {
	oss << " -" << option->arg->sopt;
      }
      if(option->arg->lopt.has_value()) {
	oss << " --" << option->arg->lopt.value();
      }
      if(option->arg->value) {
	oss << ' ' << (option->arg->name.has_value() ? option->arg->name.value() : "ARG");
      }
    } else {
      oss << ' ' << (option->arg->name.has_value() ? option->arg->name.value() : "ARG");
    }
    oss << ',';
    error = true;
  }
  if(error) {
    std::string errors = oss.str();
    throw std::invalid_argument(errors.substr(1, errors.length() - 2));
  }
}

void
ArgHandler::handle(int argc, char** argv) {
  build(argv[0]);
  try {
    parse(argc, argv);
  } catch(const std::invalid_argument& e) {
    std::cerr << "Invalid arguments: " << e.what() << std::endl;
    printHelp(std::cerr);
    std::exit(1);
  } catch(const std::exception& e) {
    std::cerr << "Error while handling argument: " << e.what() << std::endl;
    printHelp(std::cerr);
    std::exit(3);
  }
  try {
    check();
  } catch(const std::invalid_argument& e) {
    std::cerr << "Missing arguments: " << e.what() << std::endl;
    printHelp(std::cerr);
    std::exit(2);
  }
}

void
ArgHandler::printBanner(std::ostream& out) {
  if(_app.bannerPath.has_value()) {
    try {
      std::ifstream fis(_app.bannerPath.value());
      fis >> out.rdbuf();
    } catch(...) {
      out << "Can not read " << _app.bannerPath.value();
    }
    out << '\n';
  }
  if(_app.banner.has_value()) {
    out << _app.banner.value() << '\n';
  }
}

void
ArgHandler::build(const std::string& arg0) {
  if(!_app.name.has_value()) {
    std::filesystem::path path(arg0);
    _app.name = path.filename();
  }
  // Register help argument +
  if(!_sopts.contains('h') || !_lopts.contains("help")) {
    anch::cli::Arg help;
    help.description = "Print help and exit";
    if(!_sopts.contains('h')) {
      help.sopt = 'h';
    }
    if(!_lopts.contains("help")) {
      help.lopt = "help";
    }
    help.handler = [this](const std::string&) {
      printHelp(std::cout);
      std::exit(0);
    };
    arg(help);
  }
  // Register help argument -
  // Register version argument +
  if(_app.version.has_value()) {
    if(!_sopts.contains('v') || !_lopts.contains("version")) {
      Arg version;
      version.description = "Print version and exit";
      if(!_sopts.contains('v')) {
	version.sopt = 'v';
      }
      if(!_lopts.contains("version")) {
	version.lopt = "version";
      }
      version.handler = [this](const std::string&) {
	printVersion(std::cout);
	std::exit(0);
      };
      arg(version);
    }
  }
  // Register version argument -
}

void
ArgHandler::printAppVersion(std::ostream& out) {
  out << _app.name.value();
  if(_app.version.has_value()) {
    out << ", version " << _app.version.value();
  }
  out << '\n';
}

bool
ArgHandler::printUsage(std::ostream& out) {
  out << "Usage: " << _app.name.value();
  bool opts = false;
  if(!_sopts.empty() || !_lopts.empty()) {
    opts = true;
  }
  if(opts) {
    out << " [OPTIONS]";
  }
  for(std::size_t i = 0 ; i < _positionals.size() ; ++i) {
    auto opt = _positionals[i];
    std::ostringstream oss;
    out << ' ';
    if(opt->arg->name.has_value()) {
      oss << opt->arg->name.value();
    } else {
      oss << "ARG" << i;
    }
    out << oss.str();
    if(opt->arg->multi) {
      out << '+';
    }
  }
  out << '\n';
  return opts;
}

void
ArgHandler::printOptions(std::ostream& out) {
  out << "OPTIONS:";
  std::size_t optLen = 0;
  std::size_t valLen = 0;
  std::map<std::string,std::shared_ptr<anch::cli::RegisteredArg>> opts;
  for(auto option : _options) {
    if(option->state->positional) {
      continue;
    }
    // Register option max length +
    std::string helpStr = formatArg(*(option->arg));
    if(helpStr.length() > optLen) {
      optLen = helpStr.length();
    }
    // Register option max length -
    // Register option's value max length +
    if(option->arg->value) {
      std::size_t curLen = 3; // length for 'ARG'
      if(option->arg->name.has_value()) {
	curLen = option->arg->name.value().length();
      }
      if(curLen > valLen) {
	valLen = curLen;
      }
    }
    // Register option's value max length -
    opts[helpStr] = option;
  }

  // Print options +
  std::string expad(6 + optLen + valLen, ' ');
  for(auto iter = opts.begin() ; iter != opts.end() ; ++iter) {
    out << "\n  " << std::left << std::setw(static_cast<int>(optLen)) << iter->first
	<< "  " << std::left << std::setw(static_cast<int>(valLen));
    if(iter->second->arg->value) {
      if(iter->second->arg->name.has_value()) {
	out << iter->second->arg->name.value();
      } else {
	out << "ARG";
      }
    } else {
      out << "";
    }
    out << "  ";
    if(iter->second->arg->multi) {
      out << "[+] ";
    }
    if(iter->second->arg->description.has_value()) {
      out << iter->second->arg->description.value();
    }
    if(iter->second->arg->example.has_value()) {
      out << '\n' << expad << "Example: " << iter->second->arg->example.value();
    }
  }
  // Print options -
}

void
ArgHandler::printHelp(std::ostream& out) {
  printAppVersion(out);
  bool opts = printUsage(out);
  if(_app.description.has_value()) {
    out << _app.description.value() << '\n';
  }
  // \todo example
  if(opts) {
    printOptions(out);
  }
  out << std::endl;
}

void
ArgHandler::printVersion(std::ostream& out) {
  printAppVersion(out);
  if(_app.copyright.has_value()) {
    out << '\n' << "Copyright © " << _app.copyright.value();
  }
  if(_app.licence.has_value()) {
    out << '\n' << "Licence: " << _app.licence.value();
  }
  if(_app.author.has_value()) {
    out << '\n' << "Author: " << _app.author.value();
  }
  out << std::endl;
}
