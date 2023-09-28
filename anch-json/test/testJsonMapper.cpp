#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>

#include "json/json.hpp"


using anch::json::ObjectMapper;
using anch::json::Factory;
using anch::json::MappingError;
using anch::json::ErrorCode;
using anch::json::JSONMapper;


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

const std::string PLOP_TOTO("Toto");

struct Toto {
  std::string plop;
  std::optional<std::string> plip;
  uint64_t plap;
  bool plup;
  Tata tata;
  float plep;
  double plyp;
  long double lplyp;
  Toto* self;
  std::optional<std::string> empty;
  std::string* ptr;
  std::string* null;
  std::string getClass() const {
    return "Toto";
  }
};

template<>
void
anch::json::registerObject(ObjectMapper<Toto>& mapper) {
  std::cout << "Register Toto fields" << std::endl;
  mapper
    .registerField("plop", &Toto::plop)
    .registerField<std::string>("plip", &Toto::plip)
    .registerField("plap", &Toto::plap)
    .registerField("plup", &Toto::plup)
    .registerField("tata", &Toto::tata)
    .registerField("plep", &Toto::plep)
    .registerField("plyp", &Toto::plyp)
    .registerField("lplyp", &Toto::lplyp)
    .registerField("self", &Toto::self)
    .registerField<std::string>("invisible", &Toto::empty)
    .registerField("ptr", &Toto::ptr)
    .registerField("null", &Toto::null)
    .registerField("class", std::function<std::string(const Toto&)>(&Toto::getClass))
    ;
  std::cout << "Toto fields registered" << std::endl;
}

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

struct Test {
  std::string _id;
  std::string _value;
  std::vector<int32_t> _nums;
  inline const std::string& getValue() const {
    return _value;
  }
  inline void setValue(const std::string& value) {
    _value = value;
  }
};

template<>
void
anch::json::registerObject(ObjectMapper<Test>& mapper) {
  mapper
    .registerField("id", &Test::_id)
    //.registerField("value", std::function<std::string(const Test&)>(&Test::getValue))
    .registerField("value", std::function<std::string(const Test&)>(&Test::getValue), std::function<void(Test&, const std::string&)>(&Test::setValue))
    //.registerField("value", std::function<void(Test&, const std::string&)>(&Test::setValue))
    .registerField<int32_t>("nums", &Test::_nums)
    ;
}

namespace plop {
  struct json_proxy {
    explicit json_proxy(std::ostream & os):os(os){}

    template<typename T>
    friend std::ostream & operator<<(const json_proxy& q,
				     const T& rhs) {
      anch::json::serialize(rhs, q.os);
      return q.os;
    }

  private:
    std::ostream & os;
  };

  struct json_creator { } jsonify;
  json_proxy operator<<(std::ostream& os, json_creator) {
    return json_proxy(os);
  }
}

int
main(void) {
  std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();//, end;
  //std::chrono::microseconds duration;
  std::cout << "Enter in serialization tests" << std::endl;
  {
    anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
    std::string VIEW = "VIEW";
    Toto self;
    self.plop = "self";
    self.plip = std::optional<std::string>("self_plip");
    self.plap = 24;
    self.plup = true;
    self.plep = static_cast<float>(5.5);
    self.plyp = 6.6;
    self.lplyp = 7.7;
    self.empty = std::optional<std::string>();
    self.ptr = &self.plop;
    self.null = NULL;
    self.self = NULL;
    Toto toto;
    toto.plop = "plop";
    toto.plip = std::optional<std::string>("plip");
    toto.plap = 42;
    toto.plup = false;
    toto.plep = 2.2f;
    toto.plyp = 3.3;
    toto.lplyp = 4.4;
    toto.empty = std::optional<std::string>();
    toto.ptr = &toto.plop;
    toto.null = NULL;
    toto.self = &self;
    Tata tata;
    tata.ploum = "ploum";
    tata.view = std::string_view(VIEW.data());
    tata.numSet = {1, 2, 3};
    tata.strVect = {"4","5","6"};
    toto.tata = tata;
    std::ostringstream oss;
    std::cout << "Serialize Toto" << std::endl;
    mapper.serialize(toto, oss);
    std::cout << "Serialized toto: " << oss.str() << std::endl;
    std::cout << "Serialize Toto as string" << std::endl;
    std::string res = mapper.serialize(toto);
    std::cout << "Serialized toto as string: " << res << std::endl;
  }
  {
    anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
    Test test = {
      ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
      ._value = "this is a value",
      ._nums = {1,2,3,4}
    };
    std::ostringstream oss;
    mapper.serialize(test, oss);
    std::cout << "Serialized test: " << oss.str() << std::endl;
    std::string res = mapper.serialize(test);
    std::cout << "Serialized test as string: " << res << std::endl;
    //std::cout << "Serialized test iomanip: " << anch::json::json_format << test << std::endl;
  }
  {
    anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
    Test test = {
      ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
      ._value = "this is a value",
      ._nums = {1,2,3,4}
    };
    std::vector<Test> tests = {test, {
	._id = "44666aab-0b63-47a1-80bb-ae84bc844289",
	._value = "this is another value",
	._nums = {5,6,7,8}
      }
    };
    //std::ostringstream oss;
    std::cout << "Serialized test array: ";
    mapper.serialize(tests, std::cout);
    std::cout << std::endl;
    std::string res = mapper.serialize(tests);
    std::cout << "Serialized test array as string: " << res << std::endl;
    //std::cout << "Serialized test iomanip: " << anch::json::json_format << test << std::endl;
  }
  {
    anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
    Test test;
    std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"nums\":[1,2,3,4]}";
    std::cout << "Deserialize " << json << std::endl;
    std::istringstream iss(json);
    try {
      mapper.deserialize(test, iss);
      //Factory<Test>::getInstance().deserialize(test, iss);
    } catch(const MappingError& error) {
      std::cerr << "Fail with " << error.what() << std::endl;
      return 1;
    }
  }
  {
    anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
    Test test;
    std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":1234,\"nums\":[1,2,3,4]}";
    std::cout << "Deserialize with unknown field and default mapper " << json << std::endl;
    std::istringstream iss(json);
    try {
      mapper.deserialize(test, iss);
      std::cerr << "This test should have failed" << std::endl;
      return 1;
      //Factory<Test>::getInstance().deserialize(test, iss);
    } catch(const MappingError& error) {
      if(error.getErrorCode() == ErrorCode::UNEXPECTED_FIELD) {
	std::cout << "Fail OK with error " << error.what() << std::endl;
      } else {
	std::cerr << "Fail with " << error.what() << std::endl;
	return 1;
      }
    }
  }
  {
    anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
    Test test;
    std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":{\"tot\":{}},\"nums\":[1,2,3,4]}";
    std::cout << "Deserialize with unknown object and custom mapper " << json << std::endl;
    std::istringstream iss(json);
    try {
      mapper.deserialize(test, iss);
    } catch(const MappingError& error) {
      std::cerr << "Fail with " << error.what() << std::endl;
      return 1;
    }
  }
  {
    anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
    Test test;
    std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":\"toto\",\"nums\":[1,2,3,4]}";
    std::cout << "Deserialize with unknown string and custom mapper " << json << std::endl;
    std::istringstream iss(json);
    try {
      mapper.deserialize(test, iss);
    } catch(const MappingError& error) {
      std::cerr << "Fail with " << error.what() << std::endl;
      return 1;
    }
  }
  {
    anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
    Test test;
    std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":1234,\"nums\":[1,2,3,4]}";
    std::cout << "Deserialize with unknown integer and custom mapper " << json << std::endl;
    std::istringstream iss(json);
    try {
      mapper.deserialize(test, iss);
    } catch(const MappingError& error) {
      std::cerr << "Fail with " << error.what() << std::endl;
      return 1;
    }
    std::cout << "OK" << std::endl;
  }
  {
    anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
    Test test;
    std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":[],\"nums\":[1,2,3,4]}";
    std::cout << "Deserialize with unknown array and custom mapper " << json << std::endl;
    std::istringstream iss(json);
    try {
      mapper.deserialize(test, iss);
    } catch(const MappingError& error) {
      std::cerr << "Fail with " << error.what() << std::endl;
      return 1;
    }
    std::cout << "OK" << std::endl;
  }
  {
    anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
    Test test;
    std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":[1],\"nums\":[1,2,3,4]}";
    std::cout << "Deserialize with unknown array and with raw values custom mapper " << json << std::endl;
    std::istringstream iss(json);
    try {
      mapper.deserialize(test, iss);
    } catch(const MappingError& error) {
      std::cerr << "Fail with " << error.what() << std::endl;
      return 1;
    }
    std::cout << "OK" << std::endl;
  }
  {
    anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
    Test test;
    std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":[1,{},\"plop\"],\"nums\":[1,2,3,4]}";
    std::cout << "Deserialize with unknown array and with values custom mapper " << json << std::endl;
    std::istringstream iss(json);
    try {
      mapper.deserialize(test, iss);
    } catch(const MappingError& error) {
      std::cerr << "Fail with " << error.what() << std::endl;
      return 1;
    }
    std::cout << "OK" << std::endl;
  }
  {
    anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
    std::string json = "{\"plop\":\"plop\",\"plip\":\"plip\",\"plap\":42,\"plup\":false,\"tata\":{\"ploum\":\"ploum\",\"num_set\":[1,2,3],\"str_vector\":[\"4\",\"5\",\"6\"]},\"plep\":2.2,\"plyp\":3.3,\"lplyp\":4.4,\"self\":{\"plop\":\"self\",\"plip\":\"self_plip\",\"plap\":24,\"plup\":true,\"tata\":{\"ploum\":\"\",\"num_set\":[],\"str_vector\":[]},\"plep\":5.5,\"plyp\":6.6,\"lplyp\":7.7,\"ptr\":\"self\"},\"ptr\":\"plop\"}";
    std::cout << "Deserialize " << json << std::endl;
    std::istringstream iss(json);
    Toto toto;
    try {
      mapper.deserialize(toto, iss);
    } catch(const std::bad_cast& e) {
      std::cerr << "Bad cast " << e.what() << std::endl;
      return 1;
    } catch(const MappingError& error) {
      std::cerr << "Fail with " << error.what() << std::endl;
      return 1;
    }
  }
  {
    anch::json::JSONMapper mapper({.deserialize_max_discard_char = 128});
    std::ifstream iss("toto.json");
    std::cout << "Deserialize toto.json" << std::endl;
    //Toto toto;
    try {
      Toto toto = mapper.deserialize<Toto>(iss);
    } catch(const std::bad_cast& e) {
      std::cerr << "Bad cast " << e.what() << std::endl;
      return 1;
    } catch(const MappingError& error) {
      std::cerr << "Fail with " << error.what() << std::endl;
      return 1;
    }
  }
  {
    anch::json::JSONMapper mapper({.deserialize_max_discard_char = 128});
    std::ifstream iss("totos.json");
    std::cout << "Deserialize totos.json" << std::endl;
    //Toto toto;
    try {
      std::vector<Test> tests;
      mapper.deserialize<Test>(tests, iss);
    } catch(const std::bad_cast& e) {
      std::cerr << "Bad cast " << e.what() << std::endl;
      return 1;
    } catch(const MappingError& error) {
      std::cerr << "Fail with " << error.what() << std::endl;
      return 1;
    }
  }
  {
    Test test = {
      ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
      ._value = "this is a value",
      ._nums = {1,2,3,4}
    };
    std::cout << "As iomanip: " << plop::jsonify << test << std::endl;
  }
  std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
  std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  std::cout << "Exit serialization tests in " << duration.count() << " µs" << std::endl;
  return 0;
}
