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

void
handleTotoError() {
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
finalize() {
  std::cout << "Finalize flux" << std::endl;
}

void
testPushOK() {
  std::cout << "Enter in testPushOK" << std::endl;
  anch::Flux<Toto> flux;
  flux.setConsumer(okToto);
  flux.setFinalizer(finalize);
  flux.setErrorHandler(handleTotoError);
  flux.ready();
  Toto toto;
  toto.msg = "a";
  flux.push(toto);
  toto.msg = "b";
  flux.push(toto);
  std::cout << "finalize" << std::endl;
  flux.finalize();
  std::cout << "Exit testPushOK" << std::endl;
}

void
testPushAsyncOK(int nbPush) {
  std::cout << "Enter in testPushAsyncOK" << std::endl;
  anch::Flux<Toto> flux;
  // Start push early +
  std::thread t([&flux, &nbPush]() {
    Toto toto;
    for(int i = 0 ; i < nbPush ; ++i) {
      toto.msg = std::to_string(i);
      flux.push(toto);
    }
    std::cout << "finalize" << std::endl;
    flux.finalize();
  });
  std::mutex m;
  m.lock();
  t.detach();
  // Start push early -
  flux.setConsumer(okToto);
  flux.setFinalizer([&m]() {
    finalize();
    m.unlock();
  });
  flux.setErrorHandler(handleTotoError);
  flux.ready();
  m.lock();
  m.unlock();
  std::cout << "Exit testPushAsyncOK(" << nbPush << ')' << std::endl;
}

void
testPushSyncOK(int nbPush) {
  std::cout << "Enter in testPushSyncOK" << std::endl;
  anch::Flux<Toto> flux;
  flux.setConsumer(okToto);
  flux.setFinalizer(finalize);
  flux.setErrorHandler(handleTotoError);
  flux.ready();
  Toto toto;
  for(int i = 0 ; i < nbPush ; ++i) {
    toto.msg = std::to_string(i);
    flux.push(toto);
  }
  std::cout << "finalize" << std::endl;
  flux.finalize();
  std::cout << "Exit testPushSyncOK(" << nbPush << ')' << std::endl;
}

void
testPushKO() {
  std::cout << "Enter in testPushKO" << std::endl;
  anch::Flux<Toto> flux;
  flux.setConsumer(koToto);
  flux.setErrorHandler(handleTotoError);
  flux.ready();
  Toto toto;
  toto.msg = "ko";
  flux.push(toto);
  toto.msg = "a";
  flux.push(toto);
  toto.msg = "b";
  flux.push(toto);
  flux.finalize();
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

void
testPushMultiOK() {
  std::cout << "Enter in testPushMultiOK" << std::endl;
  anch::Flux<int,Toto> flux;
  flux.setConsumer(okTotoMulti);
  flux.setFinalizer(finalize);
  flux.setErrorHandler(handleTotoError);
  flux.ready();
  Toto toto;
  toto.msg = "a";
  flux.push(1, toto);
  toto.msg = "b";
  flux.push(2, toto);
  flux.finalize();
  std::cout << "Exit testPushMultiOK" << std::endl;
}

void
testPushMultiKO() {
  std::cout << "Enter in testPushMultiKO" << std::endl;
  anch::Flux<int,Toto> flux;
  flux.setConsumer(koTotoMulti);
  flux.setErrorHandler(handleTotoError);
  flux.ready();
  Toto toto;
  toto.msg = "ko";
  flux.push(1, toto);
  toto.msg = "a";
  flux.push(2, toto);
  toto.msg = "b";
  flux.push(3, toto);
  flux.finalize();
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
    .add("push-async-ok", std::bind(testPushAsyncOK, 1000))
    .add("push-async-10-ok", std::bind(testPushAsyncOK, 10))
    .add("push-async-100-ok", std::bind(testPushAsyncOK, 100))
    .add("push-async-10000-ok", std::bind(testPushAsyncOK, 10000))
    .add("push-sync-10-ok", std::bind(testPushSyncOK, 10))
    .add("push-sync-100-ok", std::bind(testPushSyncOK, 100))
    .add("push-sync-1000-ok", std::bind(testPushSyncOK, 1000))
    .add("push-sync-10000-ok", std::bind(testPushSyncOK, 10000))
    ;
}
