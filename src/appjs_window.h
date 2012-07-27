#ifndef APPJS_WINDOW_H
#define APPJS_WINDOW_H
#pragma once

#include "appjs.h"
#include "base/native_window.h"

namespace appjs {

using namespace v8;

#define WINDOW_ACCESSOR(propName, getType, setType) \
  DECLARE_CPP_ACCESSOR(Window, propName, getType, setType)

class Window : public node::ObjectWrap {
  DECLARE_NODE_OBJECT_FACTORY(Window);
  DEFINE_CPP_METHOD(OpenDevTools);
  DEFINE_CPP_METHOD(CloseDevTools);
  DEFINE_CPP_METHOD(Fullscreen);
  DEFINE_CPP_METHOD(Minimize);
  DEFINE_CPP_METHOD(Maximize);
  DEFINE_CPP_METHOD(Restore);
  DEFINE_CPP_METHOD(Drag);
  DEFINE_CPP_METHOD(Drop);
  DEFINE_CPP_METHOD(Show);
  DEFINE_CPP_METHOD(Hide);
  DEFINE_CPP_METHOD(Destroy);
  DEFINE_CPP_METHOD(RunInBrowser);
  DEFINE_CPP_METHOD(SendSync);
  DEFINE_CPP_METHOD(Move);
  DEFINE_CPP_METHOD(Resize);
  DEFINE_CPP_ACCESSOR(Left);
  DEFINE_CPP_ACCESSOR(Top);
  DEFINE_CPP_ACCESSOR(Height);
  DEFINE_CPP_ACCESSOR(Width);
  DEFINE_CPP_ACCESSOR(Title);
  DEFINE_CPP_ACCESSOR(Topmost);
  DEFINE_CPP_ACCESSOR(State);
  DEFINE_CPP_ACCESSOR(Resizable);
  DEFINE_CPP_ACCESSOR(ShowChrome);
  DEFINE_CPP_ACCESSOR(Alpha);
  DEFINE_CPP_ACCESSOR(Opacity);
  DEFINE_CPP_ACCESSOR(AutoResize);

#if defined(__WIN__)
  DEFINE_CPP_METHOD(Style);
#endif
};

} /* appjs */
#endif /* end of APPJS_WINDOW_H */

