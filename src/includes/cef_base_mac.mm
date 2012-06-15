#include "include/cef_app.h"
#include "includes/cef_handler.h"
#include "includes/cef_base_mac.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

void CefBase::Init() {

};

void CefBase::AddWebView(CefWindowHandle ParentWidget,char* url,Settings* settings) {

  CefWindowInfo window_info;
  CefBrowserSettings browser_settings;

  browser_settings.web_security_disabled = settings->getBoolean("disableSecurity",false);
  
  int width = settings->getNumber("width",800);
  int height = settings->getNumber("height",600);
  
  window_info.SetAsChild(ParentWidget,0,0,width,height);

  CefBrowser::CreateBrowserSync(window_info,
                                g_handler.get(),
                                url, browser_settings);

  if(!g_handler->GetBrowser().get()) {
    g_handler->SetMainHwnd(ParentWidget);
  }
}


} /* appjs */

