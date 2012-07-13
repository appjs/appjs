#include "include/cef_app.h"
#include "includes/cef_scheme_handler.h"
#include "includes/cef_handler.h"
#include "includes/cef.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

bool Cef::initialized_;

void Cef::Init(char* locales_dir) {

  if( !Cef::initialized_ ) {
    CefLoop::Init();

    CefSettings settings;
    CefRefPtr<CefApp> app;

    char* localesPath = new char[strlen(locales_dir)+9];
    char* packPath = new char[strlen(locales_dir)+12];

    sprintf(localesPath, "%s%s", locales_dir,"/locales");
    sprintf(packPath,"%s%s",locales_dir,"/chrome.pak");
    CefString(&settings.pack_file_path) = packPath;
    CefString(&settings.locales_dir_path) = localesPath;
    CefString(&settings.javascript_flags) = " --harmony_proxies --harmony_collections --harmony_scoping";

    settings.log_severity = LOGSEVERITY_DISABLE;
    settings.multi_threaded_message_loop = false;

    g_handler = new ClientHandler();
    CefInitialize(settings, app);

    // Accoding to this thread [1], I changed appjs scheme to
    // normal http scheme with a special domain name.
    // [1] http://code.google.com/p/chromiumembedded/issues/detail?id=404

    //CefRegisterCustomScheme("appjs", true, false, false);
    CefRegisterSchemeHandlerFactory("http", "appjs",
        new AppjsSchemeHandlerFactory());

    CefBase::Init();
    Cef::initialized_ = true;
  }

};

} /* appjs */

