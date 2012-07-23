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
  : mainHandle(NULL),
    mainBrowserHwnd(NULL) {
}

ClientHandler::~ClientHandler() {
}



Handle<Object> ClientHandler::GetNodeWindow(CefRefPtr<CefBrowser> browser) {
  return GetWindow(browser)->GetV8Handle();
}

Handle<Object> ClientHandler::CreatedBrowser(CefRefPtr<CefBrowser> browser) {
  NativeWindow* window = GetWindow(browser);
  window->SetBrowser(browser);
  return window->GetV8Handle();
}

NativeWindow* ClientHandler::GetWindow(CefRefPtr<CefBrowser> browser){
  return GetWindow(GetContainer(browser));
}


void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  REQUIRE_UI_THREAD();
  AutoLock lock_scope(this);

  if (!browser->IsPopup()) {
    if (!mainBrowser.get()) {
      mainBrowser = browser;
      mainBrowserHwnd = browser->GetWindowHandle();
    }

    CreatedBrowser(browser);
    GetWindow(browser)->Emit("create");
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
    GetWindow(browser)->Emit("context-created");
  }
}

void ClientHandler::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
  REQUIRE_UI_THREAD();
  if (!browser->IsPopup()) {
    GetWindow(browser)->Emit("context-released");
  }
}

bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser) {
  REQUIRE_UI_THREAD();

  if (!browser->IsPopup() && mainBrowserHwnd == browser->GetWindowHandle()) {

    Local<Object> global = Context::GetCurrent()->Global();
    Local<Object> process = global->Get(String::NewSymbol("process"))->ToObject();
    Local<Object> emitter = Local<Object>::Cast(process->Get(String::NewSymbol("AppjsEmitter")));
    Handle<Value> exitArgv[1] = {String::New("exit")};
    node::MakeCallback(emitter,"emit",1,exitArgv);

    mainBrowser = NULL;
    mainBrowserHwnd = NULL;
    CloseMainWindow();
    return true;
  }
  return false;
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  REQUIRE_UI_THREAD();

  if(!browser->IsPopup()) {
    GetWindow(browser)->Emit("close");
    DoClose(browser);
#ifdef __WIN__
    delete GetWindow(browser);
#endif
  }
}

void ClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
  REQUIRE_UI_THREAD();
  if (!browser->IsPopup()) {
    GetWindow(browser)->Emit("ready");
  }
}

void ClientHandler::SetMainHwnd(CefWindowHandle& hwnd) {
  AutoLock lock_scope(this);
  mainHandle = hwnd;
}

void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
  REQUIRE_UI_THREAD();
  std::string titleStr(title);
  SetWindowTitle(GetContainer(browser),titleStr.c_str());
  if (!browser->IsPopup()) {
    GetWindow(browser)->Emit("title-change");
  }
}
