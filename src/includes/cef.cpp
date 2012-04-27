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

    settings.pack_loading_disabled = true;
    settings.multi_threaded_message_loop = false;

    g_handler = new ClientHandler();
    CefInitialize(settings, app);

    CefRegisterCustomScheme("appjs", true, false, false);
    CefRegisterSchemeHandlerFactory("appjs", "",
        new AppjsSchemeHandlerFactory());

    CefBase::Init();
    Cef::initialized_ = true;
  }

};

} /* appjs */

