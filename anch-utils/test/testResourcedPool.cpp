/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2016 Vincent Lachenal

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
#include "resourcePool.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using namespace std::chrono_literals;

struct Config {
};

// Resource with creation/destruction counters
class Res {
public:
  static std::atomic<int> created;
  static std::atomic<int> destroyed;
  int id;
  bool valid = true;
  std::atomic<bool> inUse = false;

  Res(const Config&): id(created.fetch_add(1)) {
  }

  ~Res() {
    destroyed.fetch_add(1);
  }

  bool isValid() const {
    return valid;
  }
};

std::atomic<int> Res::created;
std::atomic<int> Res::destroyed;

static void
resetCounters() {
  Res::created = 0;
  Res::destroyed = 0;
}

using Pool = anch::ResourcePool<Res, Config>;

static_assert(!std::is_copy_constructible_v<anch::PoolableResource<Res>>, "PoolableResource must not be copyable");
static_assert(std::is_nothrow_move_constructible_v<anch::PoolableResource<Res>>, "PoolableResource must be movable");

// Resource factory which can fail
static std::atomic<bool> failCreation = false;

std::shared_ptr<Res>
makeRes(const Config& config) {
  if(failCreation) {
    throw std::runtime_error("creation failure");
  }
  return std::make_shared<Res>(config);
}

using FailPool = anch::ResourcePool<Res, Config, makeRes>;

// Anch unit tests +
void
testReuse() {
  resetCounters();
  Config config;
  Pool pool(config, 2);
  int id;
  {
    auto res = pool.borrowResource();
    id = res.get().id;
  }
  {
    auto res = pool.borrowResource();
    anch::ut::assertThat(res.get().id == id, "Resource {} has not been reused (got {})", id, res.get().id);
  }
  anch::ut::assertThat(Res::created == 1, "{} resources have been created instead of 1", Res::created.load());
}

void
testInitSize() {
  resetCounters();
  Config config;
  Pool pool(config, 3, 5);
  anch::ut::assertThat(Res::created == 3, "{} resources have been created instead of 3 (max size)", Res::created.load());
  auto r1 = pool.borrowResource();
  auto r2 = pool.borrowResource();
  auto r3 = pool.borrowResource();
  anch::ut::assertThat(Res::created == 3, "{} resources have been created instead of 3", Res::created.load());
}

void
testMaxSizeZero() {
  Config config;
  try {
    Pool pool(config, 0);
    anch::ut::fail("Pool with maximum size 0 has been created");
  } catch(const std::invalid_argument&) {
    // expected
  }
}

void
testTimeout() {
  resetCounters();
  Config config;
  Pool pool(config, 1, 0, 50ms);
  auto res = pool.borrowResource();
  auto start = std::chrono::steady_clock::now();
  try {
    auto other = pool.borrowResource();
    anch::ut::fail("Second resource has been borrowed from a pool of 1");
  } catch(const std::exception& e) { // TimeoutException must be catchable as std::exception
    auto elapsed = std::chrono::steady_clock::now() - start;
    anch::ut::assertTrue(dynamic_cast<const anch::TimeoutException*>(&e) != nullptr, "Exception is not a TimeoutException");
    anch::ut::assertThat(elapsed >= 50ms, "Timeout raised after {} instead of 50ms",
			 std::chrono::duration_cast<std::chrono::milliseconds>(elapsed));
  }
}

void
testWakeUp() {
  resetCounters();
  Config config;
  Pool pool(config, 1, 0, 2s);
  auto start = std::chrono::steady_clock::now();
  std::thread holder([res = pool.borrowResource()]() mutable {
    std::this_thread::sleep_for(100ms);
  });
  auto res = pool.borrowResource(); // has to wait for holder release
  auto elapsed = std::chrono::steady_clock::now() - start;
  holder.join();
  anch::ut::assertThat(elapsed >= 100ms && elapsed < 1s, "Resource borrowed after {} (expected about 100ms)",
		       std::chrono::duration_cast<std::chrono::milliseconds>(elapsed));
}

void
testInvalid() {
  resetCounters();
  Config config;
  Pool pool(config, 1);
  int id;
  {
    auto res = pool.borrowResource();
    id = res.get().id;
    res.get().valid = false;
  }
  anch::ut::assertThat(Res::destroyed == 1, "Invalid resource has not been destroyed ({} destroyed)", Res::destroyed.load());
  auto res = pool.borrowResource();
  anch::ut::assertThat(res.get().id != id, "Invalid resource {} has been reused", id);
  anch::ut::assertThat(Res::created == 2, "{} resources have been created instead of 2", Res::created.load());
}

void
testMove() {
  resetCounters();
  Config config;
  Pool pool(config, 1, 0, 10ms);
  {
    auto res = pool.borrowResource();
    auto moved = std::move(res);
  }
  // Resource has to be returned only once: the second borrow has to fail
  auto res = pool.borrowResource();
  try {
    auto other = pool.borrowResource();
    anch::ut::fail("Moved resource has been returned twice");
  } catch(const anch::TimeoutException&) {
    // expected
  }
}

void
testCreationFailure() {
  resetCounters();
  Config config;
  FailPool pool(config, 1, 0, 50ms);
  failCreation = true;
  try {
    auto res = pool.borrowResource();
    anch::ut::fail("Resource creation failure has not been raised");
  } catch(const std::runtime_error&) {
    // expected
  }
  failCreation = false;
  // Slot reserved by the failed creation has to be released
  auto res = pool.borrowResource();
  anch::ut::assertThat(Res::created == 1, "{} resources have been created instead of 1", Res::created.load());
}

void
testOutlivePool() {
  resetCounters();
  Config config;
  auto pool = std::make_unique<Pool>(config, 2, 2);
  {
    auto res = pool->borrowResource();
    pool.reset();
    anch::ut::assertThat(Res::destroyed == 1, "Available resource has not been destroyed with pool ({} destroyed)", Res::destroyed.load());
  }
  anch::ut::assertThat(Res::destroyed == 2, "Borrowed resource has not been destroyed on release ({} destroyed)", Res::destroyed.load());
}

void
testConcurrent() {
  resetCounters();
  Config config;
  constexpr std::size_t MAX_SIZE = 4;
  constexpr int NB_THREADS = 16;
  constexpr int NB_BORROWS = 500;
  Pool pool(config, MAX_SIZE, 0, 60s); // safety timeout: the test fails instead of hanging
  std::atomic<int> sharedUse = 0;
  std::atomic<int> errors = 0;
  std::atomic<std::size_t> concurrent = 0;
  std::atomic<std::size_t> maxConcurrent = 0;
  std::vector<std::thread> threads;
  for(int i = 0 ; i < NB_THREADS ; ++i) {
    threads.emplace_back([&]() {
      for(int j = 0 ; j < NB_BORROWS ; ++j) {
	try {
	  auto res = pool.borrowResource();
	  if(res.get().inUse.exchange(true)) {
	    ++sharedUse;
	  }
	  std::size_t current = ++concurrent;
	  std::size_t max = maxConcurrent;
	  while(current > max && !maxConcurrent.compare_exchange_weak(max, current)) {
	  }
	  std::this_thread::yield();
	  --concurrent;
	  res.get().inUse = false;
	} catch(...) {
	  ++errors;
	}
      }
    });
  }
  for(std::thread& thread : threads) {
    thread.join();
  }
  anch::ut::assertThat(errors == 0, "{} borrow errors", errors.load());
  anch::ut::assertThat(sharedUse == 0, "Resources have been used by several threads at the same time {} times", sharedUse.load());
  anch::ut::assertThat(maxConcurrent <= MAX_SIZE, "{} resources have been used at the same time (max {})", maxConcurrent.load(), MAX_SIZE);
  anch::ut::assertThat(Res::created <= static_cast<int>(MAX_SIZE), "{} resources have been created (max {})", Res::created.load(), MAX_SIZE);
}

void
testFifo() {
  resetCounters();
  Config config;
  Pool pool(config, 1, 0, 60s);
  std::vector<int> order;
  std::mutex orderMutex;
  std::vector<std::thread> threads;
  {
    auto res = pool.borrowResource(); // every borrower waits
    for(int i = 0 ; i < 3 ; ++i) {
      threads.emplace_back([&pool, &order, &orderMutex, i]() {
	auto borrowed = pool.borrowResource();
	std::lock_guard<std::mutex> lock(orderMutex);
	order.push_back(i);
      });
      std::this_thread::sleep_for(300ms); // borrower i is waiting before borrower i + 1
    }
    // A new borrower can not overtake waiting ones: it times out
    pool.setTimeout(50ms);
    try {
      auto late = pool.borrowResource();
      anch::ut::fail("Late borrower has overtaken waiting borrowers");
    } catch(const anch::TimeoutException&) {
      // expected
    }
  }
  for(std::thread& thread : threads) {
    thread.join();
  }
  anch::ut::assertThat(order == std::vector<int>({0, 1, 2}), "Borrowers have not been served in arrival order");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH resource pool unit tests")
    .description("Test AnCH resource pool utiliy class")
    .add("res-pool-reuse", testReuse)
    .add("res-pool-init-size", testInitSize)
    .add("res-pool-max-size-zero", testMaxSizeZero)
    .add("res-pool-timeout", testTimeout)
    .add("res-pool-wake-up", testWakeUp)
    .add("res-pool-invalid", testInvalid)
    .add("res-pool-move", testMove)
    .add("res-pool-creation-failure", testCreationFailure)
    .add("res-pool-outlive", testOutlivePool)
    .add("res-pool-concurrent", testConcurrent)
    .add("res-pool-fifo", testFifo)
    ;
}
// Anch unit tests -
