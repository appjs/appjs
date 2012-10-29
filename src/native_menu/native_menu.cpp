#include "includes/cef.h"
#include "includes/cef_handler.h"
#include "native_menu/native_menu.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

NativeMenu::NativeMenu(Settings* settings){
  Init(settings);
}

NativeMenu::~NativeMenu(){
  // destroy all v8 persistent objects
}

void NativeMenu::Emit(Handle<Value>* args,int length){
  node::MakeCallback(v8handle_, "emit", length, args);
}

void NativeMenu::Emit(const char* event){
  Handle<Value> args[1] = { String::New(event) };
  Emit(args,1);
}

void NativeMenu::Emit(const char* event, Handle<Value> arg){

  Handle<Value> args[2] = {
    String::New(event),
    arg
  };

  Emit(args,2);
}


} /* appjs */
