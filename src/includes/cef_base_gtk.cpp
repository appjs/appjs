#include "include/cef_app.h"
#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "cef_handler.h"
#include "cef_base_gtk.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

void CefBase::Init() {
      
  g_thread_init (NULL);
  gdk_threads_init ();
  gtk_init(NULL,NULL);

};

void CefBase::AddWebView(CefWindowHandle ParentWidget,char* url) {

  CefWindowInfo window_info;
  CefBrowserSettings browserSettings;

  window_info.SetAsChild(ParentWidget);

  CefBrowser::CreateBrowserSync(window_info,
                                static_cast<CefRefPtr<CefClient> >(g_handler),
                                url, browserSettings);
  g_handler->SetMainHwnd(ParentWidget);
}


} /* appjs */

