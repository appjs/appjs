#include <node.h>
#include "appjs.h"
#include "appjs_tray.h"
#include "includes/cef_handler.h"
#include "includes/util.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

Tray::Tray(){}
Tray::~Tray(){}

Persistent<Function> Tray::constructor;

void Tray::Init() {
  DECLARE_CONSTRUCTOR("NativeTray");
  END_CONSTRUCTOR();
}

Handle<Value> Tray::New(const Arguments& args) {
  HandleScope scope;

  Persistent<Object> options = Persistent<Object>::New(args[0]->ToObject());
  Settings* settings = new Settings(options);
  NativeTray* window = new NativeTray(settings);

  Persistent<Object> self = Persistent<Object>::New(args.This());
  tray->SetV8Handle(self);
  self->SetPointerInInternalField(0, tray);

  return scope.Close(args.This());
}

Handle<Value> Tray::NewInstance(const Arguments& args) {
  HandleScope scope;
  Handle<Value> argv[1] = { args[0] };
  return scope.Close(constructor->NewInstance(1, argv));
}

} /* appjs */
