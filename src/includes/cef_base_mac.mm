#import <Cocoa/Cocoa.h>
#include "include/cef_app.h"
#include "includes/cef_handler.h"
#include "includes/cef_base_mac.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

void CefBase::Init(){}

void CefBase::AddWebView(CefWindowHandle parent, char* url, Settings* settings) {
  CefWindowInfo windowInfo;
  if (settings->getBoolean("alpha",false)) {
    windowInfo.SetTransparentPainting(true);
  }
  g_handler->browserSettings_.web_security_disabled = settings->getBoolean("disableSecurity", false);
  NSRect contentFrame = [parent frame];
  windowInfo.SetAsChild(parent, 0, 0, contentFrame.size.width, contentFrame.size.height);
  CefBrowser::CreateBrowser(windowInfo, g_handler.get(), url, g_handler->browserSettings_);
}

} /* appjs */
