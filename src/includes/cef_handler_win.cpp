#include "include/cef_browser.h"
#include "includes/cef.h"
#include "includes/cef_handler.h"


using namespace v8;
using namespace appjs;

CefWindowHandle ClientHandler::GetMainHwnd(){
  return m_MainHwnd;
}


Handle<Object> ClientHandler::CreatedBrowser(CefRefPtr<CefBrowser> browser) {
  CefWindowHandle window = GetParent(browser->GetWindowHandle());
  MainWindow* mainwindow = (MainWindow*)GetWindowLongPtr(window, GWLP_USERDATA);
  mainwindow->setBrowser(browser);
  return mainwindow->getV8Handle();
}

Handle<Object> ClientHandler::GetV8WindowHandle(CefRefPtr<CefBrowser> browser) {
  CefWindowHandle window = GetParent(browser->GetWindowHandle());
  MainWindow* mainwindow = ((MainWindow*)GetWindowLongPtr(window, GWLP_USERDATA));
  return mainwindow->getV8Handle();
}

MainWindow* ClientHandler::WindowFromHandle(CefWindowHandle handle){
   return (MainWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
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
  SetWindowText(hwnd,std::basic_string<TCHAR>(title).c_str());

}

void ClientHandler::OnContentsSizeChange(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    int width,
                                    int height)
{
  REQUIRE_UI_THREAD();

  if( this->m_AutoResize ) {
    HWND window = GetParent(browser->GetWindowHandle());
    RECT rect;
    GetClientRect(window, &rect);

    HDWP hdwp = BeginDeferWindowPos(1);
    hdwp = DeferWindowPos(hdwp, window, NULL,
         rect.left, rect.top, width, height,
         SWP_NOZORDER);
    EndDeferWindowPos(hdwp);
  }

}
void ClientHandler::CloseMainWindow() {

  REQUIRE_UI_THREAD();
  ::PostMessage(m_MainHwnd, WM_CLOSE, 0, 0);
  appjs::Cef::Shutdown();
}

CefRefPtr<ClientHandler> g_handler;
