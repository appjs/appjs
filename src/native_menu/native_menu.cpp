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


void NativeMenu::SetV8Handle(Handle<Object> v8handle) {
  v8handle_ = v8handle;
}


} /* appjs */
