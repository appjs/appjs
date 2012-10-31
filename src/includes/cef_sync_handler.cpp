#include <node.h>
#include "includes/cef.h"
#include "includes/cef_sync_handler.h"
#include "includes/util.h"
#include "native_window/native_window.h"

namespace appjs {

using namespace v8;


bool AppjsSyncHandler::Execute(const CefString& name,
                               CefRefPtr<CefV8Value> object,
                               const CefV8ValueList& arguments,
                               CefRefPtr<CefV8Value>& retval,
                               CefString& exception) {
  if (browser_.get()) {
    HandleScope scope;
    Local<Value> argv[1] = { CefStringToV8(arguments[0]->GetStringValue()) };
    v8::Handle<Object> window = NativeWindow::GetWindow(browser_)->GetV8Handle();;
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
