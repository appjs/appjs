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

void ClientHandler::OnContentsSizeChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int width, int height) {
  REQUIRE_UI_THREAD();
  NativeWindow* window = GetWindow(browser);
  if (window && window->auto_resize) {
    RECT rect;
    GetClientRect(window->handle_, &rect);

    HDWP hdwp = BeginDeferWindowPos(1);
    hdwp = DeferWindowPos(hdwp, window->handle_, NULL, rect.left, rect.top, width, height, SWP_NOZORDER);
    EndDeferWindowPos(hdwp);
  }
}

void ClientHandler::CloseMainWindow() {
  REQUIRE_UI_THREAD();
  PostMessage(mainHandle, WM_CLOSE, 0, 0);
  appjs::Cef::Shutdown();
}

void ClientHandler::SetWindowTitle(CefWindowHandle handle, const char* title) {
  SetWindowText(handle, title);
}

CefRefPtr<ClientHandler> g_handler;
