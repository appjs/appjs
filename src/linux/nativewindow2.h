#ifndef APPJS_NATIVE_WINDOW_H
#define APPJS_NATIVE_WINDOW_H
#pragma once

#include <node.h>
#include "include/cef_browser.h"
#include "includes/util.h"

namespace appjs {
/*
class LinuxNativeWindow {

public:
  LinuxNativeWindow(char*,Settings*);
  ~LinuxNativeWindow();
  virtual void init() = 0;

  virtual void show() = 0;
  virtual void hide() = 0;
  virtual void destroy() = 0;

  virtual static int ScreenWidth() = 0;
  virtual static int ScreenHeight() = 0;

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

};
*/
} /* appjs */

#endif /* end of APPJS_BASE_NATIVE_WINDOW_H */
