#import  <Cocoa/Cocoa.h>
#import  <objc/runtime.h>
#import  "include/cef_application_mac.h"
#include "appjs.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/cef_handler.h"
#include "native_menu/native_menu.h"

// The global ClientHandler reference.
extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

void NativeMenu::Init(Settings* settings) {
}

//int NativeMenu::AddSubMenu(GtkWidget* menu,Settings* settings){
//}

} /* appjs */

