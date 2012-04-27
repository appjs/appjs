#include <node.h>
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "includes/cef_handler.h"

using namespace v8;

ClientHandler::ClientHandler()
  : m_MainHwnd(NULL),
    m_BrowserHwnd(NULL) {
}

ClientHandler::~ClientHandler() {
}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  REQUIRE_UI_THREAD();

  AutoLock lock_scope(this);
  if (!m_Browser.get())   {
    // We need to keep the main child window, but not popup windows

    m_Browser = browser;
    m_BrowserHwnd = browser->GetWindowHandle();

    Local<Object> global = Context::GetCurrent()->Global();
    Local<Object> process = global->Get(String::NewSymbol("process"))->ToObject();
    Local<Object> emitter = Local<Object>::Cast(process->Get(String::NewSymbol("AppjsEmitter")));

    const int argc = 1;
    Handle<Value> argv[argc] = {String::New("window_ready")};
    node::MakeCallback(emitter,"emit",argc,argv);

  }
}

void ClientHandler::OnContextCreated(CefRefPtr<CefBrowser> browser,CefRefPtr<CefFrame> frame,CefRefPtr<CefV8Context> context) {
  REQUIRE_UI_THREAD();
  ///CefRefPtr<CefV8Value> ret;
  //CefRefPtr<CefV8Exception> exc;
  //CefRefPtr<CefV8Value> global = context->GetGlobal();

  //CefRefPtr<CefV8Value> m = static_cast<CefV8Value*>(&cb);
  
  //CefRefPtr<CefV8Value> m = static_cast<CefV8Value*>(v8::External::Unwrap(cb));

  //m->ExecuteFunctionWithContext(context,global,{},ret,exc,false);
//  fprintf(stderr, "%d\n",cb->IsFunction());
}

bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser) {
  REQUIRE_UI_THREAD();

  if (m_BrowserHwnd == browser->GetWindowHandle()) {
    // Since the main window contains the browser window, we need to close
    // the parent window instead of the browser window.
    CloseMainWindow();

    // Return true here so that we can skip closing the browser window
    // in this pass. (It will be destroyed due to the call to close
    // the parent above.)
    return true;
  }

  // A popup browser window is not contained in another window, so we can let
  // these windows close by themselves.
  return false;
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  REQUIRE_UI_THREAD();

  if (m_BrowserHwnd == browser->GetWindowHandle()) {
    // Free the browser pointer so that the browser can be destroyed
    m_Browser = NULL;

    Local<Object> global = Context::GetCurrent()->Global();
    Local<Object> process = global->Get(String::NewSymbol("process"))->ToObject();
    Local<Object> emitter = Local<Object>::Cast(process->Get(String::NewSymbol("AppjsEmitter")));

    const int argc = 1;
    Handle<Value> argv[argc] = {String::New("window_close")};
    node::MakeCallback(emitter,"emit",argc,argv);

    DoClose(browser);
  }
}

void ClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int httpStatusCode) 
{
  REQUIRE_UI_THREAD();

  if (m_Browser.get()) {

    Local<Object> global = Context::GetCurrent()->Global();
    Local<Object> process = global->Get(String::NewSymbol("process"))->ToObject();
    Local<Object> emitter = Local<Object>::Cast(process->Get(String::NewSymbol("AppjsEmitter")));

    const int argc = 2;
    Handle<Value> argv[argc] = {String::New("ready"),Number::New(httpStatusCode)};
    node::MakeCallback(emitter,"emit",argc,argv);

  }
}


void ClientHandler::SetMainHwnd(CefWindowHandle hwnd) {
  AutoLock lock_scope(this);
  m_MainHwnd = hwnd;
}
