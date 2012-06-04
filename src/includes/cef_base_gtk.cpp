#include "include/cef_app.h"
#include "includes/cef_handler.h"
#include "includes/cef_base_gtk.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {


static void destroy_handler (int status = 0) {
  g_handler->GetBrowser()->CloseBrowser();
};

void CefBase::Init() {

  g_thread_init (NULL);
  gdk_threads_init ();
  gtk_init(NULL,NULL);

};

void CefBase::AddWebView(CefWindowHandle& ParentWidget,char* url,Settings* settings) {

  CefWindowInfo window_info;
  CefBrowserSettings browser_settings;

  browser_settings.web_security_disabled = settings->getBoolean("disableSecurity",false);

  window_info.SetAsChild(ParentWidget);

  CefBrowser::CreateBrowser(window_info,
                                static_cast<CefRefPtr<CefClient> >(g_handler),
                                url, browser_settings);

  if(!g_handler->GetBrowser().get()){
    g_handler->SetMainHwnd(ParentWidget);

    // Install a signal handler so we clean up after ourselves.
    signal(SIGINT, destroy_handler);
    signal(SIGTERM, destroy_handler);
  }
}


} /* appjs */

