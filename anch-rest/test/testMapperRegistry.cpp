#include "rest/mapperRegistry.hpp"

#include <iostream>
#include <sstream>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::rest::MapperRegistry;
using anch::json::JSONMapper;
using anch::json::ObjectMapper;
using anch::rest::MediaType;

auto&
anch::rest::castMapper(const std::string& contentType, std::any& mapper) {
  if(contentType == anch::rest::MediaType::APP_JSON) {
    return std::any_cast<anch::json::JSONMapper&>(mapper);
  }
  std::ostringstream out;
  out << contentType << " has not been defined in mapper caster function";
  throw anch::rest::MapperException(out.str());
}

struct Tata {
  std::string ploum;
  std::string_view view;
  std::list<Tata> selfList;
  std::vector<std::string> strVect;
  std::set<uint64_t> numSet;
  const std::string_view& getView() const {
    return view;
  }
};

template<>
void
anch::json::registerObject(ObjectMapper<Tata>& mapper) {
  std::cout << "Register Tata fields" << std::endl;
  mapper
    .registerField("ploum", &Tata::ploum)
    .registerField("view", std::function<const std::string_view&(const Tata&)>(&Tata::getView))
    .registerField<uint64_t>("num_set", &Tata::numSet)
    .registerField<std::string>("str_vector", &Tata::strVect)
    ;
  std::cout << "Tata fields registered" << std::endl;
}

struct Toto {
};

template<>
void
anch::json::registerObject([[maybe_unused]] ObjectMapper<Toto>& mapper) {
  throw std::bad_cast();
}

MapperRegistry reg;

void
beforeAll() {
  JSONMapper json(anch::json::DEFAULT_MAPPING_OPTIONS);
  reg.registerMapper("application/json", std::move(json));
  std::optional<JSONMapper> mapper = reg.mapper<JSONMapper>(MediaType::APP_JSON);
  if(!mapper.has_value()) {
    std::cerr << "Unable to register app/json serializer" << std::endl;
    anch::ut::fail("Unable to register app/json serializer");
  }
}

void
testSerializeTata(const std::string& contentType) {
  std::cout << "Enter in testSerializeTata into " << contentType << std::endl;
  std::string VIEW = "VIEW";
  Tata tata;
  tata.ploum = "ploum";
  tata.view = std::string_view(VIEW.data());
  tata.numSet = {1, 2, 3};
  tata.strVect = {"4","5","6"};
  try {
    reg.serialize(contentType, tata, std::cout);
    std::cout << std::endl;
  } catch(const anch::rest::MapperException& e) {
    std::ostringstream oss;
    oss << "Error while serializing: " << e.what();
    anch::ut::fail(oss.str());
  }
}

void
testSerializeToto(const std::string& contentType) {
  std::cout << "Enter in testSerializeToto into " << contentType << std::endl;
  Toto toto;
  try {
    reg.serialize(contentType, toto, std::cout);
    std::cout << std::endl;
  } catch(const anch::rest::MapperException& e) {
    std::ostringstream oss;
    oss << "Error while serializing: " << e.what();
    anch::ut::fail(oss.str());
  } catch(std::bad_cast&) {
    std::ostringstream oss;
    oss << "Error while serializing: " << contentType << " which is not supported for Toto";
    anch::ut::fail(oss.str());
  }

}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH router mapper registry tests")
    .description("Test AnCH router mapper registry library")
    .initialize(beforeAll)
    .add("test_ser_tata_txt", std::bind(testSerializeTata, "text/plain"))
    .add("test_ser_tata_json", std::bind(testSerializeTata, "application/json"))
    .add("test_ser_toto_txt", std::bind(testSerializeToto, "text/plain"))
    .add("test_ser_toto_json", std::bind(testSerializeToto, "application/json"))
    ;
}
