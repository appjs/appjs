#include <node.h>
#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "includes/cef_handler.h"
#include "includes/cef_sync_handler.h"
#include "appjs_window.h"

using namespace v8;
using namespace appjs;

ClientHandler::ClientHandler()
  : m_MainHwnd(NULL),
    m_BrowserHwnd(NULL) {
}

ClientHandler::~ClientHandler() {
}



Handle<Object> ClientHandler::GetV8WindowHandle(CefRefPtr<CefBrowser> browser) {
  return ClientHandler::GetWindow(browser)->GetV8Handle();
}

Handle<Object> ClientHandler::CreatedBrowser(CefRefPtr<CefBrowser> browser) {
  NativeWindow* window = ClientHandler::GetWindow(browser);
  window->SetBrowser(browser);
  return window->GetV8Handle();
}


void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  REQUIRE_UI_THREAD();

  AutoLock lock_scope(this);

  if (!browser->IsPopup()) {
    // Set main browser of the application
    if (!m_Browser.get()) {
      m_Browser = browser;
      m_BrowserHwnd = browser->GetWindowHandle();
    }

    Handle<Object> handle = ClientHandler::CreatedBrowser(browser);
    Handle<Value> argv[1] = {String::New("create")};
    node::MakeCallback(handle,"emit", 1, argv);
  }
}

void ClientHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
  REQUIRE_UI_THREAD();
  if (!browser->IsPopup()) {
    context->Enter();
    CefRefPtr<CefV8Value> appjsObj = CefV8Value::CreateObject(NULL);
    CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("send", new AppjsSyncHandler(browser));
    context->GetGlobal()->SetValue("appjs", appjsObj, V8_PROPERTY_ATTRIBUTE_NONE);
    appjsObj->SetValue("send", func, V8_PROPERTY_ATTRIBUTE_NONE);
    context->Exit();
  }
}

bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser) {
  REQUIRE_UI_THREAD();

  if (!browser->IsPopup() && m_BrowserHwnd == browser->GetWindowHandle()) {
    // Since the main window contains the browser window, we need to close
    // the parent window instead of the browser window.
    m_Browser = NULL;
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
  if(!browser->IsPopup()) {

#ifndef __LINUX__
    Handle<Object> handle = ClientHandler::GetV8WindowHandle(browser);
    Handle<Value> argv[1] = {String::New("close")};
    node::MakeCallback(handle,"emit",1,argv);
#endif

    if (m_BrowserHwnd == browser->GetWindowHandle()) {
      Local<Object> global = Context::GetCurrent()->Global();
      Local<Object> process = global->Get(String::NewSymbol("process"))->ToObject();
      Local<Object> emitter = Local<Object>::Cast(process->Get(String::NewSymbol("AppjsEmitter")));

      Handle<Value> exitArgv[1] = {String::New("exit")};
      node::MakeCallback(emitter,"emit",1,exitArgv);
      DoClose(browser);
    }

#ifndef __LINUX__
    delete ClientHandler::GetWindow(browser);
#endif
  }
}

void ClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int httpStatusCode)
{
  REQUIRE_UI_THREAD();

  if (!browser->IsPopup()) {
    const int argc = 1;
    Handle<Object> handle = ClientHandler::GetV8WindowHandle(browser);
    Handle<Value> argv[argc] = {String::New("ready")};
    node::MakeCallback(handle,"emit",argc,argv);
  }
}

void ClientHandler::SetMainHwnd(CefWindowHandle& hwnd) {
  AutoLock lock_scope(this);

  m_MainHwnd = hwnd;
}
