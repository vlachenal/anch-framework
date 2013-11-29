#include "threadPool.hpp"

#include <iostream>

using anch::ThreadPool;

void
sleepFor2s(int index) {
  std::cout << "Begin thread " << index << " execution in thread " << std::this_thread::get_id() << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "End thread " << index << " execution in thread " << std::this_thread::get_id() << std::endl;
}

int
main(void) {
  std::cout << "Begin ThreadPool unitary test in thread " << std::this_thread::get_id() << std::endl;
  ThreadPool pool(4);
  pool.start();
  for(int i = 0 ; i < 10 ; i++) {
    std::cout << "Add a new thread in queue " << i << std::endl;
    pool.add(sleepFor2s, i);
    std::cout << "New thread has been added " << i << std::endl;
  }
  std::this_thread::sleep_for(std::chrono::seconds(5));
  pool.stop();
  std::cout << "Exit ThreadPool unitary test in thread " << std::this_thread::get_id() << std::endl;
  return 0;
}
