#include "cli/formatter.hpp"

#include <iostream>
#include <map>
#include <functional>

using anch::cli::Formatter;
using anch::cli::Color;

void
testFgBlue() {
  std::cout << "Enter in testFgBlue" << std::endl;
  std::cout << Formatter::format().fgColor(Color::BLUE) << "I am blue text" << std::endl;
  std::cout << "and it is always blue after sdt::endl" << anch::cli::RESET << std::endl;
  std::cout << "Exit testFgBlue" << std::endl;
}

void
testBgBlue() {
  std::cout << "Enter in testBgBlue" << std::endl;
  std::cout << Formatter::format().bgColor(Color::BLUE) << "I am blue background" << std::endl;
  std::cout << "and it is always blue after new line" << anch::cli::RESET << std::endl;
  std::cout << "Exit testBgBlue" << std::endl;
}

void
testUnderline() {
  std::cout << "Enter in testUnderline" << std::endl;
  std::cout << Formatter::format().underline() << "I am underline text" << std::endl;
  std::cout << "and it is always underlined after new line" << anch::cli::RESET << std::endl;
  std::cout << "Exit testUnderline" << std::endl;
}

void
testBlink() {
  std::cout << "Enter in testBlink" << std::endl;
  std::cout << Formatter::format().blink() << "I am blink text" << std::endl;
  std::cout << "and it is always blink after new line" << anch::cli::RESET << std::endl;
  std::cout << "Exit testBlink" << std::endl;
}

void
testMix1() {
  std::cout << "Enter in testMix1" << std::endl;
  std::cout << Formatter::format().fgColor(Color::RED).bgColor(Color::BLUE).underline() << "I am foreground text + background text + underline text" << std::endl;
  std::cout << "and it is always formatted after new line" << anch::cli::RESET << std::endl;
  std::cout << "Exit testMix1" << std::endl;
}

void
testMix2() {
  std::cout << "Enter in testMix2" << std::endl;
  std::cout << Formatter::format().fgColor(Color::RED).bgColor(Color::BLUE).underline().blink() << "I am foreground text + background text + underline text + bink" << std::endl;
  std::cout << "and it is always formatted after new line" << anch::cli::RESET << std::endl;
  std::cout << "Exit testMix2" << std::endl;
}

int
main(int argc, char** argv) {
  std::map<std::string, std::function<void(void)>> tests = {
    {"fg_blue", testFgBlue},
    {"bg_blue", testBgBlue},
    {"underline", testUnderline},
    {"blink", testBlink},
    {"mix1", testMix1},
    {"mix2", testMix2},
  };
  if(argc == 1) { // run all tests
    for(auto iter = tests.cbegin() ; iter != tests.cend() ; ++iter) {
      std::invoke(iter->second);
    }
  } else {
    std::invoke(tests[argv[1]]);
  }
  return 0;
}
