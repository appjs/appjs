#include <node.h>
#include "base/logging.h"
#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "includes/cef_handler.h"
#include "includes/cef_sync_handler.h"
#include "native_window/native_window.h"
#include <node_internals.h>
#include "appjs_window.h"

using namespace v8;
using namespace appjs;

CefRefPtr<ClientHandler> g_handler;

int windowCount = 0;

ClientHandler::ClientHandler(CefBrowserSettings browserSettings)
  : mainBrowserHandle(NULL),
  browserSettings_(browserSettings) {
}

ClientHandler::~ClientHandler() {
}


void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser>  browser) {
  REQUIRE_UI_THREAD();
  AutoLock lock_scope(this);
  if (!browser->IsPopup()) {
    if (!mainBrowserHandle.get()) {
      mainBrowserHandle = browser->GetHost();
    }
    windowCount++;

    NativeWindow* window = NativeWindow::GetWindow(mainBrowserHandle);
    window->SetBrowser(mainBrowserHandle);
    window->Emit("create");
  }
}

bool ClientHandler::HasMainWindow() {
  return mainBrowserHandle != NULL && mainBrowserHandle.get() != NULL;
}

void ClientHandler::Shutdown() {
  if ( HasMainWindow() ) {
    mainBrowserHandle->CloseBrowser(false);
  }
}

void ClientHandler::OnContextCreated(CefRefPtr<CefBrowserHost>  browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
  if (!browser->GetBrowser()->IsPopup() && frame->IsMain()) {
    context->Enter();
    CefRefPtr<CefV8Value> appjsObj = CefV8Value::CreateObject(NULL);
    CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("send", new AppjsSyncHandler(browser));
    context->GetGlobal()->SetValue("appjs", appjsObj, V8_PROPERTY_ATTRIBUTE_NONE);
    appjsObj->SetValue("send", func, V8_PROPERTY_ATTRIBUTE_NONE);
    context->Exit();
    NativeWindow::GetWindow(browser)->Emit("context-created");
  }
}

void ClientHandler::OnContextReleased(CefRefPtr<CefBrowserHost>  browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
  /*REQUIRE_UI_THREAD();
  if (!browser->IsPopup() && frame->IsMain()) {
    NativeWindow::GetWindow(browser)->Emit("context-released");
  }*/
}

bool ClientHandler::DoClose(CefRefPtr<CefBrowser>  browser) {
  REQUIRE_UI_THREAD();

  if (!browser->IsPopup() && --windowCount == 0) {
    Local<Object> global = Context::GetCurrent()->Global();
    Local<Object> emitter = global->Get(String::NewSymbol("process"))->ToObject();
    v8::Handle<Value> exitArgv[1] = {String::New("appjs-exit")};
    node::MakeCallback(emitter,"emit",1,exitArgv);
    Cef::Shutdown();
    mainBrowserHandle = NULL;
    return true;
  } else {
    return false;
  }
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser>  browser) {
  REQUIRE_UI_THREAD();

  if(!browser->IsPopup()) {
    NativeWindow* window = NativeWindow::GetWindow(browser->GetHost());
    window->PrepareClose();
    //DoClose(browser);
#ifdef __WIN__
    delete window;
#endif
  }
}

void ClientHandler::OnLoadEnd(CefRefPtr<CefBrowser>  browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
  REQUIRE_UI_THREAD();
  //v8::Locker lock(node::node_isolate);
  v8::HandleScope scope;
  //node::node_context->Enter();
  //Local<Object> window = Local<Object>::Cast(node::node_context->Global()->Get(v8::String::New("Window")));
  if (!browser->IsPopup() && frame->IsMain()) {
    NativeWindow::GetWindow(browser->GetHost())->Emit("ready");
  }
}

void ClientHandler::OnLoadStart(CefRefPtr<CefBrowser>  browser, CefRefPtr<CefFrame> frame) {
  REQUIRE_UI_THREAD();
  if (!browser->IsPopup() && frame->IsMain()) {
    NativeWindow::GetWindow(browser->GetHost())->Emit("reloading");
  }
}

void ClientHandler::OnContentsSizeChange(CefRefPtr<CefBrowserHost>  browser, CefRefPtr<CefFrame> frame, int width, int height) {
  REQUIRE_UI_THREAD();
  if (!browser->GetBrowser()->IsPopup() && frame->IsMain()) {
    NativeWindow* window = NativeWindow::GetWindow(browser);
    if (window != NULL && window->GetAutoResize()) {
      window->Resize(width, height);
    }
  }
}

void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser>  browser, const CefString& title) {
  REQUIRE_UI_THREAD();
  if (!browser->IsPopup()) {
    tstring titleStr(title);
    NativeWindow::GetWindow(browser->GetHost())->SetTitle(titleStr.c_str());
  }
}
