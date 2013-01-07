#ifndef APPJS_BASE_NATIVE_WINDOW_H
#define APPJS_BASE_NATIVE_WINDOW_H
#pragma once

#include "appjs.h"
#include "include/cef_browser.h"
#include "includes/util.h"
#include "native_menu/native_menu.h"
#include <node.h>


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

enum NW_ICONSIZE {
  NW_ICONSIZE_SMALLER,
  NW_ICONSIZE_SMALL,
  NW_ICONSIZE_BIG,
  NW_ICONSIZE_BIGGER
};

enum NW_DIALOGTYPE {
  NW_DIALOGTYPE_FILE_SAVE,
  NW_DIALOGTYPE_FILE_OPEN,
  NW_DIALOGTYPE_FONT,
  NW_DIALOGTYPE_COLOR
};

class NativeWindow {

public:
  NativeWindow(Settings* settings);
  ~NativeWindow();
  void Init(char* url, Settings* settings);

  static int ScreenWidth();
  static int ScreenHeight();
  //static void SetAppIcon(NW_ICONSIZE size, char* path);
  static NativeWindow* GetWindow(CefWindowHandle handle);
  static NativeWindow* GetWindow(CefRefPtr<CefBrowser> browser);

  void SetIcon(NW_ICONSIZE size, TCHAR* path);
  void Emit(v8::Handle<v8::Value>* args,int length);
  void Emit(const char* event);
  void Emit(const char* event, v8::Handle<v8::Value> arg);
  void Emit(const char* event, v8::Handle<v8::Value> arg1, v8::Handle<v8::Value> arg2);
  void Emit(const char* event, int arg1, int arg2);
  void Emit(const char* event, const int arg1, const int arg2, const int arg3);
  long JSResult();
  void PrepareClose();
  bool IsClosed();

  void Drag();
  void Drop();
  void Restore();
  void Minimize();
  void Maximize();
  void Fullscreen();
  void Show();
  void Focus();
  void Hide();
  void Destroy();

  void Move(int left, int top, int width, int height);
  void Move(int left, int top);
  void Move(appjs_rect rect);
  void Resize(int width, int height);
  void UpdatePosition(int left, int top, int width, int height);
  void UpdatePosition(appjs_rect rect);
  void UpdatePosition();

  void SetState(NW_STATE state);
  NW_STATE GetState();
  void SetTopmost(bool ontop);
  bool GetTopmost();
  void SetShowChrome(bool showChrome);
  bool GetShowChrome();
  void SetAutoResize(bool autoResize);
  bool GetAutoResize();
  void SetResizable(bool resizable);
  bool GetResizable();
  void SetAlpha(bool alpha);
  bool GetAlpha();
  void SetOpacity(double opacity);
  double GetOpacity();

  void SetLeft(int left);
  void SetTop(int top);
  void SetWidth(int width);
  void SetHeight(int height);
  int GetLeft();
  int GetTop();
  int GetWidth();
  int GetHeight();

  void SetTitle(const TCHAR* title);
  const TCHAR* GetTitle();

  void SetMenuBar(NativeMenu* menu);

  void OpenDialog(Settings* settings,v8::Persistent<v8::Function> cb);
  static void DialogClosed();
  static void OpenFileDialog(uv_work_t* req);
  static void ProcessFileDialog(uv_work_t* req);
  static void OpenColorDialog(uv_work_t* req);
  static void ProcessColorDialog(uv_work_t* req);
  static void OpenFontDialog(uv_work_t* req);
  static void ProcessFontDialog(uv_work_t* req);
  void OpenDevTools();
  void CloseDevTools();
  void RunInBrowser(char* script);
  bool IsMainWindow();

  appjs_rect GetRect();

  void SetBrowser(CefRefPtr<CefBrowser> browser);
  void SetV8Handle(v8::Handle<v8::Object> v8handle);
  CefRefPtr<CefBrowser> GetBrowser();
  v8::Handle<v8::Object> GetV8Handle();


#if defined(__WIN__)
  void SetStyle(long style, bool extended);
  long GetStyle(bool extended = false);

  //static void SetAppIcon(NW_ICONSIZE size, wchar_t* path);
  //void SetTitle(const wchar_t* title);
  //const wchar_t* GetTitle();
#endif

#if defined(__LINUX__)
  long GetDragHandlerId();
#endif

  CefWindowHandle handle_;

private:
  v8::Handle<v8::Object> v8handle_;
  CefRefPtr<CefBrowser> browser_;

  bool closed_;
  bool is_main_window_;
  bool auto_resize_;
  bool show_chrome_;
  bool resizable_;
  bool show_resize_grip;
  bool fullscreen_;
  bool show_on_taskbar_;
  bool show_on_pager_;
  bool topmost_;
  Settings* icons;
  bool alpha_;
  double opacity_;

  appjs_rect rect_;
  appjs_rect restoreRect_;
  long restoreStyle_;
  long restoreExStyle_;

#if defined(__LINUX__)
  long drag_handler_id;
#endif

};

typedef struct _appjs_dialog_settings {
  NW_DIALOGTYPE   type;
  NativeWindow*   me;
  void*           result;
  tstring         title;
  tstring         initialValue;
  tstring         reserveString1;
  int             reserveNumber1;
  bool            reserveBool1;
  bool            reserveBool2;
  v8::Persistent<v8::Function> cb;
} AppjsDialogSettings;

} /* appjs */

#endif /* end of APPJS_BASE_NATIVE_WINDOW_H */
