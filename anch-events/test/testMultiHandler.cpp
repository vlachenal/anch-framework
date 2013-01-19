#include "events/observable.hpp"

#include <iostream>

using std::string;
using std::cout;
using std::endl;

using anch::events::Observable;
using anch::events::Observer;

class Handler: public Observer<string>, public Observer<int> {

public:
  Handler() {
    // Nothing to do
  }

  virtual ~Handler() {
    // Nothing to do
  }

public:
  virtual void notify(const string& str) {
    cout << "Observer<string> " << getIdentifier() << " received " << str << endl;
  }

  virtual void notify(const int& integer) {
    cout << "Observer<int> " << getIdentifier() << " received " << integer << endl;
  }

};


int
main(void) {
  cout << "Enter in test" << endl;

  Observable<string> obsStr;
  Observable<int> obsInt;

  cout << "Exit test" << endl;
  return 0;
}
