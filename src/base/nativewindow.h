#ifndef APPJS_BASE_NATIVE_WINDOW_H
#define APPJS_BASE_NATIVE_WINDOW_H
#pragma once

#include "include/cef_browser.h"
#include "includes/util.h"

namespace appjs {

class NativeWindow {

public:
  NativeWindow(char* url,Settings*);
  ~NativeWindow();
  void init(char*,Settings*);

  void show();
  void hide();
  void destroy();

  static int ScreenWidth();
  static int ScreenHeight();

  void openDevTools();
  void closeDevTools();

  void setBrowser(CefRefPtr<CefBrowser> browser);
  void setV8Handle(v8::Handle<v8::Object> v8handle);
  CefRefPtr<CefBrowser> getBrowser();
  v8::Handle<v8::Object> getV8Handle();

private:
  v8::Handle<v8::Object> v8handle_;
  CefRefPtr<CefBrowser> browser_;

  char* url;
  int width;
  int height;
  int x;
  int y;
  double opacity;
  bool show_chrome;
  bool resizable;
  bool show_resize_grip;
  bool auto_resize;
  bool fullscreen;
  Settings icons;

};

} /* appjs */

#endif /* end of APPJS_BASE_NATIVE_WINDOW_H */
