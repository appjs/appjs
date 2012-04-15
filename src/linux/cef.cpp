#include <gtk/gtk.h>
#include "include/cef_app.h"
#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "cef/client_handler.h"
#include "linux/cef.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

bool Cef::initialized_;

void Cef::Init() {

  if( !Cef::initialized_ ) {
    CefLoop::Init();
        
    g_thread_init (NULL);
    gdk_threads_init ();
    gtk_init(NULL,NULL);
    
    CefSettings settings;
    CefRefPtr<CefApp> app;

    settings.pack_loading_disabled = true;
    settings.multi_threaded_message_loop = false;

    g_handler = new ClientHandler();

    CefInitialize(settings, app);
      
    Cef::initialized_ = true;
  }

};

void Cef::AddWebView(CefWindowHandle ParentWidget) {

  CefWindowInfo window_info;
  CefBrowserSettings browserSettings;

  window_info.SetAsChild(ParentWidget);

  CefBrowser::CreateBrowserSync(window_info,
                                static_cast<CefRefPtr<CefClient> >(g_handler),
                                "http://www.google.com", browserSettings);

}

} /* appjs */

