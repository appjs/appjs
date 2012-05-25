#include "include/cef_browser.h"
#include "includes/cef.h"
#include "includes/cef_handler.h"

CefWindowHandle ClientHandler::GetMainHwnd(){
  return m_MainHwnd;
}

void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title)
{
  REQUIRE_UI_THREAD();
  // Set the frame window title bar
  CefWindowHandle hwnd = browser->GetWindowHandle();
  if (m_BrowserHwnd == hwnd)   {
    // The frame window will be the parent of the browser window
    hwnd = GetParent(hwnd);
  }
  // TODO check to see if this type cast works in x86 systems.
  SetWindowText(hwnd,std:basic_string<TCHAR>(title).c_str());

}

void ClientHandler::OnContentsSizeChange(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame, 
                                    int width, 
                                    int height)
{
  REQUIRE_UI_THREAD();

}
void ClientHandler::CloseMainWindow() {
  
  REQUIRE_UI_THREAD();
  ::PostMessage(m_MainHwnd, WM_CLOSE, 0, 0);
  appjs::Cef::Shutdown();
}

CefRefPtr<ClientHandler> g_handler;
