#include <node.h>
#include "appjs.h"
#include "appjs_app.h"
#include "appjs_window.h"
#include "appjs_menu.h"
#include "appjs_status_icon.h"
#include "includes/cef.h"
#include "includes/util.h"

namespace appjs {

using namespace v8;

App::App() {};
App::~App() {};

bool App::initialized_ = false;
Persistent<Function> App::constructor;

void App::Init() {
  DECLARE_CONSTRUCTOR("App");
  DECLARE_PROTOTYPE_METHOD("createWindow",CreateWindow2);
  DECLARE_PROTOTYPE_METHOD("createMenu",CreateMenu2);
  DECLARE_PROTOTYPE_METHOD("createStatusIcon",CreateStatusIcon);
  DECLARE_CLASS_FUNCTION(screenWidth, ScreenWidth);
  DECLARE_CLASS_FUNCTION(screenHeight, ScreenHeight);
  END_CONSTRUCTOR();
}

v8::Handle<Value> App::New(const Arguments& args) {
  v8::HandleScope scope;
  App* obj = new App();
  obj->Wrap(args.This());
  return scope.Close(args.This());
}

v8::Handle<Value> App::NewInstance(const Arguments& args) {
  v8::HandleScope scope;

  Persistent<Object> options = Persistent<Object>::New(args[0]->ToObject());
  Cef::Init(new Settings(options));

  v8::Handle<Value> argv[1] = { args[0] };
  Local<Object> instance = constructor->NewInstance(1, argv);

  return scope.Close(instance);
}

v8::Handle<Value> App::CreateWindow2(const Arguments& args) {
  v8::HandleScope scope;
  return scope.Close(Window::NewInstance(args));
}

v8::Handle<Value> App::CreateMenu2(const Arguments& args) {
  v8::HandleScope scope;
  return scope.Close(Menu::NewInstance(args));
}

v8::Handle<Value> App::CreateStatusIcon(const Arguments& args) {
  v8::HandleScope scope;
  return scope.Close(StatusIcon::NewInstance(args));
}

v8::Handle<Value> App::ScreenWidth(const Arguments& args) {
  v8::HandleScope scope;
  v8::Handle<Value> width = Integer::New(NativeWindow::ScreenWidth());
  return scope.Close(width);
}

v8::Handle<Value> App::ScreenHeight(const Arguments& args) {
  v8::HandleScope scope;
  v8::Handle<Value> height = Integer::New(NativeWindow::ScreenHeight());
  return scope.Close(height);
}

} /* appjs */

