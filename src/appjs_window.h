#ifndef APPJS_WINDOW_H
#define APPJS_WINDOW_H
#pragma once

#include "appjs.h"
#include "base/native_window.h"
namespace appjs {

using namespace v8;

class Window : public node::ObjectWrap {

  DECLARE_NODE_OBJECT_FACTORY(Window);

  DEFINE_CPP_METHOD(OpenDevTools);
  DEFINE_CPP_METHOD(CloseDevTools);
  DEFINE_CPP_METHOD(Show);
  DEFINE_CPP_METHOD(Hide);
  DEFINE_CPP_METHOD(Destroy);
  DEFINE_CPP_METHOD(RunInBrowser);
  DEFINE_CPP_METHOD(SendSync);

};

} /* appjs */
#endif /* end of APPJS_WINDOW_H */

