#include "include/cef_browser.h"
#include "includes/cef.h"
#include "includes/cef_handler.h"

using namespace v8;
using namespace appjs;

CefWindowHandle ClientHandler::GetMainHwnd(){
  return m_MainHwnd;
}

NativeWindow* ClientHandler::GetWindow(CefWindowHandle handle){
  LONG_PTR win = GetWindowLongPtr(handle, GWLP_USERDATA);
  return win ? (NativeWindow*)win : NULL;
}

NativeWindow* ClientHandler::GetWindow(CefRefPtr<CefBrowser> browser){
  return ClientHandler::GetWindow(GetParent(browser->GetWindowHandle()));
}


void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title)
{
  REQUIRE_UI_THREAD();
  // Set the frame window title bar
  CefWindowHandle hwnd = browser->GetWindowHandle();
  if (browser->IsPopup()) {
    // The frame window will be the parent of the browser window
    hwnd = GetParent(hwnd);
  }
  // TODO check to see if this type cast works in x86 systems.
  SetWindowText(hwnd,std::basic_string<TCHAR>(title).c_str());
}

bool ClientHandler::OnKeyEvent(CefRefPtr<CefBrowser> browser, KeyEventType type, int code,
                               int modifiers, bool isSystemKey, bool isAfterJavaScript) {

  if (!browser->IsPopup() && type == KEYEVENT_RAWKEYDOWN && code == VK_F12) {
    browser->ShowDevTools();
    return true;
  }
  return false;
};

void ClientHandler::OnContentsSizeChange(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    int width,
                                    int height)
{
  REQUIRE_UI_THREAD();
  NativeWindow* window = ClientHandler::GetWindow(browser);
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
  PostMessage(m_MainHwnd, WM_CLOSE, 0, 0);
  appjs::Cef::Shutdown();
}

CefRefPtr<ClientHandler> g_handler;
