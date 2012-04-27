#include "include/cef_app.h"
#include "includes/cef_handler.h"
#include "includes/cef_base_win.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

void CefBase::Init() {

};

void Cef::AddWebView(CefWindowHandle hWndParent,RECT windowRect) {

  CefWindowInfo window_info;
  CefBrowserSettings browserSettings;

  window_info.SetAsChild(hWndParent, windowRect);

  CefBrowser::CreateBrowserSync(window_info,
                                static_cast<CefRefPtr<CefClient> >(g_handler),
                                "http://www.google.com", browserSettings);

}

} /* appjs */

