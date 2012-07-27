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
  DEFINE_PROTOTYPE_METHOD("restore", Restore);
  DEFINE_PROTOTYPE_METHOD("minimize", Minimize);
  DEFINE_PROTOTYPE_METHOD("maximize", Maximize);
  DEFINE_PROTOTYPE_METHOD("fullscreen", Fullscreen);
  DEFINE_PROTOTYPE_METHOD("drag", Drag);
  DEFINE_PROTOTYPE_METHOD("show", Show);
  DEFINE_PROTOTYPE_METHOD("hide", Hide);
  DEFINE_PROTOTYPE_METHOD("destroy", Destroy);
  DEFINE_PROTOTYPE_METHOD("runInBrowser", RunInBrowser);
  DEFINE_PROTOTYPE_METHOD("send", SendSync);
  DEFINE_PROTOTYPE_METHOD("move", Move);
  DEFINE_PROTOTYPE_METHOD("resize", Resize);
  CREATE_CPP_ACCESSOR("left", Left);
  CREATE_CPP_ACCESSOR("top", Top);
  CREATE_CPP_ACCESSOR("height", Height);
  CREATE_CPP_ACCESSOR("width", Width);
  CREATE_CPP_ACCESSOR("title", Title);
  CREATE_CPP_ACCESSOR("state", State);
  CREATE_CPP_ACCESSOR("topmost", Topmost);
  CREATE_CPP_ACCESSOR("resizable", Resizable);
  CREATE_CPP_ACCESSOR("showChrome", ShowChrome);
  CREATE_CPP_ACCESSOR("alpha", Alpha);
  CREATE_CPP_ACCESSOR("opacity", Opacity);
  CREATE_CPP_ACCESSOR("autoResize", AutoResize);
#ifdef __WIN__
  DEFINE_PROTOTYPE_METHOD("style", Style);
#endif

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



Handle<Value> Window::Move(const Arguments& args) {
  HandleScope scope;
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(args.This());

  int top = args[0]->Int32Value();
  int left = args[1]->Int32Value();
  if (args[2]->IsNumber()) {
    int width = args[2]->Int32Value();
    int height = args[3]->Int32Value();
    window->Move(top, left, width, height);
  } else {
    window->Move(top, left);
  }

  return scope.Close(args.This());
}

Handle<Value> Window::Resize(const Arguments& args) {
  HandleScope scope;
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(args.This());

  int width = args[0]->Int32Value();
  int height = args[1]->Int32Value();
  window->Resize(width, height);

  return scope.Close(args.This());
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

Handle<Value> Window::Drag(const Arguments& args) {
  HandleScope scope;
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(args.This());
  window->Drag();
  return scope.Close(args.This());
}

Handle<Value> Window::Restore(const Arguments& args) {
  HandleScope scope;
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(args.This());
  window->Restore();
  return scope.Close(args.This());
}

Handle<Value> Window::Minimize(const Arguments& args) {
  HandleScope scope;
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(args.This());
  window->Minimize();
  return scope.Close(args.This());
}

Handle<Value> Window::Maximize(const Arguments& args) {
  HandleScope scope;
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(args.This());
  window->Maximize();
  return scope.Close(args.This());
}

Handle<Value> Window::Fullscreen(const Arguments& args) {
  HandleScope scope;
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(args.This());
  window->Fullscreen();
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

WINDOW_ACCESSOR(Left, Integer, MAKE_INT32)
WINDOW_ACCESSOR(Top, Integer, MAKE_INT32)
WINDOW_ACCESSOR(Width, Integer, MAKE_INT32)
WINDOW_ACCESSOR(Height, Integer, MAKE_INT32)
WINDOW_ACCESSOR(Title, String, V8StringToChar)
WINDOW_ACCESSOR(Topmost, Boolean, MAKE_BOOLEAN)
WINDOW_ACCESSOR(Resizable, Boolean, MAKE_BOOLEAN)
WINDOW_ACCESSOR(ShowChrome, Boolean, MAKE_BOOLEAN)
WINDOW_ACCESSOR(Alpha, Boolean, MAKE_BOOLEAN)
WINDOW_ACCESSOR(Opacity, Number, MAKE_DOUBLE)
WINDOW_ACCESSOR(AutoResize, Boolean, MAKE_BOOLEAN)




Handle<Value> Window::GetState(Local<String> property, const AccessorInfo &info) {
  HandleScope scope;
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(info.Holder());
  Handle<Value> state = String::New("normal");

  switch (window->GetState()) {
    case NW_STATE_NORMAL:
      state = String::New("normal");
      break;
    case NW_STATE_MINIMIZED:
      state = String::New("minimized");
      break;
    case NW_STATE_MAXIMIZED:
      state = String::New("maximized");
      break;
    case NW_STATE_FULLSCREEN:
      state = String::New("fullscreen");
  }

  return scope.Close(state);
}




void Window::SetState(Local<String> property, Local<Value> value, const AccessorInfo& info) {
  NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(info.Holder());
  Local<String> val = value->ToString();
  if (STRING_EQ(val, "normal")) {
    window->SetState(NW_STATE_NORMAL);
  } else if (STRING_EQ(val, "minimized")) {
    window->SetState(NW_STATE_MINIMIZED);
  } else if (STRING_EQ(val, "maximized")) {
    window->SetState(NW_STATE_MAXIMIZED);
  } else if (STRING_EQ(val, "fullscreen")) {
    window->SetState(NW_STATE_FULLSCREEN);
  }
}


#if defined(__WIN__)

// Handle<Value> Window::SetNonclientWidth(const Arguments& args) {
//   HandleScope scope;
//   NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(args.This());

//   if (args.Length() == 4) {
//     int top = args[0]->Int32Value();
//     int left = args[1]->Int32Value();
//     int right = args[2]->Int32Value();
//     int bottom = args[3]->Int32Value();
//     window->SetNonclientWidth(top, left, right, bottom);
//   } else {
//     window->SetNonclientWidth(args[0]->Int32Value());
//   }

//   return scope.Close(args.This());
// }

Handle<Value> Window::Style(const Arguments& args) {
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

// Handle<Value> Window::GetBlur(Local<String> property, const AccessorInfo &info) {
//   HandleScope scope;
//   NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(info.Holder());
//   return scope.Close(Boolean::New(window->GetBlur()));
// }

// void Window::SetBlur(Local<String> property, Local<Value> value, const AccessorInfo& info) {
//   NativeWindow *window = ObjectWrap::Unwrap<NativeWindow>(info.Holder());
//   window->SetBlur(value->BooleanValue());
// }

#endif

} /* appjs */
