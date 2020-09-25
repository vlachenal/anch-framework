#include <iostream>
#include <sstream>

#include "json/json.hpp"


using anch::json::JSONMapper;
using anch::json::JSONFactory;


struct Tata {
  std::string ploum;
  std::string_view view;
};

struct Toto {
  std::string plop;
  std::optional<std::string> plip;
  uint64_t plap;
  bool plup;
  Tata tata;
  float plep;
  double plyp;
  long double lplyp;
  std::optional<std::string> empty;
  std::string* ptr;
  std::string* null;
  std::string getClass() const {
    return "Toto";
  }
};

template<>
void
anch::json::registerFields(JSONMapper<Toto>& mapper) {
  std::cout << "Register Toto fields" << std::endl;
  mapper
    .registerField<>("plop", &Toto::plop)
    .registerField<std::optional<std::string>, std::string>("plip", &Toto::plip)
    .registerField<>("plap", &Toto::plap)
    .registerField<>("plup", &Toto::plup)
    .registerField<>("tata", &Toto::tata)
    .registerField<>("plep", &Toto::plep)
    .registerField<>("plyp", &Toto::plyp)
    .registerField<>("lplyp", &Toto::lplyp)
    .registerField<std::optional<std::string>, std::string>("invisible", &Toto::empty)
    .registerField<>("ptr", &Toto::ptr)
    .registerField<>("null", &Toto::null)
    .registerField<>("class", std::function<std::string(const Toto&)>(&Toto::getClass))
    ;
  std::cout << "Toto fields registered" << std::endl;
}

template<>
void
anch::json::registerFields(JSONMapper<Tata>& mapper) {
  std::cout << "Register Tata fields" << std::endl;
  mapper
    .registerField<>("ploum", &Tata::ploum)
    .registerField<>("view", &Tata::view)
    ;
  std::cout << "Tata fields registered" << std::endl;
}

int
main(void) {
  std::cout << "Enter in serialization tests" << std::endl;
  std::string VIEW = "VIEW";
  Toto toto;
  toto.plop = "plop";
  toto.plip = std::optional<std::string>("plip");
  toto.plap = 42;
  toto.plup = false;
  toto.plep = static_cast<float>(2.2);
  toto.plyp = 3.3;
  toto.lplyp = 4.4;
  toto.empty = std::optional<std::string>();
  toto.ptr = &toto.plop;
  toto.null = NULL;
  Tata tata;
  tata.ploum = "ploum";
  tata.view = std::string_view(VIEW.data());
  toto.tata = tata;
  std::ostringstream oss;
  std::cout << "Serialize Toto" << std::endl;
  anch::json::serialize(toto, oss);
  std::cout << "Serialized toto: " << oss.str() << std::endl;
  std::cout << "Serialize Toto as string" << std::endl;
  std::string res = anch::json::serialize(toto);
  std::cout << "Serialized toto as string: " << res << std::endl;
  /*JSONParser parser;
  std::string json("{}");
  std::istringstream iss(json);
  parser.parse<int>(iss);*/
  std::cout << "Exit serialization tests" << std::endl;
  return 0;
}
