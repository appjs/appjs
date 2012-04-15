#include "appjs.h"
#include "appjs_app.h"

namespace appjs {

using namespace v8;

Handle<Value> InitApp(const Arguments& args) {

  HandleScope scope;
  return scope.Close(App::NewInstance(args));
}

void Init(Handle<v8::Object> target) {

  App::Init();
  
  target->Set(String::NewSymbol("init"),
      FunctionTemplate::New(InitApp)->GetFunction());

}

} /* appjs */

#ifdef WIN32
NODE_MODULE(appjs, appjs::Init)
#endif
