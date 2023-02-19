//#include "cli/arg.hpp"
//#include "cli/app.hpp"
#include "cli/utils.hpp"
#include "cli/cli.hpp"

#include <iostream>
#include <vector>
#include <filesystem>
#include <functional>

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
};

void
setVerbose(Options& options, const std::string&) {
  options.verbose = true;
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
    .bannerPath = "/etc/fstab"
  };
  Options opts;
  {
    // std::vector<Arg> args = {
    //   {.handler = anch::cli::bindTrue(opts.verbose), .sopt = 'v', .lopt = "verbose", .description = "Verbose mode"},
    //   {.handler = anch::cli::bindStr(opts.str), .sopt = 'p', .lopt = "plop", .value = true, .name = "trestreslong", .description = "plop arg", .example = "PLOP!!!"},
    //   {.handler = anch::cli::bindStr(opts.str2), .lopt = "plip", .value = true, .description = "plip arg"},
    //   {.handler = anch::cli::bindStr(opts.pos), .value = true, .name = "POS1", .description = "pos"},
    //   {.handler = anch::cli::bindStr(opts.pos2), .value = true, .name = "POS2", .description = "pos2"},
    //   {.handler = anch::cli::bindCol(opts.multPos), .value = true, .name = "POSM", .multi = true, .description = "pos-mult"}
    // };
    // ArgHandler handler(application, args);
    ArgHandler handler(application, {
	{.handler = anch::cli::bindTrue(opts.verbose), .sopt = 'v', .lopt = "verbose", .description = "Verbose mode"},
	{.handler = anch::cli::bindStr(opts.str), .sopt = 'p', .lopt = "plop", .value = true, .name = "trestreslong", .description = "plop arg", .example = "PLOP!!!"},
	{.handler = anch::cli::bindStr(opts.str2), .lopt = "plip", .value = true, .description = "plip arg"},
	{.handler = anch::cli::bindStr(opts.pos), .value = true, .name = "POS1", .description = "pos"},
	{.handler = anch::cli::bindStr(opts.pos2), .value = true, .name = "POS2", .description = "pos2"},
	{.handler = anch::cli::bindCol(opts.multPos), .value = true, .name = "POSM", .multi = true, .description = "pos-mult"}
      });
    handler.printBanner(std::cerr);
    handler.handle(argc, argv);
  }
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
  return 0;
}
