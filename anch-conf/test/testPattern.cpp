#include <regex>
#include <iostream>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

const std::regex PH_PATTERN("\\$\\{([^}]+)\\}");
const std::regex RSV_PATTERN("([^=|]+)=([^|]+)");

void
check(const std::string& url, const std::vector<std::string>& placeholders) {
  auto iter = std::sregex_iterator(url.begin(), url.end(), PH_PATTERN);
  auto end = std::sregex_iterator();
  anch::ut::assert(static_cast<std::size_t>(std::distance(iter, end)) == placeholders.size(), "Found {} tokens instead of {}", std::distance(iter, end), placeholders.size());
  for(std::size_t i = 0 ; iter != end ; ++iter, ++i) {
    std::smatch sub = *iter;
    anch::ut::assert(sub.size() == 2, "Found {} placeholders instead of {}", sub.size(), 2);
    std::string ph = sub.str(1);
    anch::ut::assert(ph == placeholders[i], "Found placeholder {} instead of {}", ph, placeholders[i]);
    // \todo parse resolvers
  }
}

void
checkConf() {
  std::cout << "Enter in checkConf" << std::endl;
  std::string url = "https://${conf=host}/accounts/f53f5ecc-fe11-4ad0-af2e-26590831b446";
  check(url, { "conf=host" });
  std::cout << "Exit checkConf" << std::endl;
}

void
checkConfDefault() {
  std::cout << "Enter in checkConfDefault" << std::endl;
  std::string url = "https://${conf=host|toto.titi.org}/accounts/dbf31be2-2d81-474a-969e-1b85341c8150";
  check(url, { "conf=host|toto.titi.org" });
  std::cout << "Exit checkConfDefault" << std::endl;
}

void
checkEnvConfDefault() {
  std::cout << "Enter in checkEnvConfDefault" << std::endl;
  std::string url = "https://${conf=host|env=TOTO_ACCOUNT|toto.titi.org}/accounts/${env=TOTO_ACCOUNT|conf=toto.account-id}";
  check(url, {
      "conf=host|env=TOTO_ACCOUNT|toto.titi.org",
      "env=TOTO_ACCOUNT|conf=toto.account-id"
    });
  std::cout << "Exit checkEnvConfDefault" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH conf resolver patterns unit tests")
    .description("Test AnCH conf resolver patterns")
    .add("check-conf", checkConf)
    .add("check-conf-default", checkConfDefault)
    .add("check-env-conf-default", checkEnvConfDefault)
    ;
}
