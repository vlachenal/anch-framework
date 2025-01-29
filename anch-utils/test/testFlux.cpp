#include "flux.hpp"

#include <iostream>

#include "ut/unit.hpp"
#include "ut/assert.hpp"


struct Toto {
  std::string msg;
};

std::ostream&
operator<<(std::ostream& out, const Toto& toto) {
  out << "Toto: " << toto.msg << std::endl;
  return out;
}

void
okToto(const Toto& toto) {
  std::cout << toto;
}

int nbErr = 0;

void
koToto(const Toto& toto) {
  if(toto.msg == "ko") {
    throw std::string("Raise on ko");
  }
  std::cout << toto;
}

template<>
void
anch::Flux<Toto>::handleError() {
  nbErr += 1;
  try {
    throw;
  } catch(const std::string& err) {
    std::cerr << "Toto error: " << err << std::endl;
  } catch(...) {
    std::cerr << "Unexpected error" << std::endl;
    throw;
  }
}

void
testPushOK() {
  anch::Flux<Toto> flux(okToto);
  Toto toto;
  toto.msg = "a";
  flux.push(toto);
  toto.msg = "b";
  flux.push(toto);
}

void
testPushKO() {
  anch::Flux<Toto> flux(koToto);
  Toto toto;
  toto.msg = "ko";
  flux.push(toto);
  toto.msg = "a";
  flux.push(toto);
  toto.msg = "b";
  flux.push(toto);
  anch::ut::assert(nbErr == 1, "Number of errors should be equals to 1");
}

void
beforeEach() {
  nbErr = 0;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH flux unit tests")
    .description("Test AnCH flux utiliy class")
    .beforeTest(beforeEach)
    .add("push-ok", testPushOK)
    .add("push-ko", testPushKO)
    ;
}
