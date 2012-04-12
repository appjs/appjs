#ifndef APPJS_WINDOW_H
#define APPJS_WINDOW_H
#pragma once

#include "appjs.h"

// GTK+ binding for linux
#if defined(__linux__)

#include "linux/mainwindow.h"
//#include "linux/appjs_menu.h"
//#include "linux/appjs_status_icon.h"

#endif /* end of linux */

namespace appjs {

using namespace v8;

class Window {

  DECLARE_NODE_OBJECT (Window)

  static Handle<Value> Show (const Arguments& args);
  static Handle<Value> Hide (const Arguments& args);

};

} /* appjs */
#endif /* end of APPJS_WINDOW_H */

