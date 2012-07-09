#include "include/cef_browser.h"
#include "includes/cef.h"
#include "includes/cef_handler.h"


using namespace v8;
using namespace appjs;

CefWindowHandle ClientHandler::GetMainHwnd(){
  return m_MainHwnd;
}


Handle<Object> ClientHandler::CreatedBrowser(CefRefPtr<CefBrowser> browser) {
  CefWindowHandle handle = GetParent(browser->GetWindowHandle());
  NativeWindow* window = (NativeWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
  window->SetBrowser(browser);
  return window->GetV8Handle();
}

Handle<Object> ClientHandler::GetV8WindowHandle(CefRefPtr<CefBrowser> browser) {
  CefWindowHandle handle = GetParent(browser->GetWindowHandle());
  NativeWindow* window = (NativeWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
  return window->GetV8Handle();
}

NativeWindow* ClientHandler::WindowFromHandle(CefWindowHandle handle){
   return (NativeWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
}

void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title)
{
  REQUIRE_UI_THREAD();
  // Set the frame window title bar
  CefWindowHandle hwnd = browser->GetWindowHandle();
  if (m_BrowserHwnd == hwnd) {
    // The frame window will be the parent of the browser window
    hwnd = GetParent(hwnd);
  }
  // TODO check to see if this type cast works in x86 systems.
  SetWindowText(hwnd,std::basic_string<TCHAR>(title).c_str());
}

void ClientHandler::OnContentsSizeChange(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    int width,
                                    int height)
{
  REQUIRE_UI_THREAD();
  LONG_PTR handle = GetWindowLongPtr(GetParent(browser->GetWindowHandle()), GWLP_USERDATA);
  if (handle) {
    NativeWindow* window = (NativeWindow*)handle;

    if (window != NULL && window->auto_resize) {
      RECT rect;
      GetClientRect(window->handle_, &rect);

      HDWP hdwp = BeginDeferWindowPos(1);
      hdwp = DeferWindowPos(hdwp, window->handle_, NULL, rect.left, rect.top, width, height, SWP_NOZORDER);
      EndDeferWindowPos(hdwp);
    }
  }
}
void ClientHandler::CloseMainWindow() {
  REQUIRE_UI_THREAD();
  PostMessage(m_MainHwnd, WM_CLOSE, 0, 0);
  appjs::Cef::Shutdown();
}

CefRefPtr<ClientHandler> g_handler;
