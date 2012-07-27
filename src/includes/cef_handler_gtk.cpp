#include "include/cef_browser.h"
#include "includes/cef.h"
#include "includes/cef_handler.h"

using namespace v8;
using namespace appjs;

NativeWindow* ClientHandler::GetWindow(CefWindowHandle handle){
  return (NativeWindow*)g_object_get_data(G_OBJECT(handle), "nativewindow");
}

CefWindowHandle ClientHandler::GetContainer(CefRefPtr<CefBrowser> browser){
  return gtk_widget_get_ancestor(GTK_WIDGET(browser->GetWindowHandle()), GTK_TYPE_WINDOW);
}

CefRefPtr<ClientHandler> g_handler;
