// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef __FX_STORE_H__
#define __FX_STORE_H__

#include <dlib/threads.h>

namespace FX {

template <class T> class Store {
public:
  Store() {
    _mutex = new dlib::mutex();
    _signaler = new dlib::signaler(*_mutex);
  }
  ~Store() {
    delete _signaler;
    delete _mutex;
  }
  void Wait() { _signaler->wait(); }
  T Get() { return _v; }
  void Set(const T &v) {
    _v = v;
    _signaler->broadcast();
  }

private:
  T _v;
  dlib::mutex *_mutex;
  dlib::signaler *_signaler;
};
} // namespace FX

#endif
