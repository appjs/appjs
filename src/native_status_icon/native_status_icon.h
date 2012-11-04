#ifndef APPJS_BASE_NATIVE_STATUS_ICON_H
#define APPJS_BASE_NATIVE_STATUS_ICON_H
#pragma once

#ifdef __MAC__
#import <Cocoa/Cocoa.h>
#endif

#include "appjs.h"
#include "include/cef_browser.h"
#include "includes/util.h"
#include "native_menu/native_menu.h"
#include "native_status_icon/native_status_icon.h"
#include <node.h>

namespace appjs {

class NativeStatusIcon {

public:
  NativeStatusIcon(Settings* settings);
  ~NativeStatusIcon();
  void Init(Settings* settings);
  void Show();
  void Hide();

  void Emit(v8::Handle<v8::Value>* args,int);
  void Emit(const char* event);
  void Emit(const char* event, v8::Handle<v8::Value> arg);

  void SetV8Handle(v8::Handle<v8::Object> v8handle) {v8handle_ = v8handle;};
  v8::Handle<v8::Object> GetV8Handle() {return v8handle_;};

#ifdef __WIN__
  NativeMenu* GetMenu() { return menu_;};
#endif
private:
  v8::Handle<v8::Object> v8handle_;

#ifdef __LINUX__
  GtkStatusIcon* statusIconHandle_;
#endif
#ifdef __MAC__
  NSStatusItem* statusIconHandle_;
#endif
#ifdef __WIN__
  NOTIFYICONDATA statusIconHandle_;
  NativeMenu* menu_;
#endif
};

} /* appjs */

#endif /* end of APPJS_BASE_NATIVE_STATUS_ICON_H */
