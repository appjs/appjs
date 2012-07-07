#include <node.h>
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "includes/cef_handler.h"
#include "includes/cef_sync_handler.h"
#include "appjs_window.h"

using namespace v8;
using namespace appjs;
/*
Handle<Value> EmitReady(const Arguments& args) {
  HandleScope scope;

  Local<Object> global = Context::GetCurrent()->Global();
  Local<Object> process = global->Get(String::NewSymbol("process"))->ToObject();
  Local<Object> emitter = Local<Object>::Cast(process->Get(String::NewSymbol("AppjsEmitter")));

  const int argc = 1;
  Handle<Value> argv[argc] = {String::New("window_ready")};
  node::MakeCallback(emitter,"emit",argc,argv);

  return scope.Close(Undefined());
}*/

ClientHandler::ClientHandler()
  : m_MainHwnd(NULL),
    m_BrowserHwnd(NULL) {
}

ClientHandler::~ClientHandler() {
}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  REQUIRE_UI_THREAD();

  AutoLock lock_scope(this);

  // Set main browser of the application
  if (!m_Browser.get()) {
    m_Browser = browser;
    m_BrowserHwnd = browser->GetWindowHandle();
  }

  const int argc = 1;
  Handle<Object> handle = this->GetV8WindowHandle(browser);
  Handle<Value> argv[argc] = {String::New("create")};
  node::MakeCallback(handle,"emit",argc,argv);
}

void ClientHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
  REQUIRE_UI_THREAD();
  context->Enter();
  CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("sendSync", new AppjsSyncHandler(browser));
  context->GetGlobal->Set("sendSync", func);
  context->Exit();
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

// There is a bug in CEF for Linux I think that there is no window object
// when the code reaches here.
#if not defined(__LINUX__)
  const int argc = 1;
  Handle<Object> handle = this->GetV8WindowHandle(browser);
  Handle<Value> argv[argc] = {String::New("close")};
  node::MakeCallback(handle,"emit",argc,argv);
#endif

  if (m_BrowserHwnd == browser->GetWindowHandle()) {
    // Free the browser pointer so that the browser can be destroyed
    m_Browser = NULL;

    Local<Object> global = Context::GetCurrent()->Global();
    Local<Object> process = global->Get(String::NewSymbol("process"))->ToObject();
    Local<Object> emitter = Local<Object>::Cast(process->Get(String::NewSymbol("AppjsEmitter")));

    const int argc = 1;
    Handle<Value> argv[argc] = {String::New("exit")};
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

    const int argc = 1;
    Handle<Object> handle = this->GetV8WindowHandle(browser);
    Handle<Value> argv[argc] = {String::New("ready")};
    node::MakeCallback(handle,"emit",argc,argv);

  }
}


void ClientHandler::SetMainHwnd(CefWindowHandle& hwnd) {
  AutoLock lock_scope(this);

  m_MainHwnd = hwnd;
}
