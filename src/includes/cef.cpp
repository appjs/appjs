#include "include/cef_app.h"
#include "includes/cef_scheme_handler.h"
#include "includes/cef_handler.h"
#include "includes/cef.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

bool Cef::initialized_;

void Cef::Init(Settings* initSettings) {
  if (!Cef::initialized_) {
    CefLoop::Init();
    CefRefPtr<CefApp> app;
    CefSettings settings;

    CefString(&settings.pack_file_path) = initSettings->getString("chromePakPath", "");
    CefString(&settings.locales_dir_path) = initSettings->getString("localsPakPath", "");
    CefString(&settings.javascript_flags) = initSettings->getString("jsFlags", "");
    settings.log_severity = (cef_log_severity_t)initSettings->getInteger("logLevel", LOGSEVERITY_DISABLE);
    settings.multi_threaded_message_loop = false;

    g_handler = new ClientHandler();
    CefInitialize(settings, app);

    // http://code.google.com/p/chromiumembedded/issues/detail?id=404
    CefRegisterSchemeHandlerFactory("http", "appjs", new AppjsSchemeHandlerFactory());

    CefBase::Init();
    Cef::initialized_ = true;
  }

};

} /* appjs */
