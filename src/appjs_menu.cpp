#include <node.h>
#include "appjs.h"
#include "appjs_menu.h"
#include "includes/cef_handler.h"
#include "includes/util.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

Menu::Menu(){}
Menu::~Menu(){}

Persistent<Function> Menu::constructor;

void Menu::Init() {
  DECLARE_CONSTRUCTOR("NativeMenu");
  END_CONSTRUCTOR();
}

v8::Handle<Value> Menu::New(const Arguments& args) {
  HandleScope scope;

  Persistent<Object> options = Persistent<Object>::New(args[0]->ToObject());
  Settings* settings = new Settings(options);
  NativeMenu* menu = new NativeMenu(settings);

  Persistent<Object> self = Persistent<Object>::New(args.This());
  menu->SetV8Handle(self);
  self->SetPointerInInternalField(0, menu);

  return scope.Close(args.This());
}

v8::Handle<Value> Menu::NewInstance(const Arguments& args) {
  HandleScope scope;
  v8::Handle<Value> argv[1] = { args[0] };
  return scope.Close(constructor->NewInstance(1, argv));
}

} /* appjs */
