#include "events/observable.hpp"

#include <atomic>
#include <chrono>
#include <functional>
#include <latch>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "ut/unit.hpp"
#include "ut/assert.hpp"

using anch::events::Event;
using anch::events::Observable;
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
};

void
testNotify() {
  Observable<int> observable;
  Recorder obs1;
  Recorder obs2;
  anch::ut::assertTrue(observable.addObserver(obs1), "Observer has not been added");
  observable.notifyObservers(1);
  anch::ut::assertTrue(observable.addObserver(obs2), "Observer has not been added");
  anch::ut::assertFalse(observable.addObserver(obs2), "Observer has been added twice");
  observable.notifyObservers(2);
  observable.removeObserver(obs1);
  observable.notifyObservers(3);
  anch::ut::assertTrue(obs1.values() == std::vector<int>{1, 2}, "Unexpected events for first observer");
  anch::ut::assertTrue(obs2.values() == std::vector<int>{2, 3}, "Unexpected events for second observer");
}

void
testHeaders() {
  Observable<int> observable;
  std::string header;
  Recorder obs;
  obs.action = [&header](Recorder&, const Event<int>& event) { header = event.headers.at("key"); };
  observable.addObserver(obs);
  observable.notifyObservers(1, {{"key", "value"}});
  anch::ut::assertThat(header == "value", "Header is '{}' instead of 'value'", header);
}

void
testRegistrationOrder() {
  Observable<int> observable;
  std::vector<int> order;
  std::vector<Recorder> observers(5);
  for(std::size_t i = 0 ; i < observers.size() ; ++i) {
    observers[i].action = [&order, i](Recorder&, const Event<int>&) { order.push_back(static_cast<int>(i)); };
  }
  for(std::size_t i : {3uz, 0uz, 4uz, 1uz, 2uz}) {
    observable.addObserver(observers[i]);
  }
  observable.notifyObservers(1);
  anch::ut::assertTrue(order == std::vector<int>{3, 0, 4, 1, 2}, "Observers are not notified in registration order");
}

// E7: an observer which removes itself in handle used to deadlock
void
testRemoveInHandle() {
  Observable<int> observable;
  Recorder self;
  Recorder other;
  self.action = [&observable](Recorder& obs, const Event<int>&) { observable.removeObserver(obs); };
  observable.addObserver(self);
  observable.addObserver(other);
  observable.notifyObservers(1);
  observable.notifyObservers(2);
  anch::ut::assertTrue(self.values() == std::vector<int>{1}, "Removed observer has been notified again");
  anch::ut::assertTrue(other.values() == std::vector<int>{1, 2}, "Other observer has missed events");
}

// An observer removed by a previous observer during a notification is not notified anymore
void
testRemoveOtherInHandle() {
  Observable<int> observable;
  Recorder first;
  Recorder second;
  first.action = [&observable, &second](Recorder&, const Event<int>&) { observable.removeObserver(second); };
  observable.addObserver(first);
  observable.addObserver(second);
  observable.notifyObservers(1);
  anch::ut::assertTrue(second.values().empty(), "Observer removed during notification has been notified");
}

void
testAddAndNotifyInHandle() {
  Observable<int> observable;
  Recorder added;
  Recorder obs;
  obs.action = [&observable, &added](Recorder&, const Event<int>& event) {
    if(event.body == 1) {
      observable.addObserver(added);
      observable.notifyObservers(2); // reentrant notification
    }
  };
  observable.addObserver(obs);
  observable.notifyObservers(1);
  anch::ut::assertTrue(obs.values() == std::vector<int>{1, 2}, "Reentrant notification has not been received");
  anch::ut::assertTrue(added.values() == std::vector<int>{2}, "Observer added in handle has not been notified");
}

// removeObserver waits for notifications of other threads: the observer can be destroyed after
void
testRemoveWaitsNotification() {
  Observable<int> observable;
  std::atomic<bool> handling = false;
  std::atomic<bool> handled = false;
  Recorder obs;
  obs.action = [&handling, &handled](Recorder&, const Event<int>&) {
    handling = true;
    std::this_thread::sleep_for(100ms);
    handled = true;
  };
  observable.addObserver(obs);
  std::thread notifier([&observable]() { observable.notifyObservers(1); });
  while(!handling) {
    std::this_thread::yield();
  }
  observable.removeObserver(obs);
  anch::ut::assertTrue(handled, "removeObserver has returned while observer was handling an event");
  notifier.join();
}

void
testConcurrent() {
  Observable<int> observable;
  std::vector<Recorder> observers(8);
  std::atomic<bool> stop = false;
  std::latch start(3);
  std::thread notifier([&]() {
    start.arrive_and_wait();
    for(int i = 0 ; i < 2000 ; ++i) {
      observable.notifyObservers(i);
    }
    stop = true;
  });
  std::thread adder([&]() {
    start.arrive_and_wait();
    while(!stop) {
      for(Recorder& obs : observers) {
	observable.addObserver(obs);
      }
    }
  });
  std::thread remover([&]() {
    start.arrive_and_wait();
    while(!stop) {
      for(Recorder& obs : observers) {
	observable.removeObserver(obs);
      }
    }
  });
  notifier.join();
  adder.join();
  remover.join();
  for(Recorder& obs : observers) {
    std::vector<int> values = obs.values();
    for(std::size_t i = 1 ; i < values.size() ; ++i) {
      anch::ut::assertTrue(values[i - 1] < values[i], "Events have not been received in order");
    }
  }
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH observable unit tests")
    .description("Test AnCH observers/observable classes")
    .add("observable-notify", testNotify)
    .add("observable-headers", testHeaders)
    .add("observable-order", testRegistrationOrder)
    .add("observable-remove-in-handle", testRemoveInHandle)
    .add("observable-remove-other-in-handle", testRemoveOtherInHandle)
    .add("observable-add-notify-in-handle", testAddAndNotifyInHandle)
    .add("observable-remove-waits", testRemoveWaitsNotification)
    .add("observable-concurrent", testConcurrent)
    ;
}
