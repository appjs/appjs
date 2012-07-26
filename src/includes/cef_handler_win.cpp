#include "include/cef_browser.h"
#include "includes/cef.h"
#include "includes/cef_handler.h"

using namespace v8;
using namespace appjs;

CefWindowHandle ClientHandler::GetMainHwnd(){
  return mainHandle;
}

NativeWindow* ClientHandler::GetWindow(CefWindowHandle handle){
  LONG_PTR win = GetWindowLongPtr(handle, GWLP_USERDATA);
  return win ? (NativeWindow*)win : NULL;
}

CefWindowHandle ClientHandler::GetContainer(CefRefPtr<CefBrowser> browser){
  return GetParent(browser->GetWindowHandle());
}


bool ClientHandler::OnKeyEvent(CefRefPtr<CefBrowser> browser, KeyEventType type, int code,
                               int modifiers, bool isSystemKey, bool isAfterJavaScript) {

  if (!browser->IsPopup() && type == KEYEVENT_RAWKEYDOWN && code == VK_F12) {
    browser->ShowDevTools();
    return true;
  }
  return false;
};


void ClientHandler::CloseMainWindow() {
  REQUIRE_UI_THREAD();
  PostMessage(mainHandle, WM_CLOSE, 0, 0);
  appjs::Cef::Shutdown();
}

void ClientHandler::SetWindowTitle(CefWindowHandle handle, const char* title) {
  SetWindowText(handle, title);
}

CefRefPtr<ClientHandler> g_handler;
