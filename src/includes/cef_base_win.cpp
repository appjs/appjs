#include "include/cef_app.h"
#include "includes/cef_handler.h"
#include "includes/cef_base_win.h"
#include "includes/util.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

void CefBase::Init() {

};

void CefBase::AddWebView(CefWindowHandle hWndParent,RECT windowRect,char* url,Settings* settings) {

  CefWindowInfo window_info;
  CefBrowserSettings browserSettings;

  browserSettings.web_security_disabled = settings->getBoolean("disableSecurity", false);

  window_info.SetAsChild(hWndParent, windowRect);
  if (settings->getBoolean("alpha", false)) {
    window_info.SetTransparentPainting(true);
  }


  CefBrowser::CreateBrowser(window_info,
                                static_cast<CefRefPtr<CefClient> >(g_handler),
                                url, browserSettings);

  if(!g_handler->GetBrowser().get()){
    g_handler->SetMainHwnd(hWndParent);
  }

}

} /* appjs */

