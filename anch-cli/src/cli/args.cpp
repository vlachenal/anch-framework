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
#include "cli/formatter.hpp"
#include "cli/utils.hpp"

#include <sstream>
#include <stdexcept>
#include <regex>
#include <iostream>
#include <fstream>

using anch::cli::ArgHandler;
using anch::cli::Arg;
using anch::cli::App;
using anch::cli::Formatter;
using anch::cli::Color;

const std::string ANCH_CLI_NOARG;
const std::regex SREGEX("^-([^-]+)$");
const std::regex LREGEX("^--(.+)$");

const Formatter ERROR = Formatter::format().fgColor(Color::RED);
const Formatter INFO = Formatter::format().fgColor(Color::BLUE);

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

  std::string
  formatArg(const anch::cli::Arg& arg) {
    std::ostringstream oss;
    if(arg.sopt != '\0' && arg.lopt.has_value()) {
      oss << anch::cli::UNDERLINE << '-' << arg.sopt << anch::cli::UNUNDERLINE << '|' << anch::cli::UNDERLINE << "--" << arg.lopt.value() << anch::cli::UNUNDERLINE;
    } else if(arg.sopt != '\0') {
      oss << anch::cli::UNDERLINE << '-' << arg.sopt << anch::cli::UNUNDERLINE;
    } else {
      oss << anch::cli::UNDERLINE << "--" << arg.lopt.value() << anch::cli::UNUNDERLINE;
    }
    oss << anch::cli::RESET;
    return oss.str();
  }

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

    /*! Unformatted argument length */
    std::size_t length;

    /*! Formatted argument length (will be set on print help) */
    std::size_t formattedLength;

    /*! Value length */
    std::size_t valLength;
    // Attributes -

  public:
    // Constructors +
    /*!
     * \ref RegisteredArg constructor
     *
     * \param arg the argument to register
     */
    RegisteredArg(const anch::cli::Arg& arg): length(0), formattedLength(0), valLength(0) {
      this->arg = std::make_shared<anch::cli::Arg>(arg);
      this->state = std::make_shared<anch::cli::ArgState>();
      if(arg.sopt != '\0') {
	length += 2; // '-' + 1 character
      }
      if(arg.lopt.has_value()) {
	length += 2 + arg.lopt.value().length(); // '--' + option length
      }
      if(arg.sopt != '\0' && arg.lopt.has_value()) { // add '|' beetwwen options
	++length;
      }
      if(arg.value) {
	valLength = 3; // ARG
	if(arg.name.has_value()) {
	  valLength = arg.name.value().length();
	}
	if(arg.multi) {
	  ++valLength;
	}
      }
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

    // Accessors +
    /*!
     * Get console length
     *
     * \return the length
     */
    inline std::size_t getLength() const {
      return length;
    }

    /*!
     * Get console formatted length
     *
     * \return the length
     */
    inline std::size_t getFormattedLength() const {
      return formattedLength;
    }

    /*!
     * Set console formatted length
     *
     * \param length the length to set
     */
    inline void setFormattedLength(std::size_t length) {
      formattedLength = length;
    }

    /*!
     * Get console value length
     *
     * \return the length
     */
    inline std::size_t getValLength() const {
      return valLength;
    }
    // Accessors -

    // Methods +
    /*!
     * Format value
     *
     * \return the formatted value
     */
    std::string formatVal() const {
      std::ostringstream out;
      if(arg->value) {
	if(arg->name.has_value()) {
	  if(arg->multi) {
	    out << (arg->name.value() + "+");
	  } else {
	    out << arg->name.value();
	  }
	} else {
	  if(arg->multi) {
	    out << "ARG+";
	  } else {
	    out << "ARG";
	  }
	}
      } else {
	out << "";
      }
      return out.str();
    }

    /*!
     * Format argument
     *
     * \return the formatted argument
     */
    inline std::string formatArg() const {
      return anch::cli::formatArg(*arg);
    }
    // Methods -

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
    // Check mandatory state
    if(!option->arg->mandatory || option->state->found) {
      continue;
    }
    // Check pipe option
    if(!option->state->found && option->arg->pipe) {
      std::invoke(option->arg->pipe, std::cin);
      continue;
    }
    // Format error +
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
    // Format error -
  }
  // Format and raise error +
  if(error) {
    std::string errors = oss.str();
    throw std::invalid_argument(errors.substr(1, errors.length() - 2));
  }
  // Format and raise error +
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
  anch::cli::manageEnvNoFormat();
  // Set default application name to first argument when not set +
  if(!_app.name.has_value()) {
    std::filesystem::path path(arg0);
    _app.name = path.filename();
  }
  // Set default application name to first argument when not set -
  // Check multiple positinal options and piped options +
  bool multipos = false;
  bool piped = false;
  for(auto iter = _options.cbegin() ; iter != _options.cend() ; ++iter) {
    if((*iter)->state->positional && (*iter)->arg->multi) {
      if(!multipos) {
	multipos = true;
      } else {
	std::ostringstream oss;
	oss << "Multiple positional argument has already been defined.\n"
	    << "Multiple positional must be defined at last position";
	throw std::invalid_argument(oss.str());
      }
    }
    if((*iter)->arg->pipe) {
      if(!piped) {
	piped = true;
      } else {
	throw std::invalid_argument("Only one 'pipe' option can be declared");
      }
    }
  }
  // Check multiple positinal options and piped options -
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
  // Register no color argument +
  if(!_lopts.contains("no-term-format")) {
    anch::cli::Arg nc;
    nc.description = "Disable AnCH CLI formatting";
    if(!_lopts.contains("no-term-fmt")) {
      nc.lopt = "no-term-fmt";
    }
    nc.handler = [this](const std::string&) {
      anch::cli::Formatter::DISABLED = true;
    };
    arg(nc);
  }
  // Register no color argument -
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
  out << Formatter::format().fgColor(Color::CYAN) << _app.name.value();
  if(_app.version.has_value()) {
    out << ", version " << _app.version.value();
  }
  out << anch::cli::RESET << '\n';
}

bool
ArgHandler::printUsage(std::ostream& out) {
  out << INFO << "Usage: " << _app.name.value();
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
  out << '\n' << anch::cli::RESET;
  return opts;
}

void
ArgHandler::printOptions(std::ostream& out) {
  out << anch::cli::UNDERLINE << "OPTIONS:" << anch::cli::UNUNDERLINE;
  std::size_t optLen = 0;
  std::size_t valLen = 0;
  std::map<std::string,std::shared_ptr<anch::cli::RegisteredArg>> opts;
  for(auto option : _options) {
    if(option->state->positional) {
      continue;
    }
    // Register option max length +
    auto helpStr = option->formatArg();
    option->setFormattedLength(helpStr.length());
    if(option->getLength() > optLen) {
      optLen = option->getLength();
    }
    // Register option max length -
    // Register option's value max length +
    if(option->getValLength() > valLen) {
      valLen = option->getValLength();
    }
    // Register option's value max length -
    opts[helpStr] = option;
  }

  // Print options +
  std::string expad(6 + optLen + valLen, ' ');
  for(auto iter = opts.begin() ; iter != opts.end() ; ++iter) {
    out << "\n  " << std::left << std::setw(static_cast<int>(optLen + iter->second->getFormattedLength() - iter->second->getLength()))
	<< iter->first << "  " << std::left << std::setw(static_cast<int>(valLen)) << iter->second->formatVal() << "  ";
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
  out << Formatter::format().fgColor({127,127,127});
  if(_app.copyright.has_value()) {
    out << '\n' << "Copyright © " << _app.copyright.value();
  }
  if(_app.licence.has_value()) {
    out << '\n' << "Licence: " << _app.licence.value();
  }
  if(_app.author.has_value()) {
    out << '\n' << "Author: " << _app.author.value();
  }
  out << anch::cli::RESET << std::endl;
}
