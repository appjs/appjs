#include <node.h>
#include "appjs.h"
#include "appjs_window.h"
#include "includes/cef_handler.h"
#include "includes/util.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

Window::Window() {};
Window::~Window() {};

Persistent<Function> Window::constructor;

void Window::Init () {
  CREATE_NODE_CONSTRUCTOR("Window");

  DEFINE_PROTOTYPE_METHOD("show",Show);
  DEFINE_PROTOTYPE_METHOD("hide",Hide);
  DEFINE_PROTOTYPE_METHOD("destroy",Destroy);
  DEFINE_PROTOTYPE_METHOD("runInBrowser",RunInBrowser);

  END_CONSTRUCTOR();
}

Handle<Value> Window::New(const Arguments& args) {
  HandleScope scope;

  auto self = Persistent<v8::Object>::New(args.This());

  char* url = (args[0]->IsString()) ? V8StringToChar(args[0]->ToString()) : (char*) "/";
  Local<Object> v8settings = (args[1]->IsObject()) ? args[1]->ToObject() : Object::New();

  Settings* settings = new Settings(v8settings);
  MainWindow* obj = new MainWindow(url,settings);

  self->SetPointerInInternalField (0, obj);

  return scope.Close(args.This());
}

Handle<Value> Window::NewInstance(const Arguments& args) {
  HandleScope scope;

  const unsigned argc = 2;
  Handle<Value> argv[argc] = { args[0],args[1] };
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

Handle<Value> Window::Destroy(const Arguments& args) {
  HandleScope scope;
  
  MainWindow *obj = ObjectWrap::Unwrap<MainWindow> (args.This());

  obj->destroy();

  return args.This();
}

Handle<Value> Window::RunInBrowser(const Arguments& args) {
  HandleScope scope;

  if(!args[0]->IsFunction())
    THROW_BAD_ARGS;

  CefRefPtr<CefBrowser> browser = g_handler->GetBrowser();

  if(browser) {
    char* script = V8StringToFunctionChar(args[0]->ToString());
    g_handler->GetBrowser()->GetMainFrame()->ExecuteJavaScript(script,"",0);
  }

  return scope.Close(args.This());
}

} /* appjs */

