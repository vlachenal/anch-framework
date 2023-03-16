#include "cli/utils.hpp"
#include "cli/args.hpp"

#include <iostream>
#include <vector>
#include <filesystem>
#include <functional>
#include <cstring>
#include <fstream>
#include <memory>
#include <filesystem>

using anch::cli::App;
using anch::cli::Arg;
using anch::cli::ArgHandler;

struct Options {
  bool verbose = false;
  std::optional<std::string> str;
  std::optional<std::string> str2;
  std::optional<std::string> pos;
  std::optional<std::string> pos2;
  std::vector<std::string> multPos;
  std::shared_ptr<std::istream> pipeopt;
};

void
parseArgs(const App& application, Options& opts, int argc, char** argv) {
  std::cout << "Parse arguments for application " << argv[0] << std::endl;
  for(int i = 1 ; i < argc ; ++i) {
    std::cout << "arg[" << i << "] = " << argv[i] << std::endl;
  }
  ArgHandler handler(application, {
      {.handler = anch::cli::bindTrue(opts.verbose), .sopt = 'v', .lopt = "verbose", .description = "Verbose mode"},
      {.handler = anch::cli::bindStr(opts.str), .sopt = 'p', .lopt = "plop", .value = true, .name = "trestreslong", .description = "plop arg", .example = "PLOP!!!"},
      {.handler = anch::cli::bindStr(opts.str2), .lopt = "plip", .value = true, .description = "plip arg"},
      {.handler = anch::cli::bindStr(opts.pos), .value = true, .name = "POS1", .description = "pos"},
      {.handler = anch::cli::bindStr(opts.pos2), .value = true, .name = "POS2", .description = "pos2"},
      {.handler = anch::cli::bindCol(opts.multPos), .value = true, .name = "POSM", .multi = true, .description = "pos-mult"}
    });
  handler.printBanner(std::cout);
  handler.handle(argc, argv);
}

void
printOptions(const Options& opts) {
  std::cout << "Parsed values:" << std::endl;
  std::cout << "Verbose: " << opts.verbose << std::endl;
  std::cout << "Str: " << (opts.str.has_value() ? opts.str.value() : "not set") << std::endl;
  std::cout << "Str2: " << (opts.str2.has_value() ? opts.str2.value() : "not set") << std::endl;
  std::cout << "pos: " << (opts.pos.has_value() ? opts.pos.value() : "not set") << std::endl;
  std::cout << "pos2: " << (opts.pos2.has_value() ? opts.pos2.value() : "not set") << std::endl;
  std::cout << "multi pos: ";
  if(opts.multPos.empty()) {
    std::cout << "empty";
  } else {
    for(auto iter = opts.multPos.cbegin() ; iter != opts.multPos.cend() ; ++iter) {
      if(iter != opts.multPos.cbegin()) {
	std::cout << " ; ";
      }
      std::cout << *iter;
    }
  }
  std::cout << std::endl;
  if(opts.pipeopt) {
    std::cout << "pipeopt:" << std::endl;
    std::cout << opts.pipeopt->rdbuf() << std::endl;
  } else {
    std::cout << "pipeopt: not set" << std::endl;
  }
  std::cout << std::endl;
}

void
testOK(const App& application, Options& opts, int argc, char** argv) {
  parseArgs(application, opts, argc, argv);
  printOptions(opts);
}

void
testPlop(const App& application, Options& opts, int argc, char** argv) {
  parseArgs(application, opts, argc, argv);
  printOptions(opts);
  if(!opts.str.has_value()) {
    std::cerr << "Plop has no value" << std::endl;
    std::exit(1);
  }
}

void
testPlip(const App& application, Options& opts, int argc, char** argv) {
  parseArgs(application, opts, argc, argv);
  printOptions(opts);
  if(!opts.str2.has_value()) {
    std::cerr << "Plip has no value" << std::endl;
    std::exit(1);
  }
}

void
testPos1(const App& application, Options& opts, int argc, char** argv) {
  parseArgs(application, opts, argc, argv);
  printOptions(opts);
  if(!opts.pos.has_value()) {
    std::cerr << "pos has no value" << std::endl;
    std::exit(1);
  }
}

void
testPos2(const App& application, Options& opts, int argc, char** argv) {
  parseArgs(application, opts, argc, argv);
  printOptions(opts);
  if(!opts.pos2.has_value()) {
    std::cerr << "pos2 has no value" << std::endl;
    std::exit(1);
  }
}

void
testPosmMulti(const App& application, Options& opts, int argc, char** argv) {
  parseArgs(application, opts, argc, argv);
  printOptions(opts);
  if(opts.multPos.empty()) {
    std::cerr << "multPos has no value" << std::endl;
    std::exit(1);
  }
}

void
setPath(std::shared_ptr<std::istream>& input, const std::string& val) {
  std::filesystem::path path(val);
  if(!std::filesystem::exists(path)) {
    std::cerr << "File path " << val << " does not exists" << std::endl;
    std::exit(1);
  }
  if(!std::filesystem::is_regular_file(path)) {
    std::cerr << "File path " << val << " is not a regular file" << std::endl;
    std::exit(1);
  }
  input = std::make_shared<std::ifstream>(path);
}

void
parsePipeArgs(const App& application, Options& opts, int argc, char** argv) {
  std::cout << "Parse arguments for application " << argv[0] << std::endl;
  for(int i = 1 ; i < argc ; ++i) {
    std::cout << "arg[" << i << "] = " << argv[i] << std::endl;
  }
  ArgHandler handler(application, {
      {.handler = anch::cli::bindIFS(opts.pipeopt), .sopt = 'i', .lopt = "input", .value = true, .name = "IS", .mandatory = true, .pipe = anch::cli::bindPipe(opts.pipeopt), .description = "input stream"}
    });
  handler.printBanner(std::cout);
  handler.handle(argc, argv);
}

void
testPipe(const App& application, Options& opts, int argc, char** argv) {
  parsePipeArgs(application, opts, argc, argv);
  if(!opts.pipeopt) {
    std::cerr << "pipeopt has no value" << std::endl;
    std::exit(1);
  }
  printOptions(opts);
}

int
main(int argc, char** argv) {
  App application = {
    .name = "toto",
    .version = "0.1 alpha 42",
    .author = "Vincent Lachenal",
    .copyright = "No copyrigth - it is a fuking unit test",
    .licence = "WTFPL 2.0",
    .banner = "Ceci est une bannière",
    .bannerPath = "/etc/hostname"
  };
  Options opts;
  if(argc == 1) {
    std::cerr << "missing arg ..." << std::endl;
    return 1;
  }
  char** args = new char*[static_cast<std::size_t>(argc - 1)];
  int tidx = 0;
  std::string test;
  for(int i = 0 ; i < argc ; ++i) {
    if(i == 1) {
      test = argv[i];
      continue;
    }
    char* arg = new char[strlen(argv[i])];
    strcpy(arg, argv[i]);
    args[tidx] = arg;
    ++tidx;
  }
  std::cout << "Unit test: " << test << std::endl;
  int argcc = argc - 1;
  std::map<std::string, std::function<void(void)>> tests = {
    {"help", std::bind(testOK, application, opts, argcc, args)},
    {"version", std::bind(testOK, application, opts, argcc, args)},
    {"plop", std::bind(testPlop, application, opts, argcc, args)},
    {"plip", std::bind(testPlip, application, opts, argcc, args)},
    {"missing", std::bind(testOK, application, opts, argcc, args)},
    {"pos1", std::bind(testPos1, application, opts, argcc, args)},
    {"pos2", std::bind(testPos2, application, opts, argcc, args)},
    {"posm", std::bind(testPosmMulti, application, opts, argcc, args)},
    {"stream", std::bind(testPipe, application, opts, argcc, args)}
  };
  if(!tests.contains(test)) {
    std::cerr << "Unknown test: " << test << std::endl;
    return 1;
  }
  std::invoke(tests[test]);
  return 0;
}
