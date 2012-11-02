#ifndef APPJS_APP_H
#define APPJS_APP_H
#pragma once

#include "appjs.h"
#include "appjs_window.h"
#include "appjs_menu.h"
#include "appjs_status_icon.h"

namespace appjs {

using namespace v8;

class App : public node::ObjectWrap {
  DEFINE_OBJECT_FACTORY(App);
  DEFINE_CLASS_FUNCTION(ScreenWidth);
  DEFINE_CLASS_FUNCTION(ScreenHeight);
  DEFINE_PROTOTYPE_METHOD(CreateWindow2);
  DEFINE_PROTOTYPE_METHOD(CreateMenu2);
  DEFINE_PROTOTYPE_METHOD(CreateStatusIcon);

  static bool initialized_;
};

} /* appjs */

#endif /* end of APPJS_APP_H */

