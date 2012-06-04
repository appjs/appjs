#ifndef APPJS_APP_H
#define APPJS_APP_H
#pragma once

#include "appjs.h"
#include "appjs_window.h"

namespace appjs {

using namespace v8;

class App : public node::ObjectWrap {

  DECLARE_NODE_OBJECT_FACTORY(App);
  
  DEFINE_CPP_METHOD(CreateWindow2);
  DEFINE_CPP_METHOD(ScreenWidth);
  DEFINE_CPP_METHOD(ScreenHeight);
  DEFINE_CPP_METHOD(On);
  static bool initialized_;
};

} /* appjs */

#endif /* end of APPJS_APP_H */

