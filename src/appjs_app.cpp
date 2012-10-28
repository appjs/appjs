#include <node.h>
#include "appjs.h"
#include "appjs_app.h"
#include "appjs_window.h"
#include "appjs_menu.h"
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
  DECLARE_PROTOTYPE_METHOD("createMenu",CreateMenu);
  DECLARE_CLASS_FUNCTION(screenWidth, ScreenWidth);
  DECLARE_CLASS_FUNCTION(screenHeight, ScreenHeight);
  END_CONSTRUCTOR();
}

Handle<Value> App::New(const Arguments& args) {
  HandleScope scope;
  App* obj = new App();
  obj->Wrap(args.This());
  return scope.Close(args.This());
}

Handle<Value> App::NewInstance(const Arguments& args) {
  HandleScope scope;

  Persistent<Object> options = Persistent<Object>::New(args[0]->ToObject());
  Cef::Init(new Settings(options));

  Handle<Value> argv[1] = { args[0] };
  Local<Object> instance = constructor->NewInstance(1, argv);

  return scope.Close(instance);
}

Handle<Value> App::CreateWindow2(const Arguments& args) {
  HandleScope scope;
  return scope.Close(Window::NewInstance(args));
}

Handle<Value> App::CreateMenu(const Arguments& args) {
  HandleScope scope;
  return scope.Close(Menu::NewInstance(args));
}

Handle<Value> App::ScreenWidth(const Arguments& args) {
  HandleScope scope;
  Handle<Value> width = Integer::New(NativeWindow::ScreenWidth());
  return scope.Close(width);
}

Handle<Value> App::ScreenHeight(const Arguments& args) {
  HandleScope scope;
  Handle<Value> height = Integer::New(NativeWindow::ScreenHeight());
  return scope.Close(height);
}

} /* appjs */

