#include "include/cef_app.h"
#include "includes/cef_scheme_handler.h"
#include "includes/cef_handler.h"
#include "includes/cef.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

bool Cef::initialized_;

void Cef::Init() {

  if( !Cef::initialized_ ) {
    CefLoop::Init();

    CefSettings settings;
    CefRefPtr<CefApp> app;

    // TODO: Use settings.pack_file_path to set the correct chrome.pak path
    // since it searches in execute path ( where node exists ) to find
    // the pack file in linux and it won't work.
    // settings.pack_file_path = filePath;
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

