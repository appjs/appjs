#include <node.h>
#include "appjs.h"
#include "appjs_app.h"
#include "appjs_window.h"

namespace appjs {

using namespace v8;

App::App() {};
App::~App() {};

Persistent<Function> App::constructor;

void App::Init () {

  Local<FunctionTemplate> super = Window::Init(true);

  CREATE_NODE_CONSTRUCTOR(App);
  
  tpl->Inherit(super);
// TODO Should we allow more than one window?
//  DEFINE_PROTOTYPE_METHOD("createWindow",CreateWindow);
  
  END_CONSTRUCTOR();
}

Handle<Value> App::New(const Arguments& args) {
  HandleScope scope;

  App* obj = new App();
  obj->Wrap(args.This());
  
  return args.This();
}

Handle<Value> App::NewInstance(const Arguments& args) {
  HandleScope scope;

  const unsigned argc = 2;
  Handle<Value> argv[argc] = { args[0] , args[1] };
  Local<Object> instance = constructor->NewInstance(argc, argv);
  Window::constructor->NewInstance(argc,argv);

  return scope.Close(instance);
}

Handle<Value> App::CreateWindow(const Arguments& args) {

  HandleScope scope;

  return scope.Close(Window::NewInstance(args));
}

} /* appjs */

