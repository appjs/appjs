#include <node.h>
#include "appjs.h"
#include "appjs_window.h"

namespace appjs {

using namespace v8;

Window::Window() {};
Window::~Window() {};

Persistent<Function> Window::constructor;

void Window::Init () {

  /*CREATE_NODE_CONSTRUCTOR(Window);

  DEFINE_PROTOTYPE_METHOD("show",Show);

  END_CONSTRUCTOR();*/
  Window::Init(false);
}

Local<FunctionTemplate> Window::Init(bool) {
  CREATE_NODE_CONSTRUCTOR(Window);

  DEFINE_PROTOTYPE_METHOD("show",Show);

  END_CONSTRUCTOR();
  
  return tpl;
}

Handle<Value> Window::New(const Arguments& args) {
  HandleScope scope;
  
  auto self = Persistent<v8::Object>::New (args.This ());
  
  MainWindow* obj = new MainWindow();
  self->SetPointerInInternalField (0, obj);
  
  return args.This();
/*  HandleScope scope;

  Window* obj = new Window();
  //obj->counter_ = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
  obj->Wrap(args.This());
  
  return args.This();*/
}

Handle<Value> Window::NewInstance(const Arguments& args) {
  HandleScope scope;

  const unsigned argc = 1;
  Handle<Value> argv[argc] = { args[0] };
  Local<Object> instance = constructor->NewInstance(argc, argv);

  return scope.Close(instance);
}

Handle<Value> Window::Show(const Arguments& args) {
  HandleScope scope;
  
  MainWindow *obj = ObjectWrap::Unwrap<MainWindow> (args.This());

  obj->Show();

  return args.This();
}

Handle<Value> Window::Hide(const Arguments& args) {
  HandleScope scope;
  
  MainWindow *obj = ObjectWrap::Unwrap<MainWindow> (args.This());

  obj->Stop();

  return args.This();
}

} /* appjs */

