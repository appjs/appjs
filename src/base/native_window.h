#ifndef APPJS_BASE_NATIVE_WINDOW_H
#define APPJS_BASE_NATIVE_WINDOW_H
#pragma once

#include "include/cef_browser.h"
#include "includes/util.h"

namespace appjs {

class NativeWindow {

public:
  NativeWindow(char* url, Settings* settings);
  ~NativeWindow();
  void Init(char* url, Settings* settings);

  void Show();
  void Hide();
  void Destroy();

  static int ScreenWidth();
  static int ScreenHeight();

  void OpenDevTools();
  void CloseDevTools();
  void RunInBrowser(char* script);

  void SetBrowser(CefRefPtr<CefBrowser> browser);
  void SetV8Handle(v8::Handle<v8::Object> v8handle);
  CefRefPtr<CefBrowser> GetBrowser();
  v8::Handle<v8::Object> GetV8Handle();
  CefWindowHandle handle_;
  bool auto_resize;

private:
  v8::Handle<v8::Object> v8handle_;
  CefRefPtr<CefBrowser> browser_;

  int width;
  int height;
  int x;
  int y;
  double opacity;
  bool show_chrome;
  bool resizable;
  bool show_resize_grip;
  bool fullscreen;
  Settings* icons;
};

} /* appjs */

#endif /* end of APPJS_BASE_NATIVE_WINDOW_H */
