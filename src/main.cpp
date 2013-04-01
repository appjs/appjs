#include "node.h"
#include "node_internals.h"
#include "include/cef_app.h"
#undef ARRAY_SIZE
#include "includes/client_app.h"
#include "includes/cef_handler.h"
#include "includes/cef_scheme_handler.h"

using namespace appjs;

int main(int argc, char *argv[]) {

  CefRefPtr<ClientApp> app(new ClientApp);
  CefMainArgs main_args(argc,argv);

  int exit_code = CefExecuteProcess(main_args, app.get());
  if(exit_code >= 0 )
    return exit_code;

  node::InitUv(argc,argv);
  CefSettings o;
  //CefString(&o.locales_dir_path) = "cef_resources.pak";
  o.remote_debugging_port = 8081;
  //CefString(&o.locale)           = "cef_resources.pak";
  /*CefString(&o.resources_dir_path)   = O_STRING(ChromePakPath);
  CefString(&o.locales_dir_path) = O_STRING(LocalesPakPath);
  CefString(&o.log_file)         = O_STRING(LogFilePath);
  CefString(&o.cache_path)       = O_STRING(CachePath);
  CefString(&o.javascript_flags) = O_STRING(JSFlags);
  CefString(&o.user_agent)       = O_STRING(UserAgent);
  CefString(&o.product_version)  = O_STRING(ProductVersion);
  CefString(&o.locale)           = O_STRING(Locale);
*/
  CefString(&o.javascript_flags) = "--harmony_proxies --harmony_collections --harmony_scoping";
  CefString(&o.log_file)         = "appjs.log";
  //o.pack_loading_disabled        = true;//O_ENABLE(PakLoading);
  o.multi_threaded_message_loop  = false;
  o.single_process = false;
  o.log_severity = LOGSEVERITY_VERBOSE;

  CefInitialize(main_args, o, app.get());

  // http://code.google.com/p/chromiumembedded/issues/detail?id=404
  CefRegisterSchemeHandlerFactory("http", "appjs", new AppjsSchemeHandlerFactory());
  CefRunMessageLoop();
  CefShutdown();
  return 0;
  //return node::Start(argc, argv);
}