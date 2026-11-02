#include "threadPool.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <thread>

#include "ut/unit.hpp"
#include "ut/assert.hpp"

using anch::ThreadPool;
using namespace std::chrono_literals;

// Track the number of tasks running at the same time
struct Concurrency {
  std::atomic<int> current = 0;
  std::atomic<int> max = 0;
  std::atomic<int> done = 0;

  void run(std::chrono::milliseconds duration) {
    int cur = ++current;
    int prev = max;
    while(cur > prev && !max.compare_exchange_weak(prev, cur)) {
    }
    std::this_thread::sleep_for(duration);
    --current;
    ++done;
  }
};

class Counter {
public:
  std::atomic<int> count = 0;

  void increment(int value) {
    count += value;
  }
};

void
testRunAll() {
  ThreadPool pool(4);
  std::atomic<int> count = 0;
  for(int i = 0 ; i < 50 ; ++i) {
    anch::ut::assertTrue(pool.add([&count]() { ++count; }), "Task has not been queued");
  }
  pool.start();
  pool.awaitTermination(std::chrono::seconds(-1));
  anch::ut::assertThat(count == 50, "{} tasks have been executed instead of 50", count.load());
}

void
testMaxThreads() {
  ThreadPool pool(3);
  Concurrency conc;
  pool.start();
  for(int i = 0 ; i < 12 ; ++i) {
    pool.add([&conc]() { conc.run(20ms); });
  }
  pool.awaitTermination(std::chrono::seconds(-1));
  anch::ut::assertThat(conc.done == 12, "{} tasks have been executed instead of 12", conc.done.load());
  anch::ut::assertThat(conc.max <= 3, "{} tasks have run at the same time (max 3)", conc.max.load());
  anch::ut::assertThat(conc.max >= 2, "Tasks have not run in parallel (max {})", conc.max.load());
}

void
testNotStarted() {
  ThreadPool pool(2);
  std::atomic<int> count = 0;
  for(int i = 0 ; i < 5 ; ++i) {
    pool.add([&count]() { ++count; });
  }
  std::this_thread::sleep_for(50ms);
  anch::ut::assertThat(count == 0, "{} tasks have been executed before start", count.load());
  pool.start();
  pool.awaitTermination(std::chrono::seconds(-1));
  anch::ut::assertThat(count == 5, "{} tasks have been executed instead of 5", count.load());
}

void
testStop() {
  ThreadPool pool(2);
  Concurrency conc;
  pool.start();
  for(int i = 0 ; i < 10 ; ++i) {
    pool.add([&conc]() { conc.run(30ms); });
  }
  std::this_thread::sleep_for(10ms);
  pool.stop(); // wait for the 2 running tasks
  anch::ut::assertThat(conc.current == 0, "{} tasks are still running after stop", conc.current.load());
  int done = conc.done;
  anch::ut::assertThat(done < 10, "Every task has been executed before stop");
  std::this_thread::sleep_for(50ms);
  anch::ut::assertThat(conc.done == done, "Tasks have been executed after stop");
  pool.start(); // queued tasks are kept
  pool.awaitTermination(std::chrono::seconds(-1));
  anch::ut::assertThat(conc.done == 10, "{} tasks have been executed instead of 10", conc.done.load());
}

void
testAwaitTimeout() {
  ThreadPool pool(1);
  Concurrency conc;
  for(int i = 0 ; i < 10 ; ++i) {
    pool.add([&conc]() { conc.run(50ms); });
  }
  pool.start();
  auto start = std::chrono::steady_clock::now();
  pool.awaitTermination(120ms);
  auto elapsed = std::chrono::steady_clock::now() - start;
  anch::ut::assertThat(conc.current == 0, "{} tasks are still running after termination", conc.current.load());
  anch::ut::assertThat(conc.done >= 2 && conc.done <= 4, "{} tasks have been executed (expected 2 to 4)", conc.done.load());
  anch::ut::assertThat(elapsed < 500ms, "awaitTermination has returned after {}", std::chrono::duration_cast<std::chrono::milliseconds>(elapsed));
  // Pool can be reused
  std::atomic<int> count = 0;
  pool.add([&count]() { ++count; });
  pool.start();
  pool.awaitTermination(std::chrono::seconds(-1));
  anch::ut::assertThat(count == 1, "Pool has not been reused after termination");
}

void
testRejectWhileTerminating() {
  ThreadPool pool(1);
  std::atomic<bool> added = true;
  pool.add([&pool, &added]() {
    std::this_thread::sleep_for(50ms); // let main thread await termination
    added = pool.add([]() {});
  });
  pool.start();
  pool.awaitTermination(std::chrono::seconds(-1));
  anch::ut::assertFalse(added, "Task has been added while awaiting termination");
}

void
testTaskException() {
  ThreadPool pool(1);
  std::atomic<int> count = 0;
  pool.add([]() { throw std::runtime_error("task error"); });
  pool.add([&count]() { ++count; });
  pool.start();
  pool.awaitTermination(std::chrono::seconds(-1));
  anch::ut::assertThat(count == 1, "Worker has not survived to task exception");
}

void
testMemberAndMoveOnly() {
  ThreadPool pool(2);
  Counter counter;
  pool.add(&Counter::increment, &counter, 2);
  pool.add([&counter](std::unique_ptr<int> value) { counter.increment(*value); }, std::make_unique<int>(3));
  pool.start();
  pool.awaitTermination(std::chrono::seconds(-1));
  anch::ut::assertThat(counter.count == 5, "Counter is {} instead of 5", counter.count.load());
}

void
testDestroy() {
  Concurrency conc;
  {
    ThreadPool pool(2);
    pool.start();
    for(int i = 0 ; i < 10 ; ++i) {
      pool.add([&conc]() { conc.run(30ms); });
    }
    std::this_thread::sleep_for(10ms);
  } // running tasks are awaited, queued ones are dropped
  anch::ut::assertThat(conc.current == 0, "{} tasks are still running after destruction", conc.current.load());
  int done = conc.done;
  anch::ut::assertThat(done >= 2 && done < 10, "{} tasks have been executed (expected 2 to 9)", done);
  std::this_thread::sleep_for(50ms);
  anch::ut::assertThat(conc.done == done, "Tasks have been executed after destruction");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH TheadPool unit tests")
    .description("Test AnCH TheadPool utiliy class")
    .add("thread-pool-run-all", testRunAll)
    .add("thread-pool-max-threads", testMaxThreads)
    .add("thread-pool-not-started", testNotStarted)
    .add("thread-pool-stop", testStop)
    .add("thread-pool-await-timeout", testAwaitTimeout)
    .add("thread-pool-reject-terminating", testRejectWhileTerminating)
    .add("thread-pool-task-exception", testTaskException)
    .add("thread-pool-member-move-only", testMemberAndMoveOnly)
    .add("thread-pool-destroy", testDestroy)
    ;
}
