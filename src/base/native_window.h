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

  static int ScreenWidth();
  static int ScreenHeight();

  void Drag();
  void Minimize();
  void Maximize();
  void Restore();
  void Show();
  void Hide();
  void Destroy();

  void SetPosition(int top, int left, int width, int height);
  void SetPosition(int top, int left);
  void SetSize(int width, int height);
  void UpdatePosition(int top, int left, int width, int height);
  void UpdatePosition();

  void SetLeft(int left);
  void SetTop(int top);
  void SetWidth(int width);
  void SetHeight(int height);
  int GetLeft();
  int GetTop();
  int GetWidth();
  int GetHeight();

  void OpenDevTools();
  void CloseDevTools();
  void RunInBrowser(char* script);

  void SetBrowser(CefRefPtr<CefBrowser> browser);
  void SetV8Handle(v8::Handle<v8::Object> v8handle);
  CefRefPtr<CefBrowser> GetBrowser();
  v8::Handle<v8::Object> GetV8Handle();
  CefWindowHandle handle_;
  bool auto_resize;

#if defined(__WIN__)
  void SetStyle(long style, bool extended);
  long GetStyle(bool extended);
#endif

private:
  v8::Handle<v8::Object> v8handle_;
  CefRefPtr<CefBrowser> browser_;

  int width_;
  int height_;
  int left_;
  int top_;
  bool show_chrome;
  bool resizable;
  bool show_resize_grip;
  bool fullscreen;
  Settings* icons;
  bool alpha;
  double opacity;
};

} /* appjs */

#endif /* end of APPJS_BASE_NATIVE_WINDOW_H */
