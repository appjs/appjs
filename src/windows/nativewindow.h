#ifndef APPJS_NATIVE_WINDOW_H
#define APPJS_NATIVE_WINDOW_H
#pragma once

#include <node.h>
#include <Windows.h>
#include "include/cef_browser.h"
#include "includes/util.h"

namespace appjs {

class NativeWindow {

public:
  NativeWindow(char*,Settings*);
  ~NativeWindow();
  void OpenDevTools();
  void CloseDevTools();
  void show();
  void hide();
  void destroy();

  void setBrowser(CefRefPtr<CefBrowser> browser);
  void setV8Handle(v8::Handle<v8::Object> v8handle);
  CefRefPtr<CefBrowser> getBrowser();
  v8::Handle<v8::Object> getV8Handle();

  static int ScreenWidth();
  static int ScreenHeight();

private:
  CefWindowHandle handle_;
  v8::Handle<v8::Object> v8handle_;
  CefRefPtr<CefBrowser> browser_;
};

} /* appjs */
#endif /* end of APPJS_NATIVE_WINDOW_H */

