#include "includes/cef.h"
#include "includes/cef_handler.h"
#include "native_status_icon/native_status_icon.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

NativeStatusIcon::NativeStatusIcon(Settings* settings){
  Init(settings);
}

NativeStatusIcon::~NativeStatusIcon(){
}

void NativeStatusIcon::Emit(Handle<Value>* args,int length){
  node::MakeCallback(v8handle_, "emit", length, args);
}

void NativeStatusIcon::Emit(const char* event){
  Handle<Value> args[1] = { String::New(event) };
  Emit(args,1);
}

void NativeStatusIcon::Emit(const char* event, Handle<Value> arg){

  Handle<Value> args[2] = {
    String::New(event),
    arg
  };

  Emit(args,2);
}

} /* appjs */
