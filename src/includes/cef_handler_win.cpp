#include "include/cef_browser.h"
#include "includes/cef.h"
#include "includes/cef_handler.h"

using namespace v8;
using namespace appjs;

NativeWindow* ClientHandler::GetWindow(CefWindowHandle handle){
  LONG_PTR win = GetWindowLongPtr(handle, GWLP_USERDATA);
  return win ? (NativeWindow*)win : NULL;
}

CefWindowHandle ClientHandler::GetContainer(CefRefPtr<CefBrowser> browser){
  return GetParent(browser->GetWindowHandle());
}

CefRefPtr<ClientHandler> g_handler;
