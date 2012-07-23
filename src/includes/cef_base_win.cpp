#include "include/cef_app.h"
#include "includes/cef_handler.h"
#include "includes/cef_base_win.h"
#include "includes/util.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

void CefBase::Init() {

};

void CefBase::AddWebView(CefWindowHandle parent, RECT windowRect, char* url, Settings* settings) {

  CefWindowInfo windowInfo;
  CefBrowserSettings browserSettings;

  browserSettings.web_security_disabled = settings->getBoolean("disableSecurity", false);

  windowInfo.SetAsChild(parent, windowRect);
  windowInfo.SetTransparentPainting(true);

  if(!g_handler->GetBrowser().get()){
    g_handler->SetMainHwnd(parent);
  }
  CefBrowser::CreateBrowser(windowInfo, static_cast<CefRefPtr<CefClient>>(g_handler), url, browserSettings);
}

} /* appjs */

