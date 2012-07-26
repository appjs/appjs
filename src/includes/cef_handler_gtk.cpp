#include "include/cef_browser.h"
#include "includes/cef.h"
#include "includes/cef_handler.h"

using namespace v8;
using namespace appjs;

CefWindowHandle ClientHandler::GetMainHwnd(){
  return gtk_widget_get_ancestor(mainHandle,GTK_TYPE_WINDOW);
}

NativeWindow* ClientHandler::GetWindow(CefWindowHandle handle){
  return (NativeWindow*)g_object_get_data(G_OBJECT(handle), "nativewindow");
}

CefWindowHandle ClientHandler::GetContainer(CefRefPtr<CefBrowser> browser){
  return gtk_widget_get_ancestor(GTK_WIDGET(browser->GetWindowHandle()), GTK_TYPE_WINDOW);
}

bool ClientHandler::OnKeyEvent(CefRefPtr<CefBrowser> browser, KeyEventType type, int code,
                               int modifiers, bool isSystemKey, bool isAfterJavaScript) {

  if (!browser->IsPopup() && type == KEYEVENT_RAWKEYDOWN && code == 123) {
    browser->ShowDevTools();
    return true;
  }
  return false;
};

void ClientHandler::CloseMainWindow() {
  REQUIRE_UI_THREAD();
  appjs::Cef::Shutdown();
}


void ClientHandler::SetWindowTitle(CefWindowHandle handle, const char* title) {
  gtk_window_set_title((GtkWindow*)handle, title);
}


CefRefPtr<ClientHandler> g_handler;
