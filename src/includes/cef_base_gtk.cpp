#include "include/cef_app.h"
#include "includes/cef_handler.h"
#include "includes/cef_base_gtk.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

static void destroyHandler(int status = 0) {
  g_handler->Shutdown();
}

void CefBase::Init() {
  g_thread_init(NULL);
  gdk_threads_init();
  gtk_init(NULL,NULL);
}

void CefBase::AddWebView(CefWindowHandle& parent, char* url, Settings* settings) {
  CefWindowInfo windowInfo;
  windowInfo.SetAsChild(parent);
  g_handler->browserSettings_.web_security_disabled = settings->getBoolean("disableSecurity", false);
  CefBrowser::CreateBrowser(windowInfo, static_cast<CefRefPtr<CefClient>>(g_handler), url, g_handler->browserSettings_);

  if (!g_handler->HasMainWindow()) {
    signal(SIGINT, destroyHandler);
    signal(SIGTERM, destroyHandler);
  }
}

} /* appjs */
