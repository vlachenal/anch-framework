#include "json/reader.hpp"

#include <sstream>
#include <deque>
#include <any>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

#include "events/observer.hpp"

using anch::json::EventType;
using anch::json::JSONItem;

class PrintObserver: public anch::events::Observer<JSONItem> {

  virtual void handle(const anch::events::Event<JSONItem>& event) noexcept {
    switch(event.body.getType()) {
    case EventType::BEGIN_OBJECT:
      std::cout << '{';
      break;
    case EventType::END_OBJECT:
      std::cout << '}';
      break;
    case EventType::BEGIN_ARRAY:
      std::cout << '[';
      break;
    case EventType::END_ARRAY:
      std::cout << ']';
      break;
    case EventType::TRUE:
      std::cout << "true";
      break;
    case EventType::FALSE:
      std::cout << "false";
      break;
    default:
      std::cout << "plop ";
    }
  }

};

class ErrorObserver: public anch::events::Observer<anch::json::MappingError> {

  virtual void handle(const anch::events::Event<anch::json::MappingError>& event) noexcept {
    std::cerr << "Parsing error: " << event.body.what() << std::endl;
    std::exit(1);
  }

};

void
testPrintAll() {
  //const std::string totoRes = "{\"plop\":\"plop\",\"plip\":\"plip\",\"plap\":42,\"plup\":false,\"tata\":{\"ploum\":\"ploum\",\"view\":\"VIEW\",\"num_set\":[1,2,3],\"str_vector\":[\"4\",\"5\",\"6\"]},\"plep\":2.2,\"plyp\":3.3,\"self\":{\"plop\":\"self\",\"plip\":\"self_plip\",\"plap\":24,\"plup\":true,\"tata\":{\"ploum\":\"\",\"view\":\"\",\"num_set\":[],\"str_vector\":[]},\"plep\":5.5,\"plyp\":6.6,\"ptr\":\"self\",\"class\":\"Toto\"},\"ptr\":\"plop\",\"class\":\"Toto\"}";
  const std::string totoRes = "{\"plop\":\"plop\",\"plip\":\"plip\",\"plap\":42,\"plup\":false}";
  std::deque<std::any> expected = {
    EventType::BEGIN_OBJECT,
    "plop",
    "plop",
    "plip",
    "plip",
    "plap",
    42,
    "plup",
    EventType::FALSE,
    EventType::END_OBJECT
  };
  PrintObserver print;
  ErrorObserver error;
  std::istringstream iss(totoRes);
  anch::json::Reader reader(iss, anch::json::DEFAULT_MAPPING_OPTIONS);
  reader.itemObs().addObserver(print);
  reader.errorObs().addObserver(error);
  reader.parse();
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH JSON reader unit tests")
    .description("Test AnCH JSON reader library")
    .add("reader-print-all", testPrintAll)
    ;
}
