#ifndef APPJS_WINDOW_H
#define APPJS_WINDOW_H
#pragma once

#include "appjs.h"

// GTK+ binding for linux
#if defined(__LINUX__)

#include "linux/mainwindow.h"

#endif /* end of linux */

// GTK+ binding for linux
#if defined(__WIN__)

#include "linux/mainwindow.h"

#endif /* end of linux */


namespace appjs {

using namespace v8;

class Window : public node::ObjectWrap {

  DECLARE_NODE_OBJECT_FACTORY(Window);
  
  DEFINE_CPP_METHOD(Show);
  DEFINE_CPP_METHOD(Hide);

};

} /* appjs */
#endif /* end of APPJS_WINDOW_H */

