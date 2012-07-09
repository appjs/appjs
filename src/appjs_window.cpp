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

  DEFINE_PROTOTYPE_METHOD("openDevTools", OpenDevTools);
  DEFINE_PROTOTYPE_METHOD("closeDevTools", CloseDevTools);
  DEFINE_PROTOTYPE_METHOD("show", Show);
  DEFINE_PROTOTYPE_METHOD("hide", Hide);
  DEFINE_PROTOTYPE_METHOD("destroy", Destroy);
  DEFINE_PROTOTYPE_METHOD("runInBrowser", RunInBrowser);
  DEFINE_PROTOTYPE_METHOD("send", SendSync);

  END_CONSTRUCTOR();
}

Handle<Value> Window::New(const Arguments& args) {
  HandleScope scope;

  Handle<Object> self = Persistent<Object>::New(args.This());

  char* url = (args[0]->IsString()) ? V8StringToChar(args[0]->ToString()) : (char*) "/";
  Persistent<Object> windowSettings = Persistent<Object>::New((args[1]->IsObject()) ? args[1]->ToObject() : Object::New());

  Settings* settings = new Settings(windowSettings);
  NativeWindow* window = new NativeWindow(url, settings);
  window->SetV8Handle(self);

  self->SetPointerInInternalField (0, window);

  return scope.Close(args.This());
}

Handle<Value> Window::NewInstance(const Arguments& args) {
  HandleScope scope;

  const unsigned argc = 2;
  Handle<Value> argv[argc] = { args[0],args[1] };
  Local<Object> instance = constructor->NewInstance(argc, argv);

  return scope.Close(instance);
}

Handle<Value> Window::OpenDevTools(const Arguments& args) {
  HandleScope scope;

  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow> (args.This());
  window->OpenDevTools();

  return scope.Close(args.This());
}

Handle<Value> Window::CloseDevTools(const Arguments& args) {
  HandleScope scope;

  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow> (args.This());
  window->CloseDevTools();

  return scope.Close(args.This());
}

Handle<Value> Window::Show(const Arguments& args) {
  HandleScope scope;

  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow> (args.This());
  window->Show();

  return scope.Close(args.This());
}

Handle<Value> Window::Hide(const Arguments& args) {
  HandleScope scope;

  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow> (args.This());
  window->Hide();

  return scope.Close(args.This());
}

Handle<Value> Window::Destroy(const Arguments& args) {
  HandleScope scope;

  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow> (args.This());
  window->Destroy();

  return scope.Close(args.This());
}

Handle<Value> Window::RunInBrowser(const Arguments& args) {
  HandleScope scope;

  if(!args[0]->IsFunction())
    THROW_BAD_ARGS;

  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow> (args.This());
  window->RunInBrowser(V8StringToFunctionChar(args[0]->ToString()));

  return scope.Close(args.This());
}

// synchronously send a string from Node to browser, then return string result from browser to Node
Handle<Value> Window::SendSync(const Arguments& args) {
  HandleScope scope;

  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow> (args.This());

  if (window->GetBrowser()) {
    // find browser's v8 context
    CefRefPtr<CefV8Context> context = window->GetBrowser()->GetMainFrame()->GetV8Context();

    // ensure it's usable and enter
    if (context.get() && context->Enter()) {
      // try to get "appjs.onmessage" function
      CefRefPtr<CefV8Value> appjsObject = context->GetGlobal()->GetValue("appjs");
      CefRefPtr<CefV8Value> callback = appjsObject->GetValue("onmessage");
      if (callback.get()) {

        // convert Node V8 string to Cef V8 string
        CefV8ValueList argsOut;
        argsOut.push_back(CefV8Value::CreateString(V8StringToChar(args[0]->ToString())));

        // execute window.appjs fuction, passing in the string,
        // then convert the return value from a CefValue to a Node V8 string
        Handle<String> ret = CefStringToV8(callback->ExecuteFunction(appjsObject, argsOut)->GetStringValue());

        // exit browser v8 context, return string result to Node caller
        context->Exit();
        return scope.Close(ret);
      }
    }
  }
  // likely error condition
  return scope.Close(Undefined());
}


} /* appjs */
