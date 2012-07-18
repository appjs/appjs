#ifndef APPJS_BASE_NATIVE_WINDOW_H
#define APPJS_BASE_NATIVE_WINDOW_H
#pragma once

#include "include/cef_browser.h"
#include "includes/util.h"

namespace appjs {

typedef struct _appjs_rect {
  int left;
  int top;
  int width;
  int height;
} appjs_rect;

enum NW_STATE {
  NW_STATE_NORMAL,
  NW_STATE_MINIMIZED,
  NW_STATE_MAXIMIZED,
  NW_STATE_FULLSCREEN
};

class NativeWindow {

public:
  NativeWindow(char* url, Settings* settings);
  ~NativeWindow();
  void Init(char* url, Settings* settings);

  static int ScreenWidth();
  static int ScreenHeight();

  void Drag();
  void Drop();
  void Restore();
  void Minimize();
  void Maximize();
  void Fullscreen();
  void Show();
  void Hide();
  void Destroy();

  void Move(int top, int left, int width, int height);
  void Move(int top, int left);
  void Move(appjs_rect rect);
  void Resize(int width, int height);
  void UpdatePosition(int top, int left, int width, int height);
  void UpdatePosition(appjs_rect rect);
  void UpdatePosition();


  void SetState(NW_STATE state);
  NW_STATE GetState();
  void SetTopmost(bool ontop);
  bool GetTopmost();

  void SetLeft(int left);
  void SetTop(int top);
  void SetWidth(int width);
  void SetHeight(int height);
  int GetLeft();
  int GetTop();
  int GetWidth();
  int GetHeight();

  void SetTitle(const char* title);
  const char* GetTitle();

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

#if defined(__LINUX__)
  long GetDragHandlerId();
#endif

private:
  v8::Handle<v8::Object> v8handle_;
  CefRefPtr<CefBrowser> browser_;

  bool show_chrome;
  bool resizable;
  bool show_resize_grip;
  bool fullscreen_;
  bool topmost_;
  Settings* icons;
  bool alpha;
  double opacity;

  appjs_rect rect_;
  appjs_rect restoreRect_;
  long restoreStyle;

#if defined(__LINUX__)
  long drag_handler_id;
#endif

};

} /* appjs */

#endif /* end of APPJS_BASE_NATIVE_WINDOW_H */
