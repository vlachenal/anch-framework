/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2012 Vincent Lachenal

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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "device/cpu.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::device::CPU;


void
testFeatures() {
#if defined(__x86_64__) || defined(__i386__)
  // GCC builtins use their own CPUID detection
  __builtin_cpu_init();
  const CPU& cpu = CPU::getInstance();
  anch::ut::assertThat(cpu.isAES() == (__builtin_cpu_supports("aes") != 0), "AES: {}", cpu.isAES());
  anch::ut::assertThat(cpu.isMMX() == (__builtin_cpu_supports("mmx") != 0), "MMX: {}", cpu.isMMX());
  anch::ut::assertThat(cpu.isSSE() == (__builtin_cpu_supports("sse") != 0), "SSE: {}", cpu.isSSE());
  anch::ut::assertThat(cpu.isSSE2() == (__builtin_cpu_supports("sse2") != 0), "SSE2: {}", cpu.isSSE2());
  anch::ut::assertThat(cpu.isSSE3() == (__builtin_cpu_supports("sse3") != 0), "SSE3: {}", cpu.isSSE3());
  anch::ut::assertThat(cpu.isSSSE3() == (__builtin_cpu_supports("ssse3") != 0), "SSSE3: {}", cpu.isSSSE3());
  anch::ut::assertThat(cpu.isSSE4_1() == (__builtin_cpu_supports("sse4.1") != 0), "SSE4.1: {}", cpu.isSSE4_1());
  anch::ut::assertThat(cpu.isSSE4_2() == (__builtin_cpu_supports("sse4.2") != 0), "SSE4.2: {}", cpu.isSSE4_2());
  anch::ut::assertThat(cpu.isSSE4() == (cpu.isSSE4_1() || cpu.isSSE4_2()), "SSE4: {}", cpu.isSSE4());
#endif
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH device CPU unit tests")
    .description("Test AnCH CPU features detection")
    .add("cpu-features", testFeatures)
    ;
}
