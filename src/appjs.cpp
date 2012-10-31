#include "appjs.h"
#include "appjs_app.h"

namespace appjs {

using namespace v8;

v8::Handle<Value> InitApp(const Arguments& args) {

  HandleScope scope;
  return scope.Close(App::NewInstance(args));
}

void Init(v8::Handle<v8::Object> target) {
  App::Init();
  Window::Init();
  Menu::Init();
  StatusIcon::Init();

  target->Set(String::NewSymbol("init"), FunctionTemplate::New(InitApp)->GetFunction());
  target->Set(String::NewSymbol("App"), App::constructor);
  target->Set(String::NewSymbol("NativeWindow"), Window::constructor);
  target->Set(String::NewSymbol("NativeMenu"), Menu::constructor);
  target->Set(String::NewSymbol("NativeStatusIcon"), StatusIcon::constructor);
}

} /* appjs */

NODE_MODULE(appjs, appjs::Init)
