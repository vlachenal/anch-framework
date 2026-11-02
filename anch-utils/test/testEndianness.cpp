#include "endianness.hpp"

#include <array>
#include <cstdint>
#include <cstring>

#include "ut/unit.hpp"
#include "ut/assert.hpp"

static_assert(anch::isBigEndian() != anch::isLittleEndian(), "System has to be either big or little endian");

void
testEndianness() {
  const uint32_t value = 0x01020304;
  const auto* bytes = reinterpret_cast<const uint8_t*>(&value);
  anch::ut::assertTrue(anch::isLittleEndian() == (bytes[0] == 0x04), "isLittleEndian does not match memory layout");
  anch::ut::assertTrue(anch::isBigEndian() == (bytes[0] == 0x01), "isBigEndian does not match memory layout");
}

// dest has to contain src native bytes in reverse order
template<typename T>
void
checkByteSwap(T src) {
  std::array<uint8_t,sizeof(T)> native{};
  std::memcpy(native.data(), &src, sizeof(T));
  std::array<uint8_t,sizeof(T)> dest{};
  anch::byteSwap(src, dest.data());
  for(std::size_t i = 0 ; i < sizeof(T) ; ++i) {
    anch::ut::assertThat(dest[i] == native[sizeof(T) - 1 - i], "{} bytes swap: byte {} is {:02x} instead of {:02x}",
			 sizeof(T), i, dest[i], native[sizeof(T) - 1 - i]);
  }
}

void
testByteSwap() {
  checkByteSwap(uint16_t(0x0102));
  checkByteSwap(uint32_t(0x01020304));
  checkByteSwap(uint64_t(0x0102030405060708));
  if(anch::isLittleEndian()) { // on little endian system, swapped bytes are in big endian order
    std::array<uint8_t,4> dest{};
    anch::byteSwap(uint32_t(0x01020304), dest.data());
    anch::ut::assertTrue(dest == std::array<uint8_t,4>{0x01, 0x02, 0x03, 0x04}, "Swapped bytes are not in big endian order");
  }
}

void
testByteSwapInPlace() {
  // SHA-1 / SHA-2 swap words into their own storage
  uint32_t word = 0x01020304;
  anch::byteSwap(word, reinterpret_cast<uint8_t*>(&word));
  anch::ut::assertThat(word == 0x04030201, "In place swap gives {:08x}", word);
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH endianness unit tests")
    .description("Test AnCH endianness utility functions")
    .add("endianness", testEndianness)
    .add("byte-swap", testByteSwap)
    .add("byte-swap-in-place", testByteSwapInPlace)
    ;
}
