#ifndef APPJS_MENU_H
#define APPJS_MENU_H
#pragma once

#include "appjs.h"
#include "native_menu/native_menu.h"

namespace appjs {

using namespace v8;

class Menu : public node::ObjectWrap {
  DEFINE_OBJECT_FACTORY(Menu);
};

} /* appjs */
#endif /* end of APPJS_MENU_H */