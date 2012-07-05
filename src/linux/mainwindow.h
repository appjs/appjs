#ifndef APPJS_MAIN_WINDOW_H
#define APPJS_MAIN_WINDOW_H
#pragma once

#include <node.h>
#include "include/cef_browser.h"
#include "includes/util.h"

namespace appjs {

class MainWindow {

public:
  MainWindow(char*,Settings*);
  ~MainWindow();

  void OpenDevTools();
  void CloseDevTools();
  void show();
  void hide();
  void destroy();
  void setV8Handle(v8::Handle<v8::Object>);
  v8::Handle<v8::Object> getV8Handle();

  static int ScreenWidth();
  static int ScreenHeight();

private:
  CefWindowHandle window;
  v8::Handle<v8::Object> jsObj;

};

} /* appjs */
#endif /* end of APPJS_MAIN_WINDOW_H */
