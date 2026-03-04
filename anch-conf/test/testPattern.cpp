#include <regex>
#include <iostream>
#include <map>
#include <vector>
#include <string>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

const std::regex PH_PATTERN("\\$\\{([^}]+)\\}");
const std::regex RSV_PATTERN("(([^=|]+)=)?([^|]+)");
auto end = std::sregex_iterator();

struct Resolver {
  std::string res = "";
  std::string val;
};

using Placeholders = std::map<std::string, std::vector<Resolver>>;

/*!
 * Find resolver in placeholders
 *
 * \param phs the placeholder
 * \param resolvers the expected resolvers
 */
void
resolv(const std::string& phs, const std::vector<Resolver>& resolvers) {
  std::cout << "Expected resolvers:" << std::endl;
  for(auto res : resolvers) {
    std::cout << " - '" << res.res << "' = '" << res.val << '\'' << std::endl;
  }
  auto iter = std::sregex_iterator(phs.begin(), phs.end(), RSV_PATTERN);
  std::size_t nbRes = static_cast<std::size_t>(std::distance(iter, std::sregex_iterator()));
  anch::ut::assert(nbRes == resolvers.size(), "Found {} tokens instead of {}", nbRes, resolvers.size());
  for(std::size_t i = 0 ; iter != std::sregex_iterator() ; ++iter, ++i) {
    std::smatch sub = *iter;
    anch::ut::assert(sub.size() == 4, "Found {} placeholders instead of {}", sub.size(), 4);
    std::cout << std::endl << "Found:    resolver = '" << sub.str(2) << "' : value = '" << sub.str(3) << '\'' << std::endl;
    std::cout << "Expected: resolver = '" << resolvers[i].res << "' ; value = '" << resolvers[i].val << '\'' << std::endl;
    anch::ut::assert(sub.str(2) == resolvers[i].res, "Resolver '{}' should be '{}'", sub.str(2), resolvers[i].res);
    anch::ut::assert(sub.str(3) == resolvers[i].val, "Resolver '{}' should be '{}'", sub.str(3), resolvers[i].val);
  }
}

/*!
 * Check placeholders and their resolvers
 *
 * \param url the string to parse
 * \param placeholders the expected result
 */
void
check(const std::string& url, const Placeholders& placeholders) {
  std::cout << std::endl << "Parse " << url << std::endl << "Expected placeholders:" << std::endl;
  for(auto iter = placeholders.begin() ; iter != placeholders.end() ; ++iter) {
    std::cout << " - " << iter->first << std::endl;
  }
  auto iter = std::sregex_iterator(url.begin(), url.end(), PH_PATTERN);
  anch::ut::assert(static_cast<std::size_t>(std::distance(iter, end)) == placeholders.size(), "Found {} tokens instead of {}", std::distance(iter, end), placeholders.size());
  for(auto iter = std::sregex_iterator(url.begin(), url.end(), PH_PATTERN) ; iter != end ; ++iter) {
    std::smatch sub = *iter;
    anch::ut::assert(sub.size() == 2, "Found {} placeholders instead of {}", sub.size(), 2);
    std::string ph = sub.str(1); // sub.str(0) matches the complete PH_PATTERN with ${}
    std::cout << std::endl << "Found placeholders: " << ph << std::endl;
    auto it = placeholders.find(ph);
    anch::ut::assert(it != placeholders.end(), "Placeholder '{}' has not been found", ph);
    resolv(ph, it->second);
  }
  std::cout << std::endl;
}

/*!
 * Check string with 1 placeholder with 1 resolver
 */
void
checkConf() {
  std::cout << "Enter in checkConf" << std::endl;
  check("https://${conf=host}/accounts/f53f5ecc-fe11-4ad0-af2e-26590831b446", {
      { "conf=host", {
	  { .res = "conf", .val = "host"}
	}
      }
    });
  std::cout << "Exit checkConf" << std::endl;
}

/*!
 * Check string with 1 placeholder with 1 resolver and default value
 */
void
checkConfDefault() {
  std::cout << "Enter in checkConfDefault" << std::endl;
  check("https://${conf=host|toto.titi.org}/accounts/dbf31be2-2d81-474a-969e-1b85341c8150", {
      { "conf=host|toto.titi.org", {
	  { .res = "conf", .val = "host"},
	  { .val = "toto.titi.org" }
	}
      }
    });
  std::cout << "Exit checkConfDefault" << std::endl;
}

/*!
 * Check string with several placeholders with multiple resolvers and default value
 */
void
checkEnvConfDefault() {
  std::cout << "Enter in checkEnvConfDefault" << std::endl;
  check("https://${conf=toto.host|env=TOTO_HOST|toto.titi.org}/accounts/${env=TOTO_ACCOUNT|conf=toto.account-id}", {
      {
	"conf=toto.host|env=TOTO_HOST|toto.titi.org", {
	  { .res = "conf", .val = "toto.host"},
	  { .res = "env", .val = "TOTO_HOST"},
	  { .val = "toto.titi.org" }
	}
      },
      {
	"env=TOTO_ACCOUNT|conf=toto.account-id", {
	  { .res = "env", .val = "TOTO_ACCOUNT"},
	  { .res = "conf", .val = "toto.account-id" }
	}
      }
    });
  std::cout << "Exit checkEnvConfDefault" << std::endl;
}

/*!
 * Register unit tests
 */
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
