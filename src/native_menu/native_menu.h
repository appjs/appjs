#ifndef APPJS_BASE_NATIVE_MENU_H
#define APPJS_BASE_NATIVE_MENU_H
#pragma once

#ifdef __MAC__
#import <Cocoa/Cocoa.h>
#endif

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

  void Emit(v8::Handle<v8::Value>* args,int);
  void Emit(const char* event);
  void Emit(const char* event, v8::Handle<v8::Value> arg);

  void SetV8Handle(v8::Handle<v8::Object> v8handle) {v8handle_ = v8handle;};
  v8::Handle<v8::Object> GetV8Handle() {return v8handle_;};
#ifdef __LINUX__
  int AddSubMenu(GtkWidget*,Settings*);
  bool Attach(GtkMenuShell*);
#elif defined(__MAC__)
  int AddSubMenu(NSMenu*,Settings*);
  bool Attach(NSMenu*);
#elif defined(__WIN__)
  int AddSubMenu(HMENU&,Settings*);
  bool Attach(HMENU&);
  bool Detach(HMENU&);
#endif

private:
  v8::Handle<v8::Object> v8handle_;
  bool attached_;

#ifdef __LINUX__
  //GtkWidget* menu_;
  std::vector<GtkWidget*> menuItems_;
  GtkMenuShell* menu_;
#elif defined(__MAC__)
  NSArray* menuItems_;
#endif
#ifdef __WIN__
  HMENU menu_;
#endif

};

typedef struct _appjs_action_callback {
  v8::Persistent<v8::Object> action;
  v8::Persistent<v8::Object> item;
  NativeMenu* menu;
} appjs_action_callback;

} /* appjs */

#endif /* end of APPJS_BASE_NATIVE_MENU_H */
