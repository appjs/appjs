#include <node.h>
#include "includes/cef.h"
#include "includes/cef_sync_handler.h"
#include "includes/cef_handler.h"
#include "includes/util.h"

namespace appjs {

using namespace v8;


bool AppjsSyncHandler::Execute(const CefString& name,
                               CefRefPtr<CefV8Value> object,
                               const CefV8ValueList& arguments,
                               CefRefPtr<CefV8Value>& retval,
                               CefString& exception) {
  REQUIRE_UI_THREAD();
  if (browser_.get()) {
    HandleScope scope;
    //Local<Object> global = Context::GetCurrent()->Global();
    Local<Value> argv[1] = { CefStringToV8(arguments[0]->GetStringValue()) };
    Handle<Object> window = ClientHandler::GetNodeWindow(browser_);
    Local<Function> handler = Local<Function>::Cast(window->Get(String::NewSymbol("onmessage")));
    Local<Value> result = handler->Call(window, 1, argv);
    if (result->IsString()) {
      char* plain = V8StringToChar(result->ToString());
      retval = CefV8Value::CreateString(plain);
    }
  }
  return true;
}


}
