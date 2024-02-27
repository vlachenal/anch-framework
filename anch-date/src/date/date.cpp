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
#include "date/date.hpp"


using anch::date::Date;


// Static initialization +
std::mutex Date::_mutex;
// Static initialization -


//Constructors +
Date::Date(bool init) {
  if(init) {
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    auto epoch = now.time_since_epoch();
    _timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count();
    initialize(epoch); // Retrieve ms, µs, ns
    // Initialize others fields according to UTC time
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    _mutex.lock();
    initialize(localtime(&time));
    _mutex.unlock();
  }
}

Date::Date(const std::time_t& time) {
  std::chrono::time_point<std::chrono::system_clock> timePoint = std::chrono::system_clock::from_time_t(time);
  _timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(timePoint.time_since_epoch()).count();
  _mutex.lock();
  initialize(localtime(&time));
  _mutex.unlock();
}

Date::Date(const std::tm* const time) {
  _mutex.lock();
  // Copy time to not change time values
  std::tm tmpTm = tm(*time);
  std::chrono::time_point<std::chrono::system_clock> timePoint = std::chrono::system_clock::from_time_t(mktime(&tmpTm));
  _timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(timePoint.time_since_epoch()).count();
  initialize(time);
  _mutex.unlock();
}

Date::Date(const std::timespec& time) {
  std::chrono::time_point<std::chrono::system_clock> timePoint = std::chrono::system_clock::from_time_t(time.tv_sec);
  _timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(timePoint.time_since_epoch()).count() + time.tv_nsec;
  _mutex.lock();
  initialize(localtime(&time.tv_sec));
  _mutex.unlock();
  _milliseconds = static_cast<uint16_t>(time.tv_nsec % 1000000);
  _microseconds = static_cast<uint16_t>(time.tv_nsec % 1000 - _milliseconds * 1000);
  _nanoseconds = static_cast<uint16_t>(time.tv_nsec - _microseconds * 1000 - _milliseconds * 1000000);
}
//Constructors -

// Destructors +
Date::~Date() {
  // Nothing to do
}
// Destructors -


// Methods +
void
Date::initialize(const std::tm* const time) {
  _seconds = static_cast<uint16_t>(time->tm_sec);
  _minutes = static_cast<uint16_t>(time->tm_min);
  if(time->tm_isdst > 0) {
    _hours = static_cast<uint16_t>(time->tm_hour + 1);
  } else {
    _hours = static_cast<uint16_t>(time->tm_hour);
  }
  _wdays = static_cast<uint16_t>(time->tm_wday);
  _mdays = static_cast<uint16_t>(time->tm_mday);
  _ydays = static_cast<uint16_t>(time->tm_yday);
  _months = static_cast<uint16_t>(time->tm_mon);
  _years = static_cast<int32_t>(time->tm_year) + 1900;
}

void
Date::computeTimestamp() {
  _mutex.lock();
  std::tm tmpTm;
  computeTm(tmpTm);

  std::time_t tmpTime = std::mktime(&tmpTm);
  std::chrono::time_point<std::chrono::system_clock> timePoint = std::chrono::system_clock::from_time_t(tmpTime);
  _timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(timePoint.time_since_epoch()).count();
  _mutex.unlock();
}
// Methods -
