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
  std::cout << "Enter in testPushOK" << std::endl;
  anch::Flux<Toto> flux(okToto);
  Toto toto;
  toto.msg = "a";
  flux.push(toto);
  toto.msg = "b";
  flux.push(toto);
  std::cout << "Exit testPushOK" << std::endl;
}

void
testPushKO() {
  std::cout << "Enter in testPushKO" << std::endl;
  anch::Flux<Toto> flux(koToto);
  Toto toto;
  toto.msg = "ko";
  flux.push(toto);
  toto.msg = "a";
  flux.push(toto);
  toto.msg = "b";
  flux.push(toto);
  anch::ut::assert(nbErr == 1, "Number of errors should be equals to 1");
  std::cout << "Exit testPushKO" << std::endl;
}

void
okTotoMulti(int i, const Toto& toto) {
  std::cout << i << "->" << toto;
}

void
koTotoMulti(int i, const Toto& toto) {
  if(toto.msg == "ko") {
    throw std::string("Raise on ko");
  }
  std::cout << i << "->" << toto;
}

template<>
void
anch::Flux<int, Toto>::handleError() {
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
testPushMultiOK() {
  std::cout << "Enter in testPushMultiOK" << std::endl;
  anch::Flux<int,Toto> flux(okTotoMulti);
  Toto toto;
  toto.msg = "a";
  flux.push(1, toto);
  toto.msg = "b";
  flux.push(2, toto);
  std::cout << "Exit testPushMultiOK" << std::endl;
}

void
testPushMultiKO() {
  std::cout << "Enter in testPushMultiKO" << std::endl;
  anch::Flux<int,Toto> flux(koTotoMulti);
  Toto toto;
  toto.msg = "ko";
  flux.push(1, toto);
  toto.msg = "a";
  flux.push(2, toto);
  toto.msg = "b";
  flux.push(3, toto);
  anch::ut::assert(nbErr == 1, "Number of errors should be equals to 1");
  std::cout << "Exit testPushMultiKO" << std::endl;
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
    .add("push-ok-multi", testPushMultiOK)
    .add("push-ko-multi", testPushMultiKO)
    ;
}
