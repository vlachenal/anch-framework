/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2013 Vincent Lachenal

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
#include "events/eventBus.hpp"
#include "events/eventHandler.hpp"

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "ut/unit.hpp"
#include "ut/assert.hpp"

using anch::events::Event;
using anch::events::EventBus;
using anch::events::Observer;
using namespace std::chrono_literals;

// Observer which records received events
class Recorder: public Observer<int> {
public:
  std::mutex mutex;
  std::vector<int> received;
  std::function<void(Recorder&, const Event<int>&)> action;

  void handle(const Event<int>& event) noexcept override {
    {
      std::lock_guard<std::mutex> lock(mutex);
      received.push_back(event.body);
    }
    if(action) {
      action(*this, event);
    }
  }

  std::vector<int> values() {
    std::lock_guard<std::mutex> lock(mutex);
    return received;
  }

  // Wait until nb events have been received (or timeout)
  bool waitFor(std::size_t nb, std::chrono::milliseconds timeout = 2s) {
    auto end = std::chrono::steady_clock::now() + timeout;
    while(std::chrono::steady_clock::now() < end) {
      if(values().size() >= nb) {
	return true;
      }
      std::this_thread::sleep_for(1ms);
    }
    return false;
  }
};

void
testFireInOrder() {
  EventBus<int> bus;
  Recorder obs;
  bus.addObserver(obs);
  for(int i = 0 ; i < 100 ; ++i) {
    bus.fireEvent(i);
  }
  anch::ut::assertTrue(obs.waitFor(100), "Events have not been received");
  std::vector<int> values = obs.values();
  for(int i = 0 ; i < 100 ; ++i) {
    anch::ut::assertThat(values[static_cast<std::size_t>(i)] == i, "Event {} received at position {}", values[static_cast<std::size_t>(i)], i);
  }
  bus.removeObserver(obs);
}

// E4: fireEvent used to wait for observers of the previous events
void
testFireDoesNotBlock() {
  EventBus<int> bus;
  Recorder obs;
  obs.action = [](Recorder&, const Event<int>&) { std::this_thread::sleep_for(100ms); };
  bus.addObserver(obs);
  auto start = std::chrono::steady_clock::now();
  for(int i = 0 ; i < 5 ; ++i) {
    bus.fireEvent(i);
  }
  auto elapsed = std::chrono::steady_clock::now() - start;
  anch::ut::assertThat(elapsed < 50ms, "fireEvent has blocked {}", std::chrono::duration_cast<std::chrono::milliseconds>(elapsed));
  bus.removeObserver(obs); // waits for the running notification
}

// E2: firing an event from an observer used to call std::terminate
void
testFireFromHandler() {
  EventBus<int> bus;
  Recorder obs;
  obs.action = [&bus](Recorder&, const Event<int>& event) {
    if(event.body < 10) {
      bus.fireEvent(event.body + 1);
    }
  };
  bus.addObserver(obs);
  bus.fireEvent(0);
  anch::ut::assertTrue(obs.waitFor(11), "Chained events have not been received");
  anch::ut::assertThat(obs.values().back() == 10, "Last event is {} instead of 10", obs.values().back());
  bus.removeObserver(obs);
}

void
testRemoveInHandler() {
  EventBus<int> bus;
  Recorder self;
  Recorder other;
  self.action = [&bus](Recorder& obs, const Event<int>&) { bus.removeObserver(obs); };
  bus.addObserver(self);
  bus.addObserver(other);
  bus.fireEvent(1);
  bus.fireEvent(2);
  anch::ut::assertTrue(other.waitFor(2), "Events have not been received");
  anch::ut::assertTrue(self.values() == std::vector<int>{1}, "Removed observer has been notified again");
  bus.removeObserver(other);
}

// E8/E9: when removeObserver returns, the observer can be destroyed
void
testRemoveWaitsNotification() {
  EventBus<int> bus;
  std::atomic<bool> handling = false;
  std::atomic<bool> handled = false;
  auto obs = std::make_unique<Recorder>();
  obs->action = [&handling, &handled](Recorder&, const Event<int>&) {
    handling = true;
    std::this_thread::sleep_for(100ms);
    handled = true;
  };
  bus.addObserver(*obs);
  bus.fireEvent(1);
  while(!handling) {
    std::this_thread::yield();
  }
  bus.removeObserver(*obs);
  anch::ut::assertTrue(handled, "removeObserver has returned while observer was handling an event");
  obs.reset();
  bus.fireEvent(2); // must not notify the destroyed observer (checked by ASan)
}

void
testDestructorNotifiesQueue() {
  Recorder obs;
  {
    EventBus<int> bus;
    obs.action = [](Recorder&, const Event<int>&) { std::this_thread::sleep_for(1ms); };
    bus.addObserver(obs);
    for(int i = 0 ; i < 20 ; ++i) {
      bus.fireEvent(i);
    }
  }
  anch::ut::assertThat(obs.values().size() == 20, "{} events have been notified instead of 20", obs.values().size());
}

void
testReleaseHeapBus() {
  Recorder obs;
  EventBus<int>* bus = new EventBus<int>();
  bus->addObserver(obs);
  bus->fireEvent(1);
  bus->release();
  anch::ut::assertTrue(obs.values() == std::vector<int>{1}, "Event has not been notified before release");
}

// Handler using global bus
template<typename D>
class EventHandlerIface: public anch::events::EventHandler<int,D> {
public:
  EventHandlerIface(): anch::events::EventHandler<int,D>(&D::onMessage, true) {}
  virtual void onMessage(const Event<int>& event) noexcept = 0;
};

class Handler: public EventHandlerIface<Handler> {
public:
  std::atomic<int> sum = 0;
  ~Handler() {
    unsubscribe(); // Handler is destroyed before EventHandler
  }
  void onMessage(const Event<int>& event) noexcept override {
    sum += event.body;
  }
};

void
testGlobalBusHandler() {
  Handler handler;
  EventBus<int>::FireEvent(2);
  EventBus<int>::FireEvent(3);
  auto end = std::chrono::steady_clock::now() + 2s;
  while(handler.sum != 5 && std::chrono::steady_clock::now() < end) {
    std::this_thread::sleep_for(1ms);
  }
  anch::ut::assertThat(handler.sum == 5, "Handler sum is {} instead of 5", handler.sum.load());
  handler.unsubscribe();
  EventBus<int>::FireEvent(10);
  std::this_thread::sleep_for(20ms);
  anch::ut::assertThat(handler.sum == 5, "Unsubscribed handler has been notified (sum {})", handler.sum.load());
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH event bus unit tests")
    .description("Test AnCH event bus")
    .add("bus-fire-in-order", testFireInOrder)
    .add("bus-fire-does-not-block", testFireDoesNotBlock)
    .add("bus-fire-from-handler", testFireFromHandler)
    .add("bus-remove-in-handler", testRemoveInHandler)
    .add("bus-remove-waits", testRemoveWaitsNotification)
    .add("bus-destructor-notifies-queue", testDestructorNotifiesQueue)
    .add("bus-release-heap", testReleaseHeapBus)
    .add("bus-global-handler", testGlobalBusHandler)
    ;
}
