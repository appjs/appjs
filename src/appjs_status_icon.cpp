#include <node.h>
#include "appjs.h"
#include "appjs_status_icon.h"
#include "includes/cef_handler.h"
#include "includes/util.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

StatusIcon::StatusIcon(){}
StatusIcon::~StatusIcon(){}

Persistent<Function> StatusIcon::constructor;

void StatusIcon::Init() {
  DECLARE_CONSTRUCTOR("NativeStatusIcon");
  DECLARE_PROTOTYPE_METHOD("show",Show);
  DECLARE_PROTOTYPE_METHOD("hide",Hide);
  END_CONSTRUCTOR();
}

v8::Handle<Value> StatusIcon::New(const Arguments& args) {
  HandleScope scope;

  Persistent<Object> options = Persistent<Object>::New(args[0]->ToObject());
  Settings* settings = new Settings(options);
  NativeStatusIcon* statusIcon = new NativeStatusIcon(settings);

  Persistent<Object> self = Persistent<Object>::New(args.This());
  statusIcon->SetV8Handle(self);
  self->SetPointerInInternalField(0, statusIcon);

  return scope.Close(args.This());
}

v8::Handle<Value> StatusIcon::NewInstance(const Arguments& args) {
  HandleScope scope;
  v8::Handle<Value> argv[1] = { args[0] };
  return scope.Close(constructor->NewInstance(1, argv));
}

CREATE_PROTOTYPE_INVOKER(StatusIcon, Show)
CREATE_PROTOTYPE_INVOKER(StatusIcon, Hide)

} /* appjs */
