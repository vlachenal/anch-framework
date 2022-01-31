#include "rest/mapperRegistry.hpp"

#include <iostream>

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

int
main(void) {
  MapperRegistry reg;
  JSONMapper json(anch::json::DEFAULT_MAPPING_OPTIONS);
  reg.registerMapper("application/json", std::move(json));
  std::optional<JSONMapper> mapper = reg.mapper<JSONMapper>(MediaType::APP_JSON);
  if(!mapper.has_value()) {
    std::cout << "KO" << std::endl;
    return 1;
  }

  std::string VIEW = "VIEW";
  Tata tata;
  tata.ploum = "ploum";
  tata.view = std::string_view(VIEW.data());
  tata.numSet = {1, 2, 3};
  tata.strVect = {"4","5","6"};
  reg.serialize(MediaType::APP_JSON, tata, std::cout);
  std::cout << std::endl;

  try {
    reg.serialize("text/plain", tata, std::cout);
    std::cout << std::endl;
  } catch(const anch::rest::MapperException& e) {
    std::cout << "Error while serializing: " << e.what() << std::endl;

  }

  Toto toto;
  try {
    reg.serialize("text/plain", toto, std::cout);
    std::cout << std::endl;
  } catch(const anch::rest::MapperException& e) {
    std::cout << "Error while serializing: " << e.what() << std::endl;
  }

  try {
    reg.serialize("application/json", toto, std::cout);
    std::cout << std::endl;
  } catch(std::bad_cast&) {
    std::cout << "Error while serializing application/json which is not supported for Toto" << std::endl;
  }

  std::cout << "OK" << std::endl;
  return 0;
}
