#include <node.h>
#include "appjs.h"
#include "appjs_window.h"

namespace appjs {

using namespace v8;
using namespace node;

Persistent<FunctionTemplate> Window::constructor_template;

void Window::Init (Handle<v8::Object> target) {

  CREATE_NODE_CONSTRUCTOR ("Window", Window);
  DEFINE_NODE_METHOD("show", Show);
  DEFINE_NODE_METHOD("hide", Hide);
  END_CONSTRUCTOR ();

}

Handle<Value> Window::New(const Arguments& args) {
  HandleScope scope;
  
  uv_ref (uv_default_loop ());
  auto self = Persistent<v8::Object>::New (args.This ());
  
  MainWindow* obj = new MainWindow();
  self->SetPointerInInternalField (0, obj);
  
  return args.This();
}

Handle<Value> Window::Show(const Arguments& args) {
  HandleScope scope;
  
  MainWindow *obj = ObjectWrap::Unwrap<MainWindow> (args.This());

//  MainWindow* obj = self->GetPointerInInternalField(0);
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

