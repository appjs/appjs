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
  DEFINE_CPP_METHOD(Minimize);
  DEFINE_CPP_METHOD(Maximize);
  DEFINE_CPP_METHOD(Restore);
  DEFINE_CPP_METHOD(Drag);
  DEFINE_CPP_METHOD(Show);
  DEFINE_CPP_METHOD(Hide);
  DEFINE_CPP_METHOD(Destroy);
  DEFINE_CPP_METHOD(RunInBrowser);
  DEFINE_CPP_METHOD(SendSync);
  DEFINE_CPP_METHOD(SetPosition);
  DEFINE_CPP_METHOD(SetSize);
  DEFINE_CPP_ACCESSOR(Left);
  DEFINE_CPP_ACCESSOR(Top);
  DEFINE_CPP_ACCESSOR(Height);
  DEFINE_CPP_ACCESSOR(Width);

#if defined(__WIN__)
  DEFINE_CPP_METHOD(Style);
#endif
};

} /* appjs */
#endif /* end of APPJS_WINDOW_H */

