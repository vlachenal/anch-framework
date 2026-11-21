/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2020 Vincent Lachenal

  This file is part of ANCH Framework.

  ANCH Framework is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ANCH Framework is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>
#include <limits>
#include <array>
#include <regex>
#include <string_view>
#include <set>
#include <map>
#include <stdexcept>
#include <initializer_list>
#include <utility>

#include "json/json.hpp"
#include "val/validation.hpp"
#include "mapping.hpp"

#ifdef ANCH_UUID
#include "uuid.hpp"
#endif

#ifdef ANCH_DATE
#include "date/date.hpp"
#endif

#include "ut/assert.hpp"
#include "ut/unit.hpp"


using anch::json::ObjectMapper;
using anch::json::Factory;
using anch::json::MappingError;
using anch::json::ErrorCode;
using anch::json::JSONMapper;
#ifdef ANCH_UUID
using anch::UUID;
#endif


template<typename T>
std::ostream&
operator<<(std::ostream& os, const std::vector<T>& array) {
  os << '[';
  bool first = true;
  for(const T& item: array) {
    if(!first) {
      os << ", " << item;
    } else {
      os << item;
      first = false;
    }
  }
  os << ']';
  return os;
}

template<typename T>
std::ostream&
operator<<(std::ostream& os, const std::list<T>& array) {
  os << '[';
  bool first = true;
  for(const T& item: array) {
    if(!first) {
      os << ", " << item;
    } else {
      os << item;
      first = false;
    }
  }
  os << ']';
  return os;
}

template<typename T>
std::ostream&
operator<<(std::ostream& os, const std::set<T>& array) {
  os << '[';
  bool first = true;
  for(const T& item: array) {
    if(!first) {
      os << ", " << item;
    } else {
      os << item;
      first = false;
    }
  }
  os << ']';
  return os;
}

template<typename T>
std::ostream&
operator<<(std::ostream& os, const std::optional<T>& opt) {
  if(!opt.has_value()) {
    os << "std::optional->empty";
  } else {
    os << opt.value();
  }
  return os;
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
  bool operator==(const Tata& tata) const {
    return  anch::ut::equals("Tata::ploum", ploum, tata.ploum)
      && anch::ut::equals("Tata::selfList", selfList, tata.selfList)
      && anch::ut::equals("Tata::strVect", strVect, tata.strVect)
      && anch::ut::equals("Tata::numSet", numSet, tata.numSet)
      ;
  }
};

// anch-json mapper is a mapper of the common description (anch-utils/mapping.hpp)
static_assert(anch::ObjectMapper<anch::json::ObjectMapper<Tata>, Tata>);

std::ostream&
operator<<(std::ostream& os, const Tata& tata) {
    os << "{ploum=" << tata.ploum
       << ",view=" << tata.view
       << ",selfList=" << tata.selfList
       << ",strVect=" << tata.strVect
       << ",numSet=" << tata.numSet << '}'
      ;
    return os;
}


struct Toto {
  std::string plop;
  std::optional<std::string> plip;
  uint64_t plap;
  bool plup;
  Tata tata;
  float plep;
  double plyp;
  //long double lplyp;
  std::shared_ptr<Toto> self = NULL;
  std::optional<std::string> empty;
  std::shared_ptr<std::string> ptr;
  std::string* null = NULL;
#ifdef ANCH_UUID
  UUID uuid;
#endif
  std::string getClass() const {
    return "Toto";
  }
  bool operator==(const Toto& toto) const {
    return anch::ut::equals("Toto::plop", plop, toto.plop)
      && anch::ut::equals("Toto::plip", plip, toto.plip)
      && anch::ut::equals("Toto::plap", plap, toto.plap)
      && anch::ut::equals("Toto::plup", plup, toto.plup)
      && anch::ut::equals("Toto::plep", plep, toto.plep)
      && anch::ut::equals("Toto::plyp", plyp, toto.plyp)
      //&& anch::ut::equals("Toto::lplyp", lplyp, toto.lplyp)
      && anch::ut::equals("Toto::self", self, toto.self)
      && anch::ut::equals("Toto::ptr", ptr, toto.ptr)
      && anch::ut::equalsPtr("Toto::null", null, toto.null)
      && anch::ut::equals("Toto::empty", empty, toto.empty)
      && anch::ut::equals("Toto::tata", tata, toto.tata)
#ifdef ANCH_UUID
      && anch::ut::equals("Toto::uuid", uuid, toto.uuid)
#endif
      ;
  }
};

std::ostream&
operator<<(std::ostream& os, const Toto& toto) {
  std::ostringstream selfOss;
  if(toto.self == NULL) {
    selfOss << "NULL";
  } else {
    selfOss << *toto.self;
  }
  os << "{plop=" << toto.plop
     << ",plip=" << toto.plip
     << ",plap=" << toto.plap
     << ",plup=" << toto.plup
     << ",plep=" << toto.plep
     << ",plyp=" << toto.plyp
    //<< ",lplyp=" << toto.lplyp
     << ",self=" << selfOss.str()
     << ",ptr=" << (toto.ptr != NULL ? *toto.ptr : std::string("NULL"))
     << ",null=" << (toto.null != NULL ? *toto.null : std::string("NULL"))
     << ",empty=" << toto.empty
     << ",tata=" << toto.tata
#ifdef ANCH_UUID
     << ",uuid=" << toto.uuid
#endif
     << '}'
    ;
  return os;
}

template<>
struct anch::Mapping<Toto> {
  static void
  registerObject(anch::ObjectMapper<Toto> auto& mapper) {
    std::cout << "Register Toto fields" << std::endl;
    mapper
      .registerField("plop", &Toto::plop)
      .registerField("plip", &Toto::plip)
      .registerField("plap", &Toto::plap)
      .registerField("plup", &Toto::plup)
      .registerField("tata", &Toto::tata)
      .registerField("plep", &Toto::plep)
      .registerField("plyp", &Toto::plyp)
      //.registerField("lplyp", &Toto::lplyp)
      .registerField("self", &Toto::self)
      .registerField("invisible", &Toto::empty)
      .registerField("ptr", &Toto::ptr)
      .registerField("null", &Toto::null)
      .registerField("class", std::function<std::string(const Toto&)>(&Toto::getClass))
#ifdef ANCH_UUID
      .registerField("uuid", &Toto::uuid)
#endif
      ;
    std::cout << "Toto fields registered" << std::endl;
  }
};

template<>
struct anch::Mapping<Tata> {
  static void
  registerObject(anch::ObjectMapper<Tata> auto& mapper) {
    std::cout << "Register Tata fields" << std::endl;
    mapper
      .registerField("ploum", &Tata::ploum)
      .registerField("view", std::function<const std::string_view&(const Tata&)>(&Tata::getView))
      .registerField("num_set", &Tata::numSet)
      .registerField("str_vector", &Tata::strVect)
      ;
    std::cout << "Tata fields registered" << std::endl;
  }
};

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
  bool operator==(const Test& test) const {
    return anch::ut::equals("Test::id", _id, test._id)
      && anch::ut::equals("Test::value", _value, test._value)
      && anch::ut::equals("Test::nums", _nums, test._nums)
      ;
  }
};

template<>
struct anch::Mapping<Test> {
  static void
  registerObject(anch::ObjectMapper<Test> auto& mapper) {
    mapper
      .registerField("id", &Test::_id)
      .registerField("value", std::function<std::string(const Test&)>(&Test::getValue), std::function<void(Test&, const std::string&)>(&Test::setValue))
      .registerField("nums", &Test::_nums)
      ;
  }
};

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

Toto toto;
//Toto* self = NULL;
std::string VIEW = "VIEW";
#ifdef ANCH_UUID
const std::string totoRes = "{\"plop\":\"plop\",\"plip\":\"plip\",\"plap\":42,\"plup\":false,\"tata\":{\"ploum\":\"ploum\",\"view\":\"VIEW\",\"num_set\":[1,2,3],\"str_vector\":[\"4\",\"5\",\"6\"]},\"plep\":2.2,\"plyp\":3.3,\"self\":{\"plop\":\"self\",\"plip\":\"self_plip\",\"plap\":24,\"plup\":true,\"tata\":{\"ploum\":\"\",\"view\":\"\",\"num_set\":[],\"str_vector\":[]},\"plep\":5.5,\"plyp\":6.6,\"ptr\":\"self\",\"class\":\"Toto\",\"uuid\":\"00000000-0000-0000-0000-000000000000\"},\"ptr\":\"plop\",\"class\":\"Toto\",\"uuid\":\"b4c7f7e7-adaf-41d6-8ff9-150f6b867071\"}";
#else
const std::string totoRes = "{\"plop\":\"plop\",\"plip\":\"plip\",\"plap\":42,\"plup\":false,\"tata\":{\"ploum\":\"ploum\",\"view\":\"VIEW\",\"num_set\":[1,2,3],\"str_vector\":[\"4\",\"5\",\"6\"]},\"plep\":2.2,\"plyp\":3.3,\"self\":{\"plop\":\"self\",\"plip\":\"self_plip\",\"plap\":24,\"plup\":true,\"tata\":{\"ploum\":\"\",\"view\":\"\",\"num_set\":[],\"str_vector\":[]},\"plep\":5.5,\"plyp\":6.6,\"ptr\":\"self\",\"class\":\"Toto\"},\"ptr\":\"plop\",\"class\":\"Toto\"}";
#endif

Test res = {
  ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
  ._value = "",
  ._nums = {1,2,3,4}
};

void
beforeAll() {
  std::cout << "Enter in JSON mapper unit tests" << std::endl << std::endl;
  {
    std::shared_ptr<Toto> self = std::make_shared<Toto>();
    self->plop = "self";
    self->plip = std::optional<std::string>("self_plip");
    self->plap = 24;
    self->plup = true;
    self->plep = static_cast<float>(5.5);
    self->plyp = 6.6;
    //self->lplyp = 7.7;
    self->empty = std::optional<std::string>();
    self->self = NULL;
    self->ptr = std::make_shared<std::string>(self->plop);
    self->null = NULL;

    toto.plop = "plop";
    toto.plip = std::optional<std::string>("plip");
    toto.plap = 42;
    toto.plup = false;
    toto.plep = 2.2f;
    toto.plyp = 3.3;
    //toto.lplyp = 4.4;
    toto.empty = std::optional<std::string>();
    toto.ptr = std::make_shared<std::string>(toto.plop);
    toto.null = NULL;
    toto.self = self;
    Tata tata;
    tata.ploum = "ploum";
    tata.view = std::string_view(VIEW.data());
    tata.numSet = {1, 2, 3};
    tata.strVect = {"4","5","6"};
    toto.tata = tata;
#ifdef ANCH_UUID
    toto.uuid = anch::UUID("b4c7f7e7-adaf-41d6-8ff9-150f6b867071");
#endif
  }
}

void
testFullSerDefaultOptions() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  std::ostringstream oss;
  std::cout << "Serialize Toto" << std::endl;
  mapper.serialize(toto, oss);
  std::string res1 = oss.str();
  std::cout << "Serialized toto: " << res1 << std::endl;
  std::cout << "Serialize Toto as string" << std::endl;
  std::string res2 = mapper.serialize(toto);
  std::cout << "Serialized toto as string: " << res2 << std::endl;
  anch::ut::assertThat(res1 == res2, "Serialization results should be equals");
  anch::ut::assertThat(totoRes == res1, "Result should be equals to: {}", totoRes);
}

void
testDeserList() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);

    std::ostringstream numsTestOss;
    numsTestOss << test._nums;
    std::ostringstream numsResOss;
    numsResOss << res._nums;
    anch::ut::assertThat(test == res, "Test has been unexpecting deserialized: id {} / {} instead of {} / {}", test._id, numsTestOss.str(), res._id, numsResOss.str());

  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testDeserUnknownFieldStructFail() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":1234,\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize with unknown field and default mapper " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);
    std::ostringstream oss;
    oss << "This test should have failed" << std::endl;
    anch::ut::fail(oss.str());
  } catch(const MappingError& error) {
    if(error.getErrorCode() == ErrorCode::UNEXPECTED_FIELD) {
      std::cout << "Fail OK with error " << error.what() << std::endl;
    } else {
      std::ostringstream oss;
      oss << "Fail with " << error.what() << std::endl;
      anch::ut::fail(oss.str());
    }
  }
}

void
testDeserUnknownFieldStructOK() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":{\"tot\":{}},\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize with unknown object and custom mapper " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);

    std::ostringstream numsTestOss;
    numsTestOss << test._nums;
    std::ostringstream numsResOss;
    numsResOss << res._nums;
    anch::ut::assertThat(test == res, "Test has been unexpecting deserialized: id {} / {} instead of {} / {}", test._id, numsTestOss.str(), res._id, numsResOss.str());

  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testDeserUnknownFieldOK() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":\"toto\",\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize with unknown string and custom mapper " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);

    std::ostringstream numsTestOss;
    numsTestOss << test._nums;
    std::ostringstream numsResOss;
    numsResOss << res._nums;
    anch::ut::assertThat(test == res, "Test has been unexpecting deserialized: id {} / {} instead of {} / {}", test._id, numsTestOss.str(), res._id, numsResOss.str());

  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testDeserUnknownFieldIntOK() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":1234,\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize with unknown integer and custom mapper " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);

    std::ostringstream numsTestOss;
    numsTestOss << test._nums;
    std::ostringstream numsResOss;
    numsResOss << res._nums;
    anch::ut::assertThat(test == res, "Test has been unexpecting deserialized: id {} / {} instead of {} / {}", test._id, numsTestOss.str(), res._id, numsResOss.str());

  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
  std::cout << "OK" << std::endl;
}

void
testDeserUnknownFieldEmptyColOK() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":[],\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize with unknown array and custom mapper " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);

    std::ostringstream numsTestOss;
    numsTestOss << test._nums;
    std::ostringstream numsResOss;
    numsResOss << res._nums;
    anch::ut::assertThat(test == res, "Test has been unexpecting deserialized: id {} / {} instead of {} / {}", test._id, numsTestOss.str(), res._id, numsResOss.str());

  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
  std::cout << "OK" << std::endl;
}

void
testDeserUnknownFielIntColOK() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":[1],\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize with unknown array and with raw values custom mapper " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);

    std::ostringstream numsTestOss;
    numsTestOss << test._nums;
    std::ostringstream numsResOss;
    numsResOss << res._nums;
    anch::ut::assertThat(test == res, "Test has been unexpecting deserialized: id {} / {} instead of {} / {}", test._id, numsTestOss.str(), res._id, numsResOss.str());

  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
  std::cout << "OK" << std::endl;
}

void
testDeserUnknownFielWTFColOK() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":[1,{},\"plop\"],\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize with unknown array and with values custom mapper " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);

    std::ostringstream numsTestOss;
    numsTestOss << test._nums;
    std::ostringstream numsResOss;
    numsResOss << res._nums;
    anch::ut::assertThat(test == res, "Test has been unexpecting deserialized: id {} / {} instead of {} / {}", test._id, numsTestOss.str(), res._id, numsResOss.str());

  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
  std::cout << "OK" << std::endl;
}

void
testFullDeserDefaultOptions() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
#ifdef ANCH_UUID
  std::string json = "{\"plop\":\"plop\",\"plip\":\"plip\",\"plap\":42,\"plup\":false,\"tata\":{\"ploum\":\"ploum\",\"num_set\":[1,2,3],\"str_vector\":[\"4\",\"5\",\"6\"]},\"plep\":2.2,\"plyp\":3.3,\"self\":{\"plop\":\"self\",\"plip\":\"self_plip\",\"plap\":24,\"plup\":true,\"tata\":{\"ploum\":\"\",\"num_set\":[],\"str_vector\":[]},\"plep\":5.5,\"plyp\":6.6,\"ptr\":\"self\"},\"ptr\":\"plop\",\"uuid\":\"b4c7f7e7-adaf-41d6-8ff9-150f6b867071\"}";
#else
  std::string json = "{\"plop\":\"plop\",\"plip\":\"plip\",\"plap\":42,\"plup\":false,\"tata\":{\"ploum\":\"ploum\",\"num_set\":[1,2,3],\"str_vector\":[\"4\",\"5\",\"6\"]},\"plep\":2.2,\"plyp\":3.3,\"self\":{\"plop\":\"self\",\"plip\":\"self_plip\",\"plap\":24,\"plup\":true,\"tata\":{\"ploum\":\"\",\"num_set\":[],\"str_vector\":[]},\"plep\":5.5,\"plyp\":6.6,\"ptr\":\"self\"},\"ptr\":\"plop\"}";
#endif
  Toto expected = toto;

  std::cout << "Deserialize " << json << std::endl;
  std::istringstream iss(json);
  Toto totoDeser;
  try {
    anch::json::Factory<Toto>::getInstance();
    std::cout << "Instance OK" << std::endl;
    mapper.deserialize(totoDeser, iss);

    std::cout << "Expected: " << expected << std::endl;
    std::cout << "Result:   " << totoDeser << std::endl;
    anch::ut::assertThat(expected == totoDeser, "NOT EQUALS");

  } catch(const std::bad_cast& e) {
    std::ostringstream oss;
    oss << "Bad cast " << e.what();
    anch::ut::fail(oss.str());
  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testFulDeserDiscard128() {
  anch::json::JSONMapper mapper({.deserialize_max_discard_char = 128});
#ifdef ANCH_UUID
  std::ifstream iss("toto_uuid.json");
#else
  std::ifstream iss("toto.json");
#endif
  std::cout << "Deserialize toto.json" << std::endl;
  try {
    Toto deserToto = mapper.deserialize<Toto>(iss);
    std::cout << deserToto << std::endl;
    anch::ut::assertThat(toto == deserToto, "NOT EQUALS");
  } catch(const std::bad_cast& e) {
    std::ostringstream oss;
    oss << "Bad cast  " << e.what();
    anch::ut::fail(oss.str());
  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testFulDeserDiscard128Col() {
  anch::json::JSONMapper mapper({.deserialize_max_discard_char = 128});
  std::ifstream iss("totos.json");
  std::vector<Test> expected = {
    {
      ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
      ._value = "this is a value",
      ._nums = {1,2,3,4}
    },
    {
      ._id = "44666aab-0b63-47a1-80bb-ae84bc844289",
      ._value = "this is another value",
      ._nums = {5,6,7,8}
    }
  };
  std::cout << "Deserialize totos.json" << std::endl;
  try {
    std::vector<Test> tests;
    mapper.deserialize(tests, iss);
    anch::ut::assertThat(expected == tests, "NOT EQUALS");
  } catch(const std::bad_cast& e) {
    std::ostringstream oss;
    oss << "Bad cast " << e.what();
    anch::ut::fail(oss.str());
  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testFulDeserDiscard128ColBuf2() {
  anch::json::JSONMapper mapper({.deserialize_max_discard_char = 128, .buffer_size = 2});
  std::ifstream iss("totos.json");
  std::vector<Test> expected = {
    {
      ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
      ._value = "this is a value",
      ._nums = {1,2,3,4}
    },
    {
      ._id = "44666aab-0b63-47a1-80bb-ae84bc844289",
      ._value = "this is another value",
      ._nums = {5,6,7,8}
    }
  };
  std::cout << "Deserialize totos.json" << std::endl;
  try {
    std::vector<Test> tests;
    mapper.deserialize(tests, iss);
    anch::ut::assertThat(expected == tests, "NOT EQUALS");
  } catch(const std::bad_cast& e) {
    std::ostringstream oss;
    oss << "Bad cast " << e.what();
    anch::ut::fail(oss.str());
  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testDeserFlux() {
  anch::json::JSONMapper mapper({.deserialize_max_discard_char = 128});
  std::ifstream iss("totos.json");
  std::vector<Test> expected = {
    {
      ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
      ._value = "this is a value",
      ._nums = {1,2,3,4}
    },
    {
      ._id = "44666aab-0b63-47a1-80bb-ae84bc844289",
      ._value = "this is another value",
      ._nums = {5,6,7,8}
    }
  };
  std::cout << "Deserialize totos.json" << std::endl;
  try {
    std::vector<Test> tests;
    anch::Flux<Test> flux;
    flux.setConsumer([&tests](const Test& test) {
      tests.push_back(test);
    });
    flux.ready();
    mapper.deserialize(flux, iss);
    anch::ut::assertThat(expected == tests, "NOT EQUALS");
  } catch(const std::bad_cast& e) {
    std::ostringstream oss;
    oss << "Bad cast " << e.what();
    anch::ut::fail(oss.str());
  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testSerFlux() {
  std::vector<Test> tests = {
    {
      ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
      ._value = "this is a value",
      ._nums = {1,2,3,4}
    },
    {
      ._id = "44666aab-0b63-47a1-80bb-ae84bc844289",
      ._value = "this is another value",
      ._nums = {5,6,7,8}
    }
  };
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  std::ostringstream out;
  anch::Flux<Test> flux;
  std::string expected("[{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"value\":\"this is a value\",\"nums\":[1,2,3,4]},{\"id\":\"44666aab-0b63-47a1-80bb-ae84bc844289\",\"value\":\"this is another value\",\"nums\":[5,6,7,8]}]");
  std::thread t([&flux, &tests]() {
    for(auto iter = tests.cbegin() ; iter != tests.cend() ; ++iter) {
      flux.push(*iter);
    }
    flux.finalize();
  });
  try {
    mapper.serialize(flux, out);
  } catch(const MappingError& error) {
    t.join();
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
  t.join(); // producer uses flux until finalize returns
  anch::ut::assertThat(out.str() == expected, "\nResult is:  {}\ninstead of: {}", out.str(), expected);
}

void
testIOManip() {
  Test test = {
    ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
    ._value = "this is a value",
    ._nums = {1,2,3,4}
  };
  std::cout << "As iomanip: " << plop::jsonify << test << std::endl;
}

struct TestMap {
  std::map<std::string,std::string> _strMap;
  std::map<std::string,bool> _boolMap;
  std::map<std::string,int64_t> _intMap;
  std::map<std::string,double> _doubleMap;
  std::map<std::string,Test> _testMap;
};

template<>
struct anch::Mapping<TestMap> {
  static void
  registerObject(anch::ObjectMapper<TestMap> auto& mapper) {
    mapper
      .registerField("strMap", &TestMap::_strMap)
      .registerField("boolMap", &TestMap::_boolMap)
      .registerField("intMap", &TestMap::_intMap)
      .registerField("doubleMap", &TestMap::_doubleMap)
      .registerField("testMap", &TestMap::_testMap)
      ;
  }
};

void
testSerializeMap() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  TestMap test = {
    ._strMap = {
      {"a","b"},
      {"1","2"},
      {"toto","tata"},
      {"plip","plop"}
    },
    ._boolMap = {
      {"a",true},
      {"1",false},
      {"toto",true},
      {"plip",false}
    },
    ._intMap = {
      {"a",123},
      {"1",456},
      {"toto",789},
      {"plip",123456789}
    },
    ._doubleMap = {
      {"a",3.14158},
      {"1",0.01234},
      {"toto",123.456},
      {"plip",789.123}
    },
    ._testMap = {
      {"test1", {
	  ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
	  ._value = "this is a value",
	  ._nums = {1,2,3,4}
	}
      },
      {"test2", {
	  ._id = "toto",
	  ._value = "this is another value",
	  ._nums = {5,6,7,8}
	}
      }
    }
  };
  std::ostringstream oss;
  std::cout << "Serialize TestMap" << std::endl;
  mapper.serialize(test, oss);
  std::cout << "Serialized test: " << oss.str() << std::endl;
  std::cout << "Serialize TestMap as string" << std::endl;
  std::string res = mapper.serialize(test);
  std::cout << "Serialized test as string: " << res << std::endl;
}

void
testDeserializeMap() {
  std::string json("{\"strMap\":{\"1\":\"2\",\"a\":\"b\",\"plip\":\"plop\",\"toto\":\"tata\"},\"boolMap\":{\"1\":false,\"a\":true,\"plip\":false,\"toto\":true},\"intMap\":{\"1\":456,\"a\":123,\"plip\":123456789,\"toto\":789},\"doubleMap\":{\"1\":0.01234,\"a\":3.14158,\"plip\":789.123,\"toto\":123.456},\"testMap\":{\"test1\":{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"value\":\"this is a value\",\"nums\":[1,2,3,4]},\"test2\":{\"id\":\"toto\",\"value\":\"this is another value\",\"nums\":[5,6,7,8]}}}");
  anch::json::JSONMapper mapper({.deserialize_max_discard_char = 128});
  std::cout << "Deserialize TestMap: " << json << std::endl;
  try {
    auto iss = std::istringstream(json);
    TestMap toto = mapper.deserialize<TestMap>(iss);
    std::ostringstream oss;
    mapper.serialize(toto, oss);
    std::cout << "Serialized TestMap:  " << oss.str() << std::endl;
  } catch(const std::bad_cast& e) {
    std::ostringstream oss;
    oss << "Bad cast  " << e.what();
    anch::ut::fail(oss.str());
  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

// Integer range +
struct TestRange {
  int16_t _i16 = 0;
  int32_t _i32 = 0;
  uint16_t _u16 = 0;
  uint32_t _u32 = 0;
};

template<>
struct anch::Mapping<TestRange> {
  static void
  registerObject(anch::ObjectMapper<TestRange> auto& mapper) {
    mapper
      .registerField("i16", &TestRange::_i16)
      .registerField("i32", &TestRange::_i32)
      .registerField("u16", &TestRange::_u16)
      .registerField("u32", &TestRange::_u32)
      ;
  }
};

void
checkRangeOverflow(const std::string& json) {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  std::cout << "Deserialize TestRange: " << json << std::endl;
  try {
    auto iss = std::istringstream(json);
    [[maybe_unused]] TestRange res = mapper.deserialize<TestRange>(iss);
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    if(e.getErrorCode() != ErrorCode::POTENTIAL_OVERFLOW) {
      throw;
    }
    std::cout << e.what() << std::endl;
  }
}

void
testDeserInt16Overflow() {
  checkRangeOverflow("{\"i16\":32768}");
}

void
testDeserInt16Underflow() {
  checkRangeOverflow("{\"i16\":-32769}");
}

void
testDeserInt32Overflow() {
  checkRangeOverflow("{\"i32\":2147483648}");
}

void
testDeserInt32Underflow() {
  checkRangeOverflow("{\"i32\":-2147483649}");
}

void
testDeserUint16Overflow() {
  checkRangeOverflow("{\"u16\":65536}");
}

void
testDeserUint32Overflow() {
  checkRangeOverflow("{\"u32\":4294967296}");
}

void
testDeserRangeLimits() {
  std::string json("{\"i16\":-32768,\"i32\":2147483647,\"u16\":65535,\"u32\":4294967295}");
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  std::cout << "Deserialize TestRange: " << json << std::endl;
  auto iss = std::istringstream(json);
  TestRange res = mapper.deserialize<TestRange>(iss);
  anch::ut::assertThat(res._i16 == std::numeric_limits<int16_t>::min(), "i16 should be {} ; found {}", std::numeric_limits<int16_t>::min(), res._i16);
  anch::ut::assertThat(res._i32 == std::numeric_limits<int32_t>::max(), "i32 should be {} ; found {}", std::numeric_limits<int32_t>::max(), res._i32);
  anch::ut::assertThat(res._u16 == std::numeric_limits<uint16_t>::max(), "u16 should be {} ; found {}", std::numeric_limits<uint16_t>::max(), res._u16);
  anch::ut::assertThat(res._u32 == std::numeric_limits<uint32_t>::max(), "u32 should be {} ; found {}", std::numeric_limits<uint32_t>::max(), res._u32);
}
// Integer range -

// Float range +
struct TestFloat {
  float _f = 0;
};

template<>
struct anch::Mapping<TestFloat> {
  static void
  registerObject(anch::ObjectMapper<TestFloat> auto& mapper) {
    mapper
      .registerField("f", &TestFloat::_f)
      ;
  }
};

void
checkFloatOverflow(const std::string& json) {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  std::cout << "Deserialize TestFloat: " << json << std::endl;
  try {
    auto iss = std::istringstream(json);
    [[maybe_unused]] TestFloat res = mapper.deserialize<TestFloat>(iss);
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    if(e.getErrorCode() != ErrorCode::POTENTIAL_OVERFLOW) {
      throw;
    }
    std::cout << e.what() << std::endl;
  }
}

void
testDeserFloatOverflow() {
  checkFloatOverflow("{\"f\":1e300}");
}

void
testDeserFloatNegativeOverflow() {
  checkFloatOverflow("{\"f\":-1e300}");
}

void
testDeserFloatLimits() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  for(std::string json : {"{\"f\":3.4028234e38}", "{\"f\":-3.4028234e38}", "{\"f\":1e-50}"}) {
    std::cout << "Deserialize TestFloat: " << json << std::endl;
    auto iss = std::istringstream(json);
    TestFloat res = mapper.deserialize<TestFloat>(iss);
    std::cout << "Found " << res._f << std::endl;
  }
}
// Float range -

// Payload size +
void
checkMaxPayloadKo(const std::string& json, std::size_t maxPayload, std::size_t bufferSize) {
  anch::json::JSONMapper mapper({.max_payload = maxPayload, .buffer_size = bufferSize});
  std::cout << "Deserialize TestFloat: " << json << " with max_payload=" << maxPayload << " and buffer_size=" << bufferSize << std::endl;
  try {
    auto iss = std::istringstream(json);
    [[maybe_unused]] TestFloat res = mapper.deserialize<TestFloat>(iss);
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    if(e.getErrorCode() != ErrorCode::PAYLOAD_TOO_LARGE) {
      throw;
    }
    std::cout << e.what() << std::endl;
  }
}

void
testDeserMaxPayloadKo() {
  checkMaxPayloadKo("{\"f\":1.5}", 8, 1024); // payload is bigger than max_payload in initial read
}

void
testDeserMaxPayloadRefillKo() {
  checkMaxPayloadKo("{\"f\":1.5}", 8, 4); // payload is bigger than max_payload while refilling buffer
}

void
testDeserMaxPayloadLimit() {
  std::string json("{\"f\":1.5}");
  for(std::size_t bufferSize : {std::size_t(3), std::size_t(1024)}) {
    anch::json::JSONMapper mapper({.max_payload = json.size(), .buffer_size = bufferSize});
    std::cout << "Deserialize TestFloat: " << json << " with max_payload=" << json.size() << " and buffer_size=" << bufferSize << std::endl;
    auto iss = std::istringstream(json);
    TestFloat res = mapper.deserialize<TestFloat>(iss);
    anch::ut::assertThat(res._f == 1.5f, "f should be 1.5 ; found {}", res._f);
  }
}
// Payload size -

// String escaping +
struct TestStr {
  std::string _s;
};

template<>
struct anch::Mapping<TestStr> {
  static void
  registerObject(anch::ObjectMapper<TestStr> auto& mapper) {
    mapper
      .registerField("s", &TestStr::_s)
      ;
  }
};

void
testSerEscape() {
  TestStr value = {._s = "a\"b\\c/\b\f\n\r\t\x01\x1f caf\xC3\xA9"};
  std::string expected = "{\"s\":\"a\\\"b\\\\c/\\b\\f\\n\\r\\t\\u0001\\u001f caf\xC3\xA9\"}";
  std::string json = anch::json::serialize(value, anch::json::DEFAULT_MAPPING_OPTIONS);
  std::cout << "Serialized: " << json << std::endl;
  anch::ut::assertThat(json == expected, "Expected {} ; found {}", expected, json);
  // Round trip +
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  auto iss = std::istringstream(json);
  TestStr res = mapper.deserialize<TestStr>(iss);
  anch::ut::assertThat(res._s == value._s, "Round trip failed: found '{}'", res._s);
  // Round trip -
}

void
testSerEscapeMapKey() {
  std::map<std::string,std::string> value = {{"k\"1", "v\n"}};
  std::string expected = "{\"k\\\"1\":\"v\\n\"}";
  std::string json = anch::json::serialize(value, anch::json::DEFAULT_MAPPING_OPTIONS);
  std::cout << "Serialized: " << json << std::endl;
  anch::ut::assertThat(json == expected, "Expected {} ; found {}", expected, json);
  // Round trip +
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  auto iss = std::istringstream(json);
  std::map<std::string,std::string> res;
  mapper.deserialize(res, iss);
  anch::ut::assertThat(res == value, "Round trip failed");
  // Round trip -
}
// String escaping -

// Nesting depth +
struct TestNode {
  std::vector<TestNode> _children;
};

template<>
struct anch::Mapping<TestNode> {
  static void
  registerObject(anch::ObjectMapper<TestNode> auto& mapper) {
    mapper
      .registerField("c", &TestNode::_children)
      ;
  }
};

void
checkDepthError(const std::function<void()>& deser) {
  try {
    deser();
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    if(e.getErrorCode() != ErrorCode::POTENTIAL_OVERFLOW) {
      throw;
    }
    std::cout << e.what() << std::endl;
  }
}

void
testDeserMaxDepthUnknown() {
  // 200000 nested arrays in ignored field: stack overflow before max_depth option
  std::string json = "{\"s\":\"a\",\"u\":" + std::string(200000, '[') + std::string(200000, ']') + "}";
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  checkDepthError([&]() {
    auto iss = std::istringstream(json);
    [[maybe_unused]] TestStr res = mapper.deserialize<TestStr>(iss);
  });
}

void
testDeserMaxDepthUnknownLimit() {
  // root object is level 1 ; arrays are levels 2 to 4
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true, .max_depth = 4});
  auto iss = std::istringstream("{\"s\":\"a\",\"u\":[[{}]]}");
  TestStr res = mapper.deserialize<TestStr>(iss);
  anch::ut::assertThat(res._s == "a", "s should be 'a' ; found '{}'", res._s);
  checkDepthError([&]() {
    auto iss = std::istringstream("{\"s\":\"a\",\"u\":[[[{}]]]}");
    [[maybe_unused]] TestStr res = mapper.deserialize<TestStr>(iss);
  });
}

void
testDeserMaxDepthMapped() {
  // object (1) > array (2) > object (3) > array (4)
  std::string json = "{\"c\":[{\"c\":[]}]}";
  {
    anch::json::JSONMapper mapper({.max_depth = 4});
    auto iss = std::istringstream(json);
    TestNode res = mapper.deserialize<TestNode>(iss);
    anch::ut::assertThat(res._children.size() == 1, "1 child expected ; found {}", res._children.size());
  }
  {
    anch::json::JSONMapper mapper({.max_depth = 3});
    checkDepthError([&]() {
      auto iss = std::istringstream(json);
      [[maybe_unused]] TestNode res = mapper.deserialize<TestNode>(iss);
    });
  }
  {
    anch::json::JSONMapper mapper({.max_depth = 0}); // unlimited
    auto iss = std::istringstream(json);
    TestNode res = mapper.deserialize<TestNode>(iss);
    anch::ut::assertThat(res._children.size() == 1, "1 child expected ; found {}", res._children.size());
  }
}
// Nesting depth -

// Objects in ignored fields +
void
testDeserUnknownObject() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true, .deserialize_max_discard_char = -1});
  for(const char* value : {"{}", "{\"a\":1}", "{\"a\":1,\"b\":2}", "{ \"a\" : 1 , \"b\" : { \"c\" : [ 1 , { \"d\" : 2 , \"e\" : 3 } ] } , \"f\" : \"g\" }",
			   "[{\"a\":1,\"b\":2},{\"c\":3,\"d\":{}}]"}) {
    std::string json = std::string("{\"u\":") + value + ",\"s\":\"a\"}";
    std::cout << "Deserialize " << json << std::endl;
    auto iss = std::istringstream(json);
    TestStr res = mapper.deserialize<TestStr>(iss);
    anch::ut::assertThat(res._s == "a", "{}: s should be 'a' ; found '{}'", json, res._s);
  }
}
// Objects in ignored fields -

// Numbers in ignored fields +
void
testDeserUnknownNumber() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  for(const char* number : {"0", "-0", "12", "-1.5", "1e5", "1E5", "-1.5E-3", "2.5e+10", "0e0", "[1e2,-3.5e-1,0]"}) {
    std::string json = std::string("{\"u\":") + number + ",\"s\":\"a\"}";
    std::cout << "Deserialize " << json << std::endl;
    auto iss = std::istringstream(json);
    TestStr res = mapper.deserialize<TestStr>(iss);
    anch::ut::assertThat(res._s == "a", "{}: s should be 'a' ; found '{}'", json, res._s);
  }
}

void
testDeserUnknownNumberKO() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  for(const char* number : {"-", "01", "1.", "1e", "1e+", "1.2.3", "1-2", "1.e2", "-01"}) {
    std::string json = std::string("{\"u\":") + number + ",\"s\":\"a\"}";
    std::cout << "Deserialize " << json << std::endl;
    try {
      auto iss = std::istringstream(json);
      [[maybe_unused]] TestStr res = mapper.deserialize<TestStr>(iss);
      anch::ut::fail("This test should have failed");
    } catch(const MappingError& e) {
      if(e.getErrorCode() != ErrorCode::INVALID_FORMAT) {
        throw;
      }
      std::cout << e.what() << std::endl;
    }
  }
}
// Numbers in ignored fields -

// Required fields +
struct TestRequiredChild {
  int64_t _id = 0;
  std::optional<std::string> _name;
};

template<>
struct anch::Mapping<TestRequiredChild> {
  static void
  registerObject(anch::ObjectMapper<TestRequiredChild> auto& mapper) {
    mapper
      .registerField("id", &TestRequiredChild::_id, anch::val::required)
      .registerField("name", &TestRequiredChild::_name)
      ;
  }
};

struct TestRequired {
  int64_t _id = 0;
  std::string _name;
  std::optional<std::string> _owner;
  std::vector<std::string> _tags;
  int64_t _count = 0;
  std::vector<TestRequiredChild> _children;
  std::map<std::string,TestRequiredChild> _byName;
  std::string _label;
};

template<>
struct anch::Mapping<TestRequired> {
  static void
  registerObject(anch::ObjectMapper<TestRequired> auto& mapper) {
    mapper
      .registerField("id", &TestRequired::_id, anch::val::required)
      .registerField("name", &TestRequired::_name, anch::val::required)
      .registerField("owner", &TestRequired::_owner, anch::val::required) // can be null
      .registerField("tags", &TestRequired::_tags, anch::val::required) // can not be null
      .registerField("count", &TestRequired::_count)
      .registerField("children", &TestRequired::_children)
      .registerField("byName", &TestRequired::_byName)
      .registerField("label",
				  std::function<std::string(const TestRequired&)>([](const TestRequired& obj) -> std::string {
				    return obj._label;
				  }),
				  std::function<void(TestRequired&, const std::string&)>([](TestRequired& obj, const std::string& label) {
				    obj._label = label;
				  }),
				  anch::val::required)
      ;
  }
};

template<typename T>
void
checkMappingError(const std::string& json, ErrorCode code, const std::string& path, const std::string& message,
		  const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS) {
  std::cout << "Deserialize " << json << std::endl;
  anch::json::JSONMapper mapper(options);
  try {
    auto iss = std::istringstream(json);
    [[maybe_unused]] T res = mapper.deserialize<T>(iss);
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    std::cout << e.what() << std::endl;
    anch::ut::assertThat(e.getErrorCode() == code, "Expected error code {} ; found {}", static_cast<int>(code), static_cast<int>(e.getErrorCode()));
    anch::ut::assertThat(e.getPath() == path, "Expected path '{}' ; found '{}'", path, e.getPath());
    std::string expected = path.empty() ? message : path + ": " + message;
    anch::ut::assertThat(std::string(e.what()).starts_with(expected), "Message should start with '{}' ; found '{}'", expected, e.what());
  }
}

void
testDeserRequired() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  {
    auto iss = std::istringstream(R"({"label":"l","tags":["a"],"owner":"o","name":"n","id":2})");
    TestRequired res = mapper.deserialize<TestRequired>(iss);
    anch::ut::assertThat(res._id == 2 && res._name == "n" && res._owner == "o" && res._tags.size() == 1 && res._label == "l",
			 "Unexpected values: {} {} {} {} {}", res._id, res._name, res._owner.value_or("<null>"), res._tags.size(), res._label);
  }
  {
    // Required std::optional can be null ; field which is not required can be null
    auto iss = std::istringstream(R"({"id":1,"name":"n","owner":null,"tags":[],"label":"l","count":null})");
    TestRequired res = mapper.deserialize<TestRequired>(iss);
    anch::ut::assertThat(!res._owner.has_value(), "owner should be empty");
    anch::ut::assertThat(res._count == 0, "count should be 0 ; found {}", res._count);
  }
  {
    // Field found twice
    auto iss = std::istringstream(R"({"id":1,"id":3,"name":"n","owner":null,"tags":[],"label":"l"})");
    TestRequired res = mapper.deserialize<TestRequired>(iss);
    anch::ut::assertThat(res._id == 3, "id should be 3 ; found {}", res._id);
  }
}

void
testDeserRequiredMissing() {
  checkMappingError<TestRequired>(R"({"id":1,"owner":null,"tags":[],"label":"l"})", ErrorCode::MISSING_FIELD, "",
				  "Missing required field 'name' near ");
  checkMappingError<TestRequired>(R"({"tags":[],"owner":"o"})", ErrorCode::MISSING_FIELD, "",
				  "Missing required fields 'id', 'name', 'label' near ");
  checkMappingError<TestRequired>("{}", ErrorCode::MISSING_FIELD, "",
				  "Missing required fields 'id', 'name', 'owner', 'tags', 'label' near ");
}

void
testDeserRequiredNull() {
  checkMappingError<TestRequired>(R"({"id":null,"name":"n","owner":null,"tags":[],"label":"l"})", ErrorCode::CONSTRAINT_VIOLATION,
				  "/id", "Required field can not be null near ");
  checkMappingError<TestRequired>(R"({"id":1,"name":null,"owner":null,"tags":[],"label":"l"})", ErrorCode::CONSTRAINT_VIOLATION,
				  "/name", "Required field can not be null near ");
  checkMappingError<TestRequired>(R"({"id":1,"name":"n","owner":null,"tags":null,"label":"l"})", ErrorCode::CONSTRAINT_VIOLATION,
				  "/tags", "Required field can not be null near ");
  checkMappingError<TestRequired>(R"({"id":1,"name":"n","owner":null,"tags":[],"label":null})", ErrorCode::CONSTRAINT_VIOLATION,
				  "/label", "Required field can not be null near ");
}

void
testDeserRequiredNested() {
  std::string base = R"({"id":1,"name":"n","owner":null,"tags":[],"label":"l",)";
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  {
    auto iss = std::istringstream(base + R"("children":[{"id":1},{"id":2,"name":"b"}],"byName":{"a":{"id":3}}})");
    TestRequired res = mapper.deserialize<TestRequired>(iss);
    anch::ut::assertThat(res._children.size() == 2 && res._byName.size() == 1, "2 children and 1 byName expected ; found {} and {}",
			 res._children.size(), res._byName.size());
  }
  checkMappingError<TestRequired>(base + R"("children":[{"id":1},{"name":"b"}]})", ErrorCode::MISSING_FIELD, "/children/1",
				  "Missing required field 'id' near ");
  checkMappingError<TestRequired>(base + R"("byName":{"a":{"id":3},"b":{"id":null}}})", ErrorCode::CONSTRAINT_VIOLATION,
				  "/byName/b/id", "Required field can not be null near ");
}

struct TestManyRequired {
  std::array<int64_t,70> _values{};
};

template<>
struct anch::Mapping<TestManyRequired> {
  static void
  registerObject(anch::ObjectMapper<TestManyRequired> auto& mapper) {
    for(std::size_t i = 0 ; i < 70 ; ++i) {
      mapper.registerField("f" + std::to_string(i),
				    std::function<void(TestManyRequired&, const int64_t&)>([i](TestManyRequired& obj, const int64_t& value) {
				      obj._values[i] = value;
				    }),
				    anch::val::required);
    }
  }
};

std::string
manyRequiredJson(std::size_t skipped) {
  std::string json = "{";
  for(std::size_t i = 0 ; i < 70 ; ++i) {
    if(i == skipped) {
      continue;
    }
    if(json.size() > 1) {
      json += ',';
    }
    json += "\"f" + std::to_string(i) + "\":" + std::to_string(i);
  }
  return json + "}";
}

void
testDeserRequiredMany() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  auto iss = std::istringstream(manyRequiredJson(70));
  TestManyRequired res = mapper.deserialize<TestManyRequired>(iss);
  anch::ut::assertThat(res._values[69] == 69, "f69 should be 69 ; found {}", res._values[69]);
  // missing fields in first and second bits words
  checkMappingError<TestManyRequired>(manyRequiredJson(3), ErrorCode::MISSING_FIELD, "", "Missing required field 'f3' near ");
  checkMappingError<TestManyRequired>(manyRequiredJson(63), ErrorCode::MISSING_FIELD, "", "Missing required field 'f63' near ");
  checkMappingError<TestManyRequired>(manyRequiredJson(64), ErrorCode::MISSING_FIELD, "", "Missing required field 'f64' near ");
  checkMappingError<TestManyRequired>(manyRequiredJson(69), ErrorCode::MISSING_FIELD, "", "Missing required field 'f69' near ");
}
// Required fields -

// Value constraints +
struct TestPoint {
  int64_t _x = 0;
  bool operator==(const TestPoint&) const = default;
};

template<>
struct anch::Mapping<TestPoint> {
  static void
  registerObject(anch::ObjectMapper<TestPoint> auto& mapper) {
    mapper
      .registerField("x", &TestPoint::_x)
      ;
  }
};

struct TestConstraints {
  std::string _name;
  int32_t _age = 0;
  uint16_t _level = 1;
  int64_t _even = 0;
  double _price = 0.0;
  std::vector<std::string> _tags;
  std::map<std::string,int64_t> _scores;
  std::optional<std::string> _color;
  std::optional<int64_t> _grade;
  std::vector<TestPoint> _points;
  std::set<int64_t> _ids;
  std::string _code;
};

template<>
struct anch::Mapping<TestConstraints> {
  static void
  registerObject(anch::ObjectMapper<TestConstraints> auto& mapper) {
    mapper
      .registerField("name", &TestConstraints::_name, anch::val::required, anch::val::minLength(2), anch::val::maxLength(5))
      .registerField("age", &TestConstraints::_age, anch::val::minimum(0), anch::val::maximum(150))
      .registerField("level", &TestConstraints::_level, anch::val::exclusiveMinimum(0), anch::val::exclusiveMaximum(10))
      .registerField("even", &TestConstraints::_even, anch::val::multipleOf(2))
      .registerField("price", &TestConstraints::_price, anch::val::minimum(0.0), anch::val::multipleOf(0.01))
      .registerField("tags", &TestConstraints::_tags, anch::val::minItems(1), anch::val::maxItems(3),
				  anch::val::uniqueItems, anch::val::items(anch::val::minLength(1)))
      .registerField("scores", &TestConstraints::_scores, anch::val::minProperties(1), anch::val::maxProperties(2),
			      anch::val::items(anch::val::minimum(0)))
      .registerField("color", &TestConstraints::_color, anch::val::allowed({"red", "green"}))
      .registerField("grade", &TestConstraints::_grade, anch::val::allowed({1, 2, 3}))
      .registerField("points", &TestConstraints::_points, anch::val::uniqueItems)
      .registerField("ids", &TestConstraints::_ids, anch::val::maxItems(2))
      .registerField("code",
				  std::function<void(TestConstraints&, const std::string&)>([](TestConstraints& obj, const std::string& code) {
				    obj._code = code;
				  }),
				  anch::val::custom<std::string>([](const std::string& code) -> bool {
				    return code.starts_with('C');
				  }, "Code has to start with 'C'"))
      ;
  }
};

// Constraints are checked at compile time against field types
static_assert(anch::val::FieldConstraint<anch::val::MinLength, std::string>);
static_assert(anch::val::FieldConstraint<anch::val::MinLength, std::optional<std::string>>);
static_assert(!anch::val::FieldConstraint<anch::val::MinLength, int32_t>);
static_assert(!anch::val::FieldConstraint<anch::val::MinLength, std::vector<std::string>>);
static_assert(anch::val::FieldConstraint<anch::val::Minimum<int>, std::shared_ptr<uint64_t>>);
static_assert(!anch::val::FieldConstraint<anch::val::Minimum<int>, bool>);
static_assert(!anch::val::FieldConstraint<anch::val::MinItems, std::string>);
static_assert(!anch::val::FieldConstraint<anch::val::MinItems, std::map<std::string,int>>);
static_assert(anch::val::FieldConstraint<anch::val::MinProperties, std::map<std::string,int>>);
static_assert(anch::val::FieldConstraint<anch::val::Items<anch::val::MinLength>, std::vector<std::string>>);
static_assert(!anch::val::FieldConstraint<anch::val::Items<anch::val::MinLength>, std::vector<int64_t>>);
static_assert(anch::val::FieldConstraint<anch::val::Items<anch::val::Minimum<int>>, std::map<std::string,std::optional<int64_t>>>);
static_assert(!anch::val::FieldConstraint<anch::val::Items<anch::val::Required>, std::vector<std::string>>);

const std::string CONSTRAINTS_BASE = R"({"name":"abc")";

void
checkConstraintError(const std::string& fields, const std::string& path, const std::string& message) {
  checkMappingError<TestConstraints>(CONSTRAINTS_BASE + fields + "}", ErrorCode::CONSTRAINT_VIOLATION, path, message);
}

void
testDeserConstraints() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  // boundary values
  auto iss = std::istringstream(R"({"name":"ab","age":150,"level":9,"even":-4,"price":19.99,"tags":["a","b","c"],)"
				R"("scores":{"a":0,"b":3},"color":"green","grade":3,"points":[{"x":1},{"x":2}],"ids":[1,2],"code":"C1"})");
  TestConstraints res = mapper.deserialize<TestConstraints>(iss);
  anch::ut::assertThat(res._name == "ab" && res._age == 150 && res._level == 9 && res._even == -4, "Invalid values");
  anch::ut::assertThat(res._tags.size() == 3 && res._scores.size() == 2 && res._color == "green" && res._grade == 3, "Invalid values");
  anch::ut::assertThat(res._points.size() == 2 && res._ids.size() == 2 && res._code == "C1", "Invalid values");
  // null and absent values are not checked
  auto nulls = std::istringstream(R"({"name":"abcde","color":null,"grade":null,"tags":null})");
  res = mapper.deserialize<TestConstraints>(nulls);
  anch::ut::assertThat(!res._color.has_value() && res._tags.empty(), "Invalid null values");
}

void
testDeserConstraintsString() {
  checkMappingError<TestConstraints>(R"({"name":"a"})", ErrorCode::CONSTRAINT_VIOLATION, "/name", "Length 1 is lower than minLength 2");
  checkMappingError<TestConstraints>(R"({"name":"abcdef"})", ErrorCode::CONSTRAINT_VIOLATION, "/name", "Length 6 is greater than maxLength 5");
  // length is the number of code points: "été" is 3 code points (5 bytes), "ééééé" 5 code points (10 bytes)
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  for(const char* name : {"été", "ééééé", "\\u00e9t"}) {
    auto iss = std::istringstream(std::string(R"({"name":")") + name + "\"}");
    [[maybe_unused]] TestConstraints res = mapper.deserialize<TestConstraints>(iss);
  }
  checkMappingError<TestConstraints>(R"({"name":"é"})", ErrorCode::CONSTRAINT_VIOLATION, "/name", "Length 1 is lower than minLength 2");
  checkMappingError<TestConstraints>(R"({"name":"éééééé"})", ErrorCode::CONSTRAINT_VIOLATION, "/name", "Length 6 is greater than maxLength 5");
}

void
testDeserConstraintsNumber() {
  checkConstraintError(R"(,"age":-1)", "/age", "Value -1 is lower than minimum 0");
  checkConstraintError(R"(,"age":151)", "/age", "Value 151 is greater than maximum 150");
  checkConstraintError(R"(,"level":0)", "/level", "Value 0 is lower than or equal to exclusiveMinimum 0");
  checkConstraintError(R"(,"level":10)", "/level", "Value 10 is greater than or equal to exclusiveMaximum 10");
  checkConstraintError(R"(,"even":-3)", "/even", "Value -3 is not a multiple of 2");
  checkConstraintError(R"(,"price":-0.5)", "/price", "Value -0.5 is lower than minimum 0");
  checkConstraintError(R"(,"price":1.005)", "/price", "Value 1.005 is not a multiple of 0.01");
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  for(const char* price : {"0", "0.3", "0.07", "1234567.89", "1e3"}) {
    auto iss = std::istringstream(CONSTRAINTS_BASE + R"(,"price":)" + price + "}");
    [[maybe_unused]] TestConstraints res = mapper.deserialize<TestConstraints>(iss);
  }
}

void
testDeserConstraintsItems() {
  checkConstraintError(R"(,"tags":[])", "/tags", "Item count 0 is lower than minItems 1");
  checkConstraintError(R"(,"tags":["a","b","c","d"])", "/tags", "Item count 4 is greater than maxItems 3");
  checkConstraintError(R"(,"tags":["b","a","b"])", "/tags", "Items 0 and 2 are equal (uniqueItems)");
  checkConstraintError(R"(,"tags":["a",""])", "/tags/1", "Length 0 is lower than minLength 1");
  checkConstraintError(R"(,"points":[{"x":1},{"x":2},{"x":1}])", "/points", "Items 0 and 2 are equal (uniqueItems)");
  checkConstraintError(R"(,"ids":[3,1,2])", "/ids", "Item count 3 is greater than maxItems 2");
  checkConstraintError(R"(,"scores":{})", "/scores", "Property count 0 is lower than minProperties 1");
  checkConstraintError(R"(,"scores":{"a":1,"b":2,"c":3})", "/scores", "Property count 3 is greater than maxProperties 2");
  checkConstraintError(R"(,"scores":{"a":1,"b/c":-1})", "/scores/b~1c", "Value -1 is lower than minimum 0");
}

void
testDeserConstraintsAllowed() {
  checkConstraintError(R"(,"color":"blue")", "/color", "Value 'blue' is not allowed (allowed: 'red', 'green')");
  checkConstraintError(R"(,"grade":4)", "/grade", "Value 4 is not allowed (allowed: 1, 2, 3)");
}

void
testDeserConstraintsCustom() {
  checkConstraintError(R"(,"code":"D1")", "/code", "Code has to start with 'C'");
}

void
testDeserConstraintsOption() {
  // neither value constraints nor required fields are checked
  anch::json::MappingOptions options;
  options.deserialize_check_constraints = false;
  anch::json::JSONMapper mapper(options);
  auto iss = std::istringstream(R"({"name":"a","age":-1,"tags":[],"color":"blue","code":"D"})");
  TestConstraints res = mapper.deserialize<TestConstraints>(iss);
  anch::ut::assertThat(res._name == "a" && res._age == -1 && res._color == "blue" && res._code == "D", "Invalid values");
  auto missing = std::istringstream(R"({"age":-1})");
  res = mapper.deserialize<TestConstraints>(missing);
  anch::ut::assertThat(res._name.empty() && res._age == -1, "Missing required field should be accepted");
  auto nulls = std::istringstream(R"({"id":null,"name":null,"owner":null,"tags":null,"label":null})");
  [[maybe_unused]] TestRequired required = mapper.deserialize<TestRequired>(nulls);
  // default options: required fields are checked
  checkMappingError<TestConstraints>(R"({"age":1})", ErrorCode::MISSING_FIELD, "", "Missing required field 'name'");
}

void
testConstraintsFactory() {
  for(double divisor : {0.0, -1.0, std::numeric_limits<double>::quiet_NaN()}) {
    try {
      [[maybe_unused]] auto constraint = anch::val::multipleOf(divisor);
      anch::ut::fail("multipleOf should refuse divisor");
    } catch(const std::invalid_argument& e) {
      std::cout << e.what() << std::endl;
    }
  }
  std::string message;
  anch::ut::assertThat(anch::val::minimum(-1).check(uint64_t{0}, message), "0 should be greater than -1");
  anch::ut::assertThat(!anch::val::maximum(uint64_t{1}).check(int64_t{2}, message), "2 should be greater than 1");
  anch::ut::assertThat(anch::val::maximum(uint64_t{1}).check(int64_t{-1}, message), "-1 should be lower than 1");
  anch::ut::assertThat(anch::val::multipleOf(3).check(std::numeric_limits<int64_t>::min() + 2, message), "-2^63 + 2 should be a multiple of 3");
}
// Value constraints -

// Patterns and formats +
struct TestFormats {
  std::string _code;
  std::optional<std::string> _word;
  std::optional<std::string> _email;
  std::vector<std::string> _ips;
  std::optional<std::string> _secret;
  std::optional<std::string> _even;
};

template<>
struct anch::Mapping<TestFormats> {
  static void
  registerObject(anch::ObjectMapper<TestFormats> auto& mapper) {
    mapper
      .registerField("code", &TestFormats::_code, anch::val::pattern("^[A-Z]{2}[0-9]+$"))
      .registerField("word", &TestFormats::_word, anch::val::pattern("b"))
      .registerField("email", &TestFormats::_email, anch::val::format("email"))
      .registerField("ips", &TestFormats::_ips, anch::val::items(anch::val::format("ipv4")))
      .registerField("secret", &TestFormats::_secret, anch::val::format("password"))
      .registerField("even", &TestFormats::_even, anch::val::format("even-length"))
      ;
  }
};

void
checkFormatError(const std::string& json, const std::string& path, const std::string& message) {
  checkMappingError<TestFormats>(json, ErrorCode::CONSTRAINT_VIOLATION, path, message);
}

void
testDeserPattern() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  auto iss = std::istringstream(R"({"code":"AB12","word":"abc"})");
  TestFormats res = mapper.deserialize<TestFormats>(iss);
  anch::ut::assertThat(res._code == "AB12" && res._word == "abc", "Invalid values");
  checkFormatError(R"({"code":"ab12"})", "/code", "Value does not match pattern ^[A-Z]{2}[0-9]+$");
  checkFormatError(R"({"code":"AB12 "})", "/code", "Value does not match pattern ^[A-Z]{2}[0-9]+$");
  // pattern is not anchored
  checkFormatError(R"({"code":"AB1","word":"ac"})", "/word", "Value does not match pattern b");
}

void
testDeserPatternInvalid() {
  for(const char* source : {"(", "[a-", "a{2,1}"}) {
    try {
      [[maybe_unused]] anch::val::Pattern constraint = anch::val::pattern(source);
      anch::ut::fail(std::string("Invalid pattern should be refused: ") + source);
    } catch(const std::regex_error& e) {
      std::cout << source << ": " << e.what() << std::endl;
    }
  }
}

void
testDeserPatternCompiler() {
  std::string message;
  // Compiler which matches values starting with the pattern
  anch::val::setPatternCompiler([](std::string_view source) -> anch::val::PatternMatcher {
    return [prefix = std::string(source)](std::string_view value) -> bool { return value.starts_with(prefix); };
  });
  anch::val::Pattern prefix = anch::val::pattern("(");
  anch::ut::assertThat(prefix.check(std::string("(a"), message) && !prefix.check(std::string("a("), message), "Custom compiler should be used");
  // Default compiler is restored
  anch::val::setPatternCompiler(nullptr);
  anch::val::Pattern regex = anch::val::pattern("^a+$");
  anch::ut::assertThat(regex.check(std::string("aaa"), message) && !regex.check(std::string("ab"), message), "Default compiler should be used");
  // Patterns built before keep their matcher
  anch::ut::assertThat(prefix.check(std::string("(b"), message), "Matcher should not change");
}

void
checkFormatValues(const std::string& name, std::initializer_list<const char*> valid, std::initializer_list<const char*> invalid) {
  anch::val::Format constraint = anch::val::format(name);
  std::string message;
  for(const char* value : valid) {
    anch::ut::assertThat(constraint.check(std::string(value), message), "'{}' should be a valid {}", value, name);
  }
  for(const char* value : invalid) {
    anch::ut::assertThat(!constraint.check(std::string(value), message), "'{}' should not be a valid {}", value, name);
    anch::ut::assertThat(message == "Value is not a valid " + name + " (format)", "Unexpected message '{}'", message);
  }
}

void
testDeserFormats() {
  checkFormatValues("email", {"joe.bloggs@example.com", "a+b@x.io", "user@localhost", "o'hara@x.com"},
		    {"joe", "@x.com", ".a@x.com", "a.@x.com", "a..b@x.com", "a@-x.com", "a@x..com", "a b@x.com", "a@x.com.", "a@"});
  checkFormatValues("hostname", {"example.com", "a-b.c1", "localhost", "xn--bcher-kva.example"},
		    {"", "-a.com", "a-.com", "a_b.com", "a..b", "a.", (std::string(64, 'a') + ".com").c_str()});
  checkFormatValues("ipv4", {"192.168.0.1", "0.0.0.0", "255.255.255.255"}, {"256.0.0.1", "01.2.3.4", "1.2.3", "1.2.3.4.5", " 1.2.3.4", ""});
  checkFormatValues("ipv6", {"::1", "2001:db8::8a2e:370:7334", "::ffff:192.168.0.1", "::"}, {"1:::2", "12345::", "fe80::1%eth0", "1.2.3.4", ""});
  checkFormatValues("uri", {"https://example.com/a?b=c#d", "urn:isbn:0451450523", "mailto:a@b.c", "http://x/%20", "a+b-c.d:"},
		    {"example.com", "//x", "1http://x", "http://x y", "http://x/%2", "http://x/%zz", "http://x/é", ""});
  checkFormatValues("byte", {"", "YQ==", "YWI=", "YWJj", "+/+/"}, {"YQ", "Y===", "YQ=a", "Y!==", "===="});
#ifdef ANCH_UUID
  checkFormatValues("uuid", {"deb94ebc-be28-4899-981a-29199b7a487d"}, {"deb94ebc", "zeb94ebc-be28-4899-981a-29199b7a487d"});
#endif
#ifdef ANCH_DATE
  checkFormatValues("date-time", {"2020-01-02T03:04:05Z", "2020-01-02T03:04:05.123+01:00"}, {"2020-01-02", "2020-01-02 03:04:05Z"});
  checkFormatValues("date", {"2020-01-02", "2024-02-29"}, {"2020-02-30", "2020-01-02T03:04:05Z"});
#endif
  // Unknown format is an annotation
  checkFormatValues("password", {"", "anything"}, {});
}

void
testDeserFormatMapper() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  auto iss = std::istringstream(R"({"code":"AB1","email":"a@b.c","ips":["1.2.3.4"],"secret":"x","even":"abc"})");
  [[maybe_unused]] TestFormats res = mapper.deserialize<TestFormats>(iss); // even-length is not registered
  checkFormatError(R"({"code":"AB1","email":"a@"})", "/email", "Value is not a valid email (format)");
  checkFormatError(R"({"code":"AB1","ips":["1.2.3.4","1.2.3"]})", "/ips/1", "Value is not a valid ipv4 (format)");
}

void
testDeserFormatRegister() {
  // Registered after mapper creation: validators are found when values are checked
  anch::val::registerFormat("even-length", [](std::string_view value) -> bool { return value.size() % 2 == 0; });
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  auto iss = std::istringstream(R"({"code":"AB1","even":"ab"})");
  [[maybe_unused]] TestFormats res = mapper.deserialize<TestFormats>(iss);
  checkFormatError(R"({"code":"AB1","even":"abc"})", "/even", "Value is not a valid even-length (format)");
  // Replace
  anch::val::registerFormat("even-length", [](std::string_view value) -> bool { return value.size() % 2 == 1; });
  checkFormatError(R"({"code":"AB1","even":"ab"})", "/even", "Value is not a valid even-length (format)");
  // Remove: unknown format again
  anch::val::registerFormat("even-length", nullptr);
  auto removed = std::istringstream(R"({"code":"AB1","even":"ab"})");
  res = mapper.deserialize<TestFormats>(removed);
  // Built-in format can be replaced
  anch::val::registerFormat("email", [](std::string_view value) -> bool { return value.ends_with("@anch"); });
  checkFormatError(R"({"code":"AB1","email":"a@b.c"})", "/email", "Value is not a valid email (format)");
}
// Patterns and formats -

// Serialization constraints +
struct TestGetter {
  int64_t _value = 0;
};

template<>
struct anch::Mapping<TestGetter> {
  static void
  registerObject(anch::ObjectMapper<TestGetter> auto& mapper) {
    mapper
      .registerField("value",
			      std::function<int64_t(const TestGetter&)>([](const TestGetter& obj) -> int64_t { return obj._value; }),
			      std::function<void(TestGetter&, const int64_t&)>([](TestGetter& obj, const int64_t& value) { obj._value = value; }),
			      anch::val::minimum(0))
      ;
  }
};

struct TestSerRequired {
  std::map<std::string,int64_t> _map;
  std::optional<std::string> _opt;
  std::vector<int64_t> _list;
  std::shared_ptr<TestPoint> _point;
  std::vector<int64_t> _other;
  std::optional<std::string> _none;
};

template<>
struct anch::Mapping<TestSerRequired> {
  static void
  registerObject(anch::ObjectMapper<TestSerRequired> auto& mapper) {
    mapper
      .registerField("map", &TestSerRequired::_map, anch::val::required)
      .registerField("opt", &TestSerRequired::_opt, anch::val::required)
      .registerField("list", &TestSerRequired::_list, anch::val::required)
      .registerField("point", &TestSerRequired::_point, anch::val::required)
      .registerField("other", &TestSerRequired::_other)
      .registerField("none", &TestSerRequired::_none)
      ;
  }
};

TestConstraints
validConstraints() {
  TestConstraints value;
  value._name = "abc";
  value._tags = {"a", "b"};
  value._scores = {{"a", 1}};
  return value;
}

template<typename T>
void
checkSerError(const T& value, const std::string& path, const std::string& message) {
  anch::json::MappingOptions options;
  options.serialize_check_constraints = true;
  try {
    std::string json = anch::json::serialize(value, options);
    anch::ut::fail("Serialization should have failed: " + json);
  } catch(const MappingError& e) {
    std::cout << e.what() << std::endl;
    anch::ut::assertThat(e.getErrorCode() == ErrorCode::CONSTRAINT_VIOLATION, "CONSTRAINT_VIOLATION expected");
    anch::ut::assertThat(e.getPath() == path, "Expected path '{}' ; found '{}'", path, e.getPath());
    std::string expected = path + ": " + message;
    anch::ut::assertThat(expected == e.what(), "Expected message '{}' ; found '{}'", expected, e.what());
  }
}

void
testSerConstraintsDefault() {
  // serialize_check_constraints is false by default
  TestConstraints value = validConstraints();
  value._name = "a";
  value._age = -1;
  std::string json = anch::json::serialize(value);
  anch::ut::assertThat(json.find(R"("name":"a")") != std::string::npos && json.find(R"("age":-1)") != std::string::npos, "Invalid JSON {}", json);
}

void
testSerConstraints() {
  anch::json::MappingOptions options;
  options.serialize_check_constraints = true;
  std::string json = anch::json::serialize(validConstraints(), options);
  std::cout << json << std::endl;
  TestConstraints value = validConstraints();
  value._name = "a";
  checkSerError(value, "/name", "Length 1 is lower than minLength 2");
  value = validConstraints();
  value._age = 151;
  checkSerError(value, "/age", "Value 151 is greater than maximum 150");
  value = validConstraints();
  value._tags = {"a", ""};
  checkSerError(value, "/tags/1", "Length 0 is lower than minLength 1");
  value = validConstraints();
  value._scores = {{"a", 1}, {"b/c", -1}};
  checkSerError(value, "/scores/b~1c", "Value -1 is lower than minimum 0");
  value = validConstraints();
  value._color = "blue";
  checkSerError(value, "/color", "Value 'blue' is not allowed (allowed: 'red', 'green')");
  // nested objects and root array
  std::vector<TestConstraints> values = {validConstraints(), validConstraints()};
  values[1]._points = {TestPoint{1}, TestPoint{1}};
  checkSerError(values, "/1/points", "Items 0 and 1 are equal (uniqueItems)");
  // getter
  checkSerError(TestGetter{-1}, "/value", "Value -1 is lower than minimum 0");
}

void
testSerRequired() {
  anch::json::MappingOptions options;
  options.serialize_null = false;
  options.serialize_empty_col = false;
  TestSerRequired value;
  std::string json = anch::json::serialize(value, options);
  anch::ut::assertThat(json == R"({"map":{},"opt":null,"list":[],"point":null})", "Unexpected JSON {}", json);
  // round trip: required fields are present
  auto iss = std::istringstream(json);
  [[maybe_unused]] TestSerRequired copy = anch::json::deserialize<TestSerRequired>(iss);
  value._map = {{"a", 1}};
  value._opt = "o";
  value._list = {1};
  value._point = std::make_shared<TestPoint>(TestPoint{2});
  json = anch::json::serialize(value, options);
  anch::ut::assertThat(json == R"({"map":{"a":1},"opt":"o","list":[1],"point":{"x":2}})", "Unexpected JSON {}", json);
}
// Serialization constraints -

// Validation +
struct TestNested {
  TestConstraints _main;
  std::optional<TestConstraints> _opt;
  std::shared_ptr<TestConstraints> _ptr;
  std::map<std::string,TestConstraints> _map;
  std::vector<TestConstraints> _list;
};

template<>
struct anch::Mapping<TestNested> {
  static void
  registerObject(anch::ObjectMapper<TestNested> auto& mapper) {
    mapper
      .registerField("main", &TestNested::_main)
      .registerField("opt", &TestNested::_opt)
      .registerField("ptr", &TestNested::_ptr)
      .registerField("map", &TestNested::_map, anch::val::maxProperties(1))
      .registerField("list", &TestNested::_list)
      ;
  }
};

void
checkViolations(const std::vector<anch::val::Violation>& violations, const std::vector<std::pair<std::string,std::string>>& expected) {
  for(const anch::val::Violation& violation : violations) {
    std::cout << violation.path << ": " << violation.message << std::endl;
  }
  anch::ut::assertThat(violations.size() == expected.size(), "{} violations expected ; found {}", expected.size(), violations.size());
  for(std::size_t i = 0 ; i < expected.size() ; ++i) {
    anch::ut::assertThat(violations[i].path.pointer() == expected[i].first && violations[i].message == expected[i].second,
			 "Expected '{}: {}' ; found '{}: {}'", expected[i].first, expected[i].second, violations[i].path.pointer(), violations[i].message);
  }
}

void
testValidate() {
  anch::ut::assertThat(anch::val::validate(validConstraints()).empty(), "Valid object should not have violation");
  anch::ut::assertThat(anch::val::isValid(validConstraints()), "Valid object should be valid");
  // Every violation is reported, in fields registration order
  TestConstraints value = validConstraints();
  value._name = "a";
  value._age = -1;
  value._tags = {"a", "", "a"};
  value._scores = {{"a", -1}, {"b", -2}};
  value._color = "blue";
  anch::ut::assertThat(!anch::val::isValid(value), "Invalid object should not be valid");
  checkViolations(anch::val::validate(value), {
      {"/name", "Length 1 is lower than minLength 2"},
      {"/age", "Value -1 is lower than minimum 0"},
      {"/tags", "Items 0 and 2 are equal (uniqueItems)"},
      {"/tags/1", "Length 0 is lower than minLength 1"},
      {"/scores/a", "Value -1 is lower than minimum 0"},
      {"/scores/b", "Value -2 is lower than minimum 0"},
      {"/color", "Value 'blue' is not allowed (allowed: 'red', 'green')"}});
  // Getter, required fields are not checked
  checkViolations(anch::val::validate(TestGetter{-1}), {{"/value", "Value -1 is lower than minimum 0"}});
  checkViolations(anch::val::validate(TestSerRequired{}), {});
}

void
testValidateNested() {
  TestNested value;
  value._main = validConstraints();
  value._main._name = "a";
  checkViolations(anch::val::validate(value), {{"/main/name", "Length 1 is lower than minLength 2"}});
  value._opt = validConstraints();
  value._opt->_age = 200;
  value._ptr = std::make_shared<TestConstraints>(validConstraints());
  value._ptr->_color = "blue";
  value._map = {{"a/b", validConstraints()}, {"c", validConstraints()}};
  value._map["a/b"]._level = 0;
  value._list = {validConstraints(), validConstraints()};
  value._list[1]._tags = {};
  checkViolations(anch::val::validate(value), {
      {"/main/name", "Length 1 is lower than minLength 2"},
      {"/opt/age", "Value 200 is greater than maximum 150"},
      {"/ptr/color", "Value 'blue' is not allowed (allowed: 'red', 'green')"},
      {"/map", "Property count 2 is greater than maxProperties 1"},
      {"/map/a~1b/level", "Value 0 is lower than or equal to exclusiveMinimum 0"},
      {"/list/1/tags", "Item count 0 is lower than minItems 1"}});
  // Root containers
  std::vector<TestConstraints> values = {validConstraints(), validConstraints()};
  values[0]._name = "toolong";
  checkViolations(anch::val::validate(values), {{"/0/name", "Length 7 is greater than maxLength 5"}});
  std::map<std::string,TestConstraints> map = {{"k", validConstraints()}};
  map["k"]._even = 3;
  checkViolations(anch::val::validate(map), {{"/k/even", "Value 3 is not a multiple of 2"}});
  // Primitive values have no constraint
  anch::ut::assertThat(anch::val::validate(std::string("a")).empty() && anch::val::validate(42).empty(), "Primitives are valid");
}
// Validation -

// Error path +
void
testErrorPath() {
  // array item
  checkMappingError<TestRequired>(R"({"id":1,"name":"n","owner":null,"tags":["a","b",3],"label":"l"})", ErrorCode::INVALID_FORMAT,
				  "/tags/2", "Unexpected character");
  // map key: '~' and '/' are escaped
  checkMappingError<TestRequired>(R"({"id":1,"name":"n","owner":null,"tags":[],"label":"l","byName":{"a/b~c":{"id":"x"}}})",
				  ErrorCode::INVALID_FORMAT, "/byName/a~1b~0c/id", "Unexpected character");
  // unknown field
  checkMappingError<TestRequired>(R"({"id":1,"unknown":2})", ErrorCode::UNEXPECTED_FIELD, "/unknown", "Unexpected field found");
  // error outside value: no path
  checkMappingError<TestRequired>(R"({"id":1"name":"n"})", ErrorCode::INVALID_FORMAT, "", "Unexpected character");
}
// Error path -

// Error message +
void
checkErrorMessage(const std::string& json, const std::string& expected) {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  try {
    auto iss = std::istringstream(json);
    [[maybe_unused]] TestStr res = mapper.deserialize<TestStr>(iss);
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    std::cout << e.what() << std::endl;
    anch::ut::assertThat(expected == e.what(), "Expected message '{}' ; found '{}'", expected, e.what());
  }
}

void
testErrorMessage() {
  // 10 characters before error (error character included) and 10 after
  checkErrorMessage("{\"s\":\"abc\"x\"0123456789ABCDEF\"}", "Unexpected character 'x' found at 9 near '\"s\":\"abc\"x\"012345678'");
  // Less than 10 characters after error
  checkErrorMessage("{\"s\":\"abc\"x}", "Unexpected character 'x' found at 9 near '\"s\":\"abc\"x}'");
}

void
testErrorMessageEnd() {
  checkErrorMessage("{\"s\":\"abc\"", "Unexpected end of data near '\"s\":\"abc\"'");
}
// Error message -

// UUID +
#ifdef ANCH_UUID
struct TestUuid {
  anch::UUID _id;
};

template<>
struct anch::Mapping<TestUuid> {
  static void
  registerObject(anch::ObjectMapper<TestUuid> auto& mapper) {
    mapper
      .registerField("id", &TestUuid::_id)
      ;
  }
};

void
testDeserUuid() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  auto iss = std::istringstream("{\"id\":\"DEB94EBC-be28-4899-981a-29199b7a487d\"}");
  TestUuid res = mapper.deserialize<TestUuid>(iss);
  anch::ut::assertThat(res._id.toString() == "deb94ebc-be28-4899-981a-29199b7a487d", "Found {}", res._id.toString());
}

void
testDeserUuidKO() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  for(const char* id : {"1234zzzz-e89b-42d3-a456-426614174000", "deb94ebc-be28-4899-981a-29199b7a487", "", "not an UUID"}) {
    std::string json = std::string("{\"id\":\"") + id + "\"}";
    std::cout << "Deserialize " << json << std::endl;
    try {
      auto iss = std::istringstream(json);
      [[maybe_unused]] TestUuid res = mapper.deserialize<TestUuid>(iss);
      anch::ut::fail("This test should have failed");
    } catch(const MappingError& e) {
      if(e.getErrorCode() != ErrorCode::INVALID_FORMAT) {
        throw;
      }
      std::cout << e.what() << std::endl;
      anch::ut::assertThat(e.getPath() == "/id", "Path should be '/id' ; found '{}'", e.getPath());
      anch::ut::assertThat(std::string(e.what()).starts_with("/id: Invalid UUID '"), "Message should come from UUID parser ; found '{}'", e.what());
    }
  }
}
#endif // ANCH_UUID
// UUID -

// Dates +
#ifdef ANCH_DATE
struct TestDate {
  anch::date::Date _timestamp{false};
  std::optional<anch::date::Date> _updated;
  std::chrono::year_month_day _birth{};
  std::vector<std::chrono::year_month_day> _days;
};

template<>
struct anch::Mapping<TestDate> {
  static void
  registerObject(anch::ObjectMapper<TestDate> auto& mapper) {
    mapper
      .registerField("timestamp", &TestDate::_timestamp)
      .registerField("updated", &TestDate::_updated)
      .registerField("birth", &TestDate::_birth)
      .registerField("days", &TestDate::_days)
      ;
  }
};

void
testDeserDate() {
  using namespace std::chrono;
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  auto iss = std::istringstream(R"({"timestamp":"2025-07-01T12:00:00.5+02:00","updated":null,"birth":"2024-02-29","days":["2025-01-01","2025-12-31"]})");
  TestDate res = mapper.deserialize<TestDate>(iss);
  auto nanos = duration_cast<nanoseconds>(static_cast<system_clock::time_point>(res._timestamp).time_since_epoch()).count();
  anch::ut::assertThat(nanos == 1751364000500000000, "Invalid timestamp: {}", nanos);
  anch::ut::assertThat(!res._updated.has_value(), "updated should be empty");
  anch::ut::assertThat(res._birth == 2024y/February/29d, "Invalid birth");
  anch::ut::assertThat((res._days == std::vector<year_month_day>{2025y/January/1d, 2025y/December/31d}), "Invalid days");
}

void
testSerDate() {
  using namespace std::chrono;
  TestDate value;
  value._timestamp = anch::date::Date(system_clock::time_point(nanoseconds(1751364000123000000)));
  value._updated = anch::date::Date(static_cast<std::time_t>(0));
  value._birth = 2024y/February/29d;
  value._days = {2025y/January/1d};
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  std::string json = mapper.serialize(value);
  std::string expected = R"({"timestamp":"2025-07-01T10:00:00.123Z","updated":"1970-01-01T00:00:00Z","birth":"2024-02-29","days":["2025-01-01"]})";
  anch::ut::assertThat(json == expected, "Expected {} ; found {}", expected, json);
  auto iss = std::istringstream(json);
  TestDate copy = mapper.deserialize<TestDate>(iss);
  anch::ut::assertThat(copy._timestamp == value._timestamp && copy._updated == value._updated
                       && copy._birth == value._birth && copy._days == value._days, "Round trip failed");
  // Invalid full-date is not serialized +
  value._birth = year_month_day{};
  try {
    json = mapper.serialize(value);
    anch::ut::fail("Invalid date should not be serialized");
  } catch(const std::invalid_argument& e) {
    std::cout << e.what() << std::endl;
  }
  // Invalid full-date is not serialized -
}

void
testDeserDateKO() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  for(auto [json, path] : std::initializer_list<std::pair<const char*, std::string>>{
      {R"({"timestamp":"2025-07-01 10:00:00Z"})", "/timestamp"}, {R"({"timestamp":"2025-07-01"})", "/timestamp"},
      {R"({"timestamp":"3000-01-01T00:00:00Z"})", "/timestamp"}, {R"({"birth":"2025-02-29"})", "/birth"},
      {R"({"days":["2025-01-01T00:00:00Z"]})", "/days/0"}}) {
    std::cout << "Deserialize " << json << std::endl;
    try {
      auto iss = std::istringstream(json);
      [[maybe_unused]] TestDate res = mapper.deserialize<TestDate>(iss);
      anch::ut::fail("This test should have failed");
    } catch(const MappingError& e) {
      if(e.getErrorCode() != ErrorCode::INVALID_FORMAT) {
        throw;
      }
      std::cout << e.what() << std::endl;
      anch::ut::assertThat(e.getPath() == path, "Path should be '{}' ; found '{}'", path, e.getPath());
      anch::ut::assertThat(std::string(e.what()).starts_with(path + ": Invalid RFC 3339 "), "Message should come from RFC 3339 parser ; found '{}'", e.what());
    }
  }
}
#endif // ANCH_DATE
// Dates -

// Data after root value +
void
checkTrailingKo(const std::function<void(anch::json::JSONMapper&, std::istream&)>& deser, const std::string& json,
		ErrorCode expected, int64_t maxDiscard = 0) {
  for(std::size_t bufferSize : {std::size_t(3), std::size_t(1024)}) {
    anch::json::JSONMapper mapper({.deserialize_max_discard_char = maxDiscard, .buffer_size = bufferSize});
    std::istringstream iss(json);
    try {
      deser(mapper, iss);
      anch::ut::fail("This test should have failed");
    } catch(const MappingError& e) {
      anch::ut::assertThat(e.getErrorCode() == expected, "Unexpected error for '{}' with buffer_size={}: {}", json, bufferSize, e.what());
      std::cout << e.what() << std::endl;
    }
  }
}

void
testDeserTrailingData() {
  auto object = [](anch::json::JSONMapper& mapper, std::istream& is) { TestStr v; mapper.deserialize(v, is); };
  auto array = [](anch::json::JSONMapper& mapper, std::istream& is) { std::vector<TestStr> v; mapper.deserialize(v, is); };
  auto map = [](anch::json::JSONMapper& mapper, std::istream& is) { std::map<std::string,TestStr> v; mapper.deserialize(v, is); };
  auto value = [](anch::json::JSONMapper& mapper, std::istream& is) { [[maybe_unused]] TestStr v = mapper.deserialize<TestStr>(is); };
  for(const char* json : {R"({"s":"a"} x)", R"({"s":"a"}})", R"({"s":"a"}{"s":"b"})", R"({"s":"a"},)"}) {
    checkTrailingKo(object, json, ErrorCode::INVALID_FORMAT, -1);
    checkTrailingKo(value, json, ErrorCode::INVALID_FORMAT, -1);
  }
  checkTrailingKo(array, R"([{"s":"a"}]])", ErrorCode::INVALID_FORMAT);
  checkTrailingKo(array, R"([{"s":"a"}],[])", ErrorCode::INVALID_FORMAT);
  checkTrailingKo(map, R"({"k":{"s":"a"}}x)", ErrorCode::INVALID_FORMAT);
  // Trailing whitespaces are discardable characters, as whitespaces between tokens
  checkTrailingKo(object, "{\"s\":\"a\"}\n", ErrorCode::POTENTIAL_OVERFLOW);
  checkTrailingKo(object, "{\"s\":\"a\"}\n  \n", ErrorCode::POTENTIAL_OVERFLOW, 2);
}

void
testDeserTrailingSpaces() {
  for(std::size_t bufferSize : {std::size_t(3), std::size_t(9), std::size_t(1024)}) {
    for(auto [json, maxDiscard] : std::initializer_list<std::pair<const char*, int64_t>>{
	{"{\"s\":\"a\"}", 0}, {"{\"s\":\"a\"}\n", -1}, {"{\"s\":\"a\"} \r\n\t ", -1}, {"{\"s\":\"a\"}\n", 1}}) {
      anch::json::JSONMapper mapper({.deserialize_max_discard_char = maxDiscard, .buffer_size = bufferSize});
      std::istringstream iss(json);
      TestStr res;
      mapper.deserialize(res, iss);
      anch::ut::assertThat(res._s == "a", "Unexpected value '{}' with buffer_size={}", res._s, bufferSize);
    }
  }
  std::istringstream iss(R"(["a","b"]  )");
  std::vector<std::string> values;
  anch::json::JSONMapper({.deserialize_max_discard_char = -1}).deserialize(values, iss);
  anch::ut::assertThat(values == std::vector<std::string>({"a", "b"}), "Unexpected array of {} values", values.size());
}
// Data after root value -

// Trailing comma +
void
testDeserUnknownStrStrict() {
  // Ignored fields are checked as mapped ones: escape sequences, control characters, UTF-8
  for(const char* json : {"{\"s\":\"a\",\"x\":\"\x01\"}", "{\"s\":\"a\",\"x\":\"\xFF\"}", "{\"s\":\"a\",\"x\":\"\\q\"}",
      "{\"s\":\"a\",\"x\":[\"\xC3\"]}", "{\"s\":\"a\",\"x\":{\"\t\":1}}"}) {
    anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
    std::istringstream iss(json);
    TestStr res;
    try {
      mapper.deserialize(res, iss);
      anch::ut::fail("This test should have failed");
    } catch(const MappingError& e) {
      anch::ut::assertThat(e.getErrorCode() == ErrorCode::INVALID_FORMAT, "Unexpected error: {}", e.what());
      std::cout << e.what() << std::endl;
    }
  }
}

void
testDeserTrailingComma() {
  auto parse = [](const std::string& json, bool ignoreUnknown) {
    anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = ignoreUnknown});
    std::istringstream iss(json);
    TestStr res;
    mapper.deserialize(res, iss);
    return res;
  };
  auto parseRequired = [](const std::string& json) {
    std::istringstream iss(json);
    TestRequiredChild res;
    anch::json::JSONMapper(anch::json::DEFAULT_MAPPING_OPTIONS).deserialize(res, iss);
    return res;
  };
  // Mapped object, mapped object with required fields, ignored object
  for(auto [json, ignoreUnknown] : std::initializer_list<std::pair<const char*, bool>>{
      {R"({"s":"a",})", false}, {R"({,})", false}, {R"({"s":"a","x":{"y":1,}})", true}, {R"({"s":"a","x":{,}})", true},
      {R"({"s":"a","x":[1,]})", true}}) {
    try {
      parse(json, ignoreUnknown);
      anch::ut::fail("This test should have failed");
    } catch(const MappingError& e) {
      anch::ut::assertThat(e.getErrorCode() == ErrorCode::INVALID_FORMAT, "Unexpected error for {}: {}", json, e.what());
      std::cout << json << ": " << e.what() << std::endl;
    }
  }
  try {
    parseRequired(R"({"id":1,})");
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    anch::ut::assertThat(e.getErrorCode() == ErrorCode::INVALID_FORMAT, "Unexpected error: {}", e.what());
  }
  // Empty objects are still accepted
  anch::ut::assertThat(parse("{}", false)._s.empty(), "Empty object expected");
  anch::ut::assertThat(parse(R"({"s":"a","x":{},"y":{"z":{}}})", true)._s == "a", "Ignored empty objects expected");
  anch::ut::assertThat(parseRequired(R"({"id":1})")._id == 1, "Required field expected");
}
// Trailing comma -

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH JSON mapper unit tests")
    .description("Test AnCH JSON mapper library")
    .initialize(beforeAll)
    .add("json-ser-full", testFullSerDefaultOptions)
    .add("json-deser-list", testDeserList)
    .add("json-deser-unf-field", testDeserUnknownFieldStructFail)
    .add("json-deser-unf-str", testDeserUnknownFieldStructOK)
    .add("json-deser-unf", testDeserUnknownFieldOK)
    .add("json-deser-unf-int", testDeserUnknownFieldIntOK)
    .add("json-deser-unf-empty-col", testDeserUnknownFieldEmptyColOK)
    .add("json-deser-unf-int-col", testDeserUnknownFielIntColOK)
    .add("json-deser-unf-wtf-col", testDeserUnknownFielWTFColOK)
    .add("json-full-deser-default", testFullDeserDefaultOptions)
    .add("json-deser-discard128", testFulDeserDiscard128)
    .add("json-deser-discard128-col", testFulDeserDiscard128Col)
    .add("json-deser-discard128-col-buf2", testFulDeserDiscard128ColBuf2)
    .add("json-iomanip", testIOManip)
    .add("json-ser-map", testSerializeMap)
    .add("json-deser-map", testDeserializeMap)
    .add("json-ser-flux", testSerFlux)
    .add("json-deser-flux", testDeserFlux)
    .add("json-deser-int16-overflow", testDeserInt16Overflow)
    .add("json-deser-int16-underflow", testDeserInt16Underflow)
    .add("json-deser-int32-overflow", testDeserInt32Overflow)
    .add("json-deser-int32-underflow", testDeserInt32Underflow)
    .add("json-deser-uint16-overflow", testDeserUint16Overflow)
    .add("json-deser-uint32-overflow", testDeserUint32Overflow)
    .add("json-deser-range-limits", testDeserRangeLimits)
    .add("json-deser-float-overflow", testDeserFloatOverflow)
    .add("json-deser-float-negative-overflow", testDeserFloatNegativeOverflow)
    .add("json-deser-float-limits", testDeserFloatLimits)
    .add("json-deser-max-payload-ko", testDeserMaxPayloadKo)
    .add("json-deser-max-payload-refill-ko", testDeserMaxPayloadRefillKo)
    .add("json-deser-max-payload-limit", testDeserMaxPayloadLimit)
    .add("json-deser-trailing-data", testDeserTrailingData)
    .add("json-deser-trailing-spaces", testDeserTrailingSpaces)
    .add("json-deser-trailing-comma", testDeserTrailingComma)
    .add("json-deser-unknown-str-strict", testDeserUnknownStrStrict)
    .add("json-ser-escape", testSerEscape)
    .add("json-ser-escape-map-key", testSerEscapeMapKey)
    .add("json-deser-max-depth-unknown", testDeserMaxDepthUnknown)
    .add("json-deser-max-depth-unknown-limit", testDeserMaxDepthUnknownLimit)
    .add("json-deser-max-depth-mapped", testDeserMaxDepthMapped)
    .add("json-deser-unknown-object", testDeserUnknownObject)
    .add("json-deser-unknown-number", testDeserUnknownNumber)
    .add("json-deser-unknown-number-ko", testDeserUnknownNumberKO)
    .add("json-deser-required", testDeserRequired)
    .add("json-deser-required-missing", testDeserRequiredMissing)
    .add("json-deser-required-null", testDeserRequiredNull)
    .add("json-deser-required-nested", testDeserRequiredNested)
    .add("json-deser-required-many", testDeserRequiredMany)
    .add("json-deser-constraints", testDeserConstraints)
    .add("json-deser-constraints-string", testDeserConstraintsString)
    .add("json-deser-constraints-number", testDeserConstraintsNumber)
    .add("json-deser-constraints-items", testDeserConstraintsItems)
    .add("json-deser-constraints-allowed", testDeserConstraintsAllowed)
    .add("json-deser-constraints-custom", testDeserConstraintsCustom)
    .add("json-deser-constraints-option", testDeserConstraintsOption)
    .add("json-constraints-factory", testConstraintsFactory)
    .add("json-deser-pattern", testDeserPattern)
    .add("json-deser-pattern-invalid", testDeserPatternInvalid)
    .add("json-deser-pattern-compiler", testDeserPatternCompiler)
    .add("json-deser-formats", testDeserFormats)
    .add("json-deser-format-mapper", testDeserFormatMapper)
    .add("json-deser-format-register", testDeserFormatRegister)
    .add("json-ser-constraints-default", testSerConstraintsDefault)
    .add("json-ser-constraints", testSerConstraints)
    .add("json-ser-required", testSerRequired)
    .add("json-validate", testValidate)
    .add("json-validate-nested", testValidateNested)
    .add("json-error-path", testErrorPath)
    .add("json-error-message", testErrorMessage)
    .add("json-error-message-end", testErrorMessageEnd)
#ifdef ANCH_UUID
    .add("json-deser-uuid", testDeserUuid)
    .add("json-deser-uuid-ko", testDeserUuidKO)
#endif
#ifdef ANCH_DATE
    .add("json-deser-date", testDeserDate)
    .add("json-ser-date", testSerDate)
    .add("json-deser-date-ko", testDeserDateKO)
#endif
    ;
}
