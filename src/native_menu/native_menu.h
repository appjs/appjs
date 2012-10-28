#ifndef APPJS_BASE_NATIVE_MENU_H
#define APPJS_BASE_NATIVE_MENU_H
#pragma once

#include "appjs.h"
#include "include/cef_browser.h"
#include "includes/util.h"
#include <node.h>
#include <vector>

namespace appjs {

class NativeMenu {

public:
  NativeMenu(Settings* settings);
  ~NativeMenu();
  void Init(Settings* settings);
  void SetV8Handle(v8::Handle<v8::Object> v8handle);
#ifdef __LINUX__
  std::vector<GtkWidget*> GetMenu() {return menuItems_;};
  int AddSubMenu(GtkWidget*,Settings*);
#endif  

private:
  v8::Handle<v8::Object> v8handle_;

#ifdef __LINUX__
  //GtkWidget* menu_;
  std::vector<GtkWidget*> menuItems_;
#endif

};

} /* appjs */

#endif /* end of APPJS_BASE_NATIVE_MENU_H */
