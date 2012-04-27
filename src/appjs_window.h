#ifndef APPJS_WINDOW_H
#define APPJS_WINDOW_H
#pragma once

#include "appjs.h"

// GTK+ binding for linux
#if defined(__LINUX__)

#include "linux/mainwindow.h"

// Windows necessary files
#elif defined(__WIN__)

#include "windows/mainwindow.h"

#endif


namespace appjs {

using namespace v8;

class Window : public node::ObjectWrap {

  DECLARE_NODE_OBJECT_FACTORY(Window);

  DEFINE_CPP_METHOD(Show);
  DEFINE_CPP_METHOD(Hide);
  DEFINE_CPP_METHOD(Destroy);
  DEFINE_CPP_METHOD(RunInBrowser);

};

} /* appjs */
#endif /* end of APPJS_WINDOW_H */

