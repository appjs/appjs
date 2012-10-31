#include <node.h>
#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "includes/cef_handler.h"
#include "includes/cef_sync_handler.h"
#include "native_window/native_window.h"
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


void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  REQUIRE_UI_THREAD();
  AutoLock lock_scope(this);

  if (!browser->IsPopup()) {
    if (!mainBrowserHandle.get()) {
      mainBrowserHandle = browser;
    }
    windowCount++;
    NativeWindow* window = NativeWindow::GetWindow(browser);
    window->SetBrowser(browser);
    window->Emit("create");
  }
}

bool ClientHandler::HasMainWindow() {
  return mainBrowserHandle != NULL && mainBrowserHandle.get() != NULL;
}

void ClientHandler::Shutdown() {
  if ( HasMainWindow() ) {
    mainBrowserHandle->CloseBrowser();
  }
}

void ClientHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
  REQUIRE_UI_THREAD();
  if (!browser->IsPopup() && frame->IsMain()) {
    context->Enter();
    CefRefPtr<CefV8Value> appjsObj = CefV8Value::CreateObject(NULL);
    CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("send", new AppjsSyncHandler(browser));
    context->GetGlobal()->SetValue("appjs", appjsObj, V8_PROPERTY_ATTRIBUTE_NONE);
    appjsObj->SetValue("send", func, V8_PROPERTY_ATTRIBUTE_NONE);
    context->Exit();
    NativeWindow::GetWindow(browser)->Emit("context-created");
  }
}

void ClientHandler::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
  REQUIRE_UI_THREAD();
  if (!browser->IsPopup() && frame->IsMain()) {
    NativeWindow::GetWindow(browser)->Emit("context-released");
  }
}

bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser) {
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

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  REQUIRE_UI_THREAD();

  if(!browser->IsPopup()) {
    NativeWindow* window = NativeWindow::GetWindow(browser);
    window->PrepareClose();
    DoClose(browser);
#ifdef __WIN__
    delete window;
#endif
  }
}

void ClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
  REQUIRE_UI_THREAD();
  if (!browser->IsPopup() && frame->IsMain()) {
    NativeWindow::GetWindow(browser)->Emit("ready");
  }
}

void ClientHandler::OnContentsSizeChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int width, int height) {
  REQUIRE_UI_THREAD();
  if (!browser->IsPopup() && frame->IsMain()) {
    NativeWindow* window = NativeWindow::GetWindow(browser);
    if (window != NULL && window->GetAutoResize()) {
      window->Resize(width, height);
    }
  }
}

void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
  REQUIRE_UI_THREAD();
  if (!browser->IsPopup()) {
    tstring titleStr(title);
    NativeWindow::GetWindow(browser)->SetTitle(titleStr.c_str());
  }
}
