/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
//! [example]
#include "events/eventBus.hpp"
#include "events/eventHandler.hpp"
#include "events/observable.hpp"
#include "events/observer.hpp"

#include <iostream>
#include <string>

// Observer: handles events of one type
class Printer: public anch::events::Observer<std::string> {
public:
  virtual void handle(const anch::events::Event<std::string>& event) noexcept override {
    std::cout << "observer: " << event.body << std::endl;
  }
};

struct Created {
  std::string name;
};

struct Deleted {
  int id;
};

// Event handlers: one class handles several event types
class Audit: public anch::events::EventHandler<Created, Audit>, public anch::events::EventHandler<Deleted, Audit> {
public:
  Audit(): anch::events::EventHandler<Created, Audit>(&Audit::onCreated),
           anch::events::EventHandler<Deleted, Audit>(&Audit::onDeleted) {
  }

  void onCreated(const anch::events::Event<Created>& event) {
    std::cout << "created: " << event.body.name << " by " << event.headers.at("user") << std::endl;
  }

  void onDeleted(const anch::events::Event<Deleted>& event) {
    std::cout << "deleted: " << event.body.id << std::endl;
  }
};

int
main() {
  // Synchronous notification
  Printer printer;
  anch::events::Observable<std::string> observable;
  observable.addObserver(printer);
  observable.notifyObservers("hello");
  observable.removeObserver(printer);

  Audit audit;
  anch::events::Observable<Created> created;
  created.addObserver(audit);
  created.notifyObservers(Created{"Rex"}, {{"user", "admin"}});

  // Asynchronous notification: events are delivered by the bus thread, in firing order
  {
    anch::events::EventBus<Deleted> bus;
    bus.addObserver(audit);
    bus.fireEvent(Deleted{1});
    bus.fireEvent(Deleted{2});
  } // bus destructor delivers queued events
  return 0;
}
//! [example]
