#include <node.h>
#include "appjs.h"
#include "appjs_window.h"
#include "includes/cef_handler.h"
#include "includes/util.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

Window::Window(){}
Window::~Window(){}

Persistent<Function> Window::constructor;

void Window::Init() {
  DECLARE_CONSTRUCTOR("NativeWindow");
  DECLARE_PROTOTYPE_METHOD("setMenuBar", SetMenuBar);
  DECLARE_PROTOTYPE_METHOD("openDialog", OpenDialog);
  DECLARE_PROTOTYPE_METHOD("openDevTools", OpenDevTools);
  DECLARE_PROTOTYPE_METHOD("closeDevTools", CloseDevTools);
  DECLARE_PROTOTYPE_METHOD("restore", Restore);
  DECLARE_PROTOTYPE_METHOD("minimize", Minimize);
  DECLARE_PROTOTYPE_METHOD("maximize", Maximize);
  DECLARE_PROTOTYPE_METHOD("fullscreen", Fullscreen);
  DECLARE_PROTOTYPE_METHOD("drag", Drag);
  DECLARE_PROTOTYPE_METHOD("show", Show);
  DECLARE_PROTOTYPE_METHOD("focus", Focus);
  DECLARE_PROTOTYPE_METHOD("hide", Hide);
  DECLARE_PROTOTYPE_METHOD("destroy", Destroy);
  DECLARE_PROTOTYPE_METHOD("runInBrowser", RunInBrowser);
  DECLARE_PROTOTYPE_METHOD("send", SendSync);
  DECLARE_PROTOTYPE_METHOD("move", Move);
  DECLARE_PROTOTYPE_METHOD("resize", Resize);
  DECLARE_PROTOTYPE_METHOD("setIcon", SetIcon);
  DECLARE_INSTANCE_ACCESSOR("left", Left);
  DECLARE_INSTANCE_ACCESSOR("top", Top);
  DECLARE_INSTANCE_ACCESSOR("height", Height);
  DECLARE_INSTANCE_ACCESSOR("width", Width);
  DECLARE_INSTANCE_ACCESSOR("title", Title);
  DECLARE_INSTANCE_ACCESSOR("state", State);
  DECLARE_INSTANCE_ACCESSOR("topmost", Topmost);
  DECLARE_INSTANCE_ACCESSOR("resizable", Resizable);
  DECLARE_INSTANCE_ACCESSOR("showChrome", ShowChrome);
  DECLARE_INSTANCE_ACCESSOR("alpha", Alpha);
  DECLARE_INSTANCE_ACCESSOR("opacity", Opacity);
  DECLARE_INSTANCE_ACCESSOR("autoResize", AutoResize);
#ifdef __WIN__
  DECLARE_PROTOTYPE_METHOD("style", Style);
#endif
  END_CONSTRUCTOR();
}

v8::Handle<Value> Window::New(const Arguments& args) {
  HandleScope scope;

  Persistent<Object> options = Persistent<Object>::New(args[0]->ToObject());
  Settings* settings = new Settings(options);
  NativeWindow* window = new NativeWindow(settings);

  Persistent<Object> self = Persistent<Object>::New(args.This());
  window->SetV8Handle(self);
  self->SetPointerInInternalField(0, window);

  return scope.Close(args.This());
}

v8::Handle<Value> Window::NewInstance(const Arguments& args) {
  HandleScope scope;
  v8::Handle<Value> argv[1] = { args[0] };
  return scope.Close(constructor->NewInstance(1, argv));
}

CREATE_INSTANCE_ACCESSOR(Window, Left, Integer, MAKE_INT32)
CREATE_INSTANCE_ACCESSOR(Window, Top, Integer, MAKE_INT32)
CREATE_INSTANCE_ACCESSOR(Window, Width, Integer, MAKE_INT32)
CREATE_INSTANCE_ACCESSOR(Window, Height, Integer, MAKE_INT32)
CREATE_INSTANCE_ACCESSOR(Window, Topmost, Boolean, MAKE_BOOLEAN)
CREATE_INSTANCE_ACCESSOR(Window, Resizable, Boolean, MAKE_BOOLEAN)
CREATE_INSTANCE_ACCESSOR(Window, ShowChrome, Boolean, MAKE_BOOLEAN)
CREATE_INSTANCE_ACCESSOR(Window, Alpha, Boolean, MAKE_BOOLEAN)
CREATE_INSTANCE_ACCESSOR(Window, Opacity, Number, MAKE_DOUBLE)
CREATE_INSTANCE_ACCESSOR(Window, AutoResize, Boolean, MAKE_BOOLEAN)
#ifdef __WIN__
CREATE_STRING_INSTANCE_ACCESSOR(Window, Title, String, V8StringToWCHAR)
#else
CREATE_STRING_INSTANCE_ACCESSOR(Window, Title, String, V8StringToChar)
#endif

CREATE_PROTOTYPE_INVOKER(Window, OpenDevTools)
CREATE_PROTOTYPE_INVOKER(Window, CloseDevTools)
CREATE_PROTOTYPE_INVOKER(Window, Drag)
CREATE_PROTOTYPE_INVOKER(Window, Restore)
CREATE_PROTOTYPE_INVOKER(Window, Minimize)
CREATE_PROTOTYPE_INVOKER(Window, Maximize)
CREATE_PROTOTYPE_INVOKER(Window, Fullscreen)
CREATE_PROTOTYPE_INVOKER(Window, Show)
CREATE_PROTOTYPE_INVOKER(Window, Focus)
CREATE_PROTOTYPE_INVOKER(Window, Hide)
CREATE_PROTOTYPE_INVOKER(Window, Destroy)


v8::Handle<Value> Window::SetMenuBar(const Arguments& args) {
  HandleScope scope;

  NativeMenu     *menu = ObjectWrap::Unwrap<NativeMenu>(args[0]->ToObject());//(NativeMenu*)args[0]->ToObject()->GetPointerFromInternalField(0);
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(args.This());

  window->SetMenuBar(menu);

  return scope.Close(args.This());
}

v8::Handle<Value> Window::OpenDialog(const Arguments& args) {
  HandleScope scope;

  Persistent<Object> options  = Persistent<Object>::New(args[0]->ToObject());
  Persistent<Function>    cb  = Persistent<Function>::New(Local<Function>::Cast(args[1]));
  Settings*          settings = new Settings(options);

  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(args.This());
  window->OpenDialog(settings,cb);

  return scope.Close(args.This());
}

v8::Handle<Value> Window::Move(const Arguments& args) {
  HandleScope scope;
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(args.This());

  int left = args[0]->Int32Value();
  int top = args[1]->Int32Value();
  if (args[2]->IsNumber()) {
    int width = args[2]->Int32Value();
    int height = args[3]->Int32Value();
    window->Move(left, top, width, height);
  } else {
    window->Move(left, top);
  }

  return scope.Close(args.This());
}

v8::Handle<Value> Window::Resize(const Arguments& args) {
  HandleScope scope;
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(args.This());

  int width = args[0]->Int32Value();
  int height = args[1]->Int32Value();
  window->Resize(width, height);

  return scope.Close(args.This());
}



v8::Handle<Value> Window::RunInBrowser(const Arguments& args) {
  HandleScope scope;

  if(!args[0]->IsFunction())
    THROW_BAD_ARGS;

  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow> (args.This());
  window->RunInBrowser(V8StringToFunctionChar(args[0]->ToString()));

  return scope.Close(args.This());
}

// synchronously send a string from Node to browser, then return string result from browser to Node
v8::Handle<Value> Window::SendSync(const Arguments& args) {
  HandleScope scope;

  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow> (args.This());

  if (!window->IsClosed()) {
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
        v8::Handle<String> ret = CefStringToV8(callback->ExecuteFunction(appjsObject, argsOut)->GetStringValue());

        // exit browser v8 context, return string result to Node caller
        context->Exit();
        return scope.Close(ret);
      }
    }
  }
  // likely error condition
  return scope.Close(args.This());
}

v8::Handle<Value> Window::SetIcon(const Arguments& args) {
  HandleScope scope;
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(args.This());

  Local<String> val = args[0]->ToString();
  NW_ICONSIZE enumVal;

  STRING_TO_ENUM("smaller", NW_ICONSIZE_SMALLER)
  STRING_TO_ENUM("small", NW_ICONSIZE_SMALL)
  STRING_TO_ENUM("big", NW_ICONSIZE_BIG)
  STRING_TO_ENUM("bigger", NW_ICONSIZE_BIGGER)

#if defined(__WIN__)
  window->SetIcon(enumVal, V8StringToWCHAR(args[1]->ToString()));
#else
  window->SetIcon(enumVal, V8StringToChar(args[1]->ToString()));
#endif

  return scope.Close(args.This());
}

v8::Handle<Value> Window::GetState(Local<String> property, const AccessorInfo &info) {
  HandleScope scope;
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(info.Holder());

  v8::Handle<Value> val;

  switch (window->GetState()) {
    ENUM_TO_STRING(NW_STATE_NORMAL, "normal")
    ENUM_TO_STRING(NW_STATE_MINIMIZED, "minimized")
    ENUM_TO_STRING(NW_STATE_MAXIMIZED, "maximized")
    ENUM_TO_STRING(NW_STATE_FULLSCREEN, "fullscreen")
  }

  return scope.Close(val);
}

void Window::SetState(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(info.Holder());
  Local<String> val = value->ToString();
  NW_STATE enumVal;

  STRING_TO_ENUM("normal", NW_STATE_NORMAL)
  STRING_TO_ENUM("minimized", NW_STATE_MINIMIZED)
  STRING_TO_ENUM("maximized", NW_STATE_MAXIMIZED)
  STRING_TO_ENUM("fullscreen", NW_STATE_FULLSCREEN)

  window->SetState(enumVal);
}




#if defined(__WIN__)

v8::Handle<Value> Window::Style(const Arguments& args) {
  HandleScope scope;
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(args.This());
  switch (args.Length()) {
    case 0:
      return scope.Close(Integer::New(window->GetStyle(false)));
    case 1:
      if (args[0]->IsBoolean()) {
        return scope.Close(Integer::New(window->GetStyle(args[0]->BooleanValue())));
      } else {
        window->SetStyle(args[0]->Int32Value(), false);
      }
      break;
    case 2:
      window->SetStyle(args[0]->Int32Value(), args[1]->BooleanValue());
      break;
  }
  return scope.Close(args.This());
}

#endif

} /* appjs */
