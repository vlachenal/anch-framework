#include "optional.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "ut/unit.hpp"
#include "ut/assert.hpp"

struct A {
  std::string a;
};

struct B {
  std::string b;
};

// Count copies to check that values are moved
struct Counted {
  static int copies;
  int value;
  Counted(int val): value(val) {}
  Counted(const Counted& other): value(other.value) { ++copies; }
  Counted(Counted&& other) noexcept: value(other.value) {}
  Counted& operator=(const Counted&) = default;
  Counted& operator=(Counted&&) noexcept = default;
};

int Counted::copies = 0;

void
testOptionalFunc() {
  std::optional<int> val(2);
  std::optional<int> noval;
  int found = 0;
  bool fallback = false;
  anch::applyIfValue<int>(val, [&found](const int& v) { found = v; });
  anch::ut::assertThat(found == 2, "applyIfValue has found {} instead of 2", found);
  anch::applyIfValue<int>(noval, [&found](const int&) { found = -1; });
  anch::ut::assertThat(found == 2, "applyIfValue has been applied on empty optional");
  anch::applyIfValueOrElse<int>(noval, [&found](const int&) { found = -1; }, [&fallback]() { fallback = true; });
  anch::ut::assertTrue(fallback && found == 2, "applyIfValueOrElse has not applied fallback");
}

void
testOptionalClass() {
  anch::Optional<int> val(2);
  anch::Optional<int> noval;
  int found = 0;
  bool fallback = false;
  val.ifValue([&found](const int& v) { found = v; });
  anch::ut::assertThat(found == 2, "ifValue has found {} instead of 2", found);
  noval.ifValueOrElse([&found](const int&) { found = -1; }, [&fallback]() { fallback = true; });
  anch::ut::assertTrue(fallback && found == 2, "ifValueOrElse has not applied fallback");

  A a1;
  a1.a = "test mapping";
  anch::Optional<A> opt1(a1);
  anch::Optional<B> optM = opt1.map<B>([](const A& a) {
    return B{a.a};
  });
  anch::ut::assertTrue(optM.has_value() && optM->b == "test mapping", "Mapped value is unexpected");
  anch::Optional<A> opt2;
  optM = opt2.map<B>([](const A& a) {
    return B{a.a};
  });
  anch::ut::assertFalse(optM.has_value(), "Mapped empty optional has a value");
}

void
testStdInterface() {
  // std::optional interface is accessible
  anch::Optional<int> val(3);
  anch::ut::assertTrue(val.has_value() && *val == 3 && val.value() == 3 && val, "std::optional interface is not available");
  val = 4;
  anch::ut::assertThat(*val == 4, "Assigned value is {} instead of 4", *val);
  val = std::nullopt;
  anch::ut::assertFalse(val.has_value(), "Optional has a value after nullopt assignment");
  anch::ut::assertThat(val.value_or(5) == 5, "value_or gives {} instead of 5", val.value_or(5));
  // Conversion from std::optional
  std::optional<int> stdOpt(6);
  anch::Optional<int> fromStd(stdOpt);
  anch::ut::assertTrue(fromStd.has_value() && *fromStd == 6, "Conversion from std::optional failed");
  // In place construction
  anch::Optional<std::vector<int>> vec(std::in_place, {1, 2, 3});
  anch::ut::assertThat(vec->size() == 3, "In place vector has {} elements instead of 3", vec->size());
}

void
testMove() {
  // Move-only type
  anch::Optional<std::unique_ptr<int>> ptr(std::make_unique<int>(7));
  anch::Optional<std::unique_ptr<int>> moved(std::move(ptr));
  anch::ut::assertTrue(moved.has_value() && **moved == 7, "Move-only value has not been moved");
  // Values are moved, not copied
  Counted::copies = 0;
  anch::Optional<Counted> counted(Counted(1));
  anch::Optional<Counted> counted2(std::move(counted));
  anch::Optional<Counted> fromStd(std::optional<Counted>(Counted(2)));
  anch::ut::assertThat(Counted::copies == 0, "{} copies instead of moves", Counted::copies);
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH optional unit tests")
    .description("Test AnCH optional utiliy class and functions")
    .add("opt-func", testOptionalFunc)
    .add("opt-class", testOptionalClass)
    .add("opt-std-interface", testStdInterface)
    .add("opt-move", testMove)
    ;
}
