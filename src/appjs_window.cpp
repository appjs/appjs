#include <node.h>
#include "appjs.h"
#include "appjs_window.h"
#include "includes/util.h"

namespace appjs {

using namespace v8;

Window::Window() {};
Window::~Window() {};

Persistent<Function> Window::constructor;

void Window::Init () {
  Window::Init(false);
}

Local<FunctionTemplate> Window::Init(bool) {
  CREATE_NODE_CONSTRUCTOR(Window);

  DEFINE_PROTOTYPE_METHOD("show",Show);
  DEFINE_PROTOTYPE_METHOD("hide",Hide);
  //DEFINE_PROTOTYPE_METHOD("onshow",On);

  END_CONSTRUCTOR();
  
  return tpl;
}

Handle<Value> Window::New(const Arguments& args) {
  HandleScope scope;

  auto self = Persistent<v8::Object>::New (args.This ());

  Local<Object> settings = (args[0]->IsObject()) ? args[0]->ToObject() : Object::New();

  MainWindow* obj = new MainWindow(settings);

  self->SetPointerInInternalField (0, obj);

  return args.This();
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

  obj->show();

  return args.This();
}

Handle<Value> Window::Hide(const Arguments& args) {
  HandleScope scope;
  
  MainWindow *obj = ObjectWrap::Unwrap<MainWindow> (args.This());

  obj->hide();

  return args.This();
}

} /* appjs */

