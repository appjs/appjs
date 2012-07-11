#ifndef APPJS_CEF_CLIENT_HANDLER_H
#define APPJS_CEF_CLIENT_HANDLER_H
#pragma once

#include <assert.h>
#include <node.h>
#include "include/cef_task.h"
#include "include/cef_client.h"
#include "base/native_window.h"


#ifndef NDEBUG
#define ASSERT(condition) if (!(condition)) { assert(false); }
#else
#define ASSERT(condition) ((void)0)
#endif

#define REQUIRE_UI_THREAD()   ASSERT(CefCurrentlyOn(TID_UI));
#define REQUIRE_IO_THREAD()   ASSERT(CefCurrentlyOn(TID_IO));
#define REQUIRE_FILE_THREAD() ASSERT(CefCurrentlyOn(TID_FILE));

// ClientHandler implementation.
class ClientHandler : public CefClient,
                      public CefLifeSpanHandler,
                      public CefLoadHandler,
                      public CefRequestHandler,
                      public CefDisplayHandler,
                      public CefFocusHandler,
                      public CefKeyboardHandler,
                      public CefV8ContextHandler
{

public:
  ClientHandler();
  virtual ~ClientHandler();

  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefV8ContextHandler> GetV8ContextHandler() OVERRIDE {
    return this;
  }

  static v8::Handle<v8::Object> CreatedBrowser(CefRefPtr<CefBrowser> browser);
  static v8::Handle<v8::Object> GetV8WindowHandle(CefRefPtr<CefBrowser> browser);
  static appjs::NativeWindow* GetWindow(CefWindowHandle handle);
  static appjs::NativeWindow* GetWindow(CefRefPtr<CefBrowser> browser);

  CefRefPtr<CefBrowser> GetBrowser() { return m_Browser; }
  CefWindowHandle GetBrowserHwnd() { return m_BrowserHwnd; }
  CefWindowHandle GetMainHwnd();
  void SetMainHwnd(CefWindowHandle&);
  void CloseMainWindow();
  void SetAutoResize(bool enable){ m_AutoResize = enable; }

  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                             const CefString& title) OVERRIDE;

  virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) OVERRIDE;

  virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context) {}

  virtual void OnContentsSizeChange(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    int width,
                                    int height) OVERRIDE;

  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int httpStatusCode) OVERRIDE;


  virtual bool OnKeyEvent(CefRefPtr<CefBrowser> browser,
                          KeyEventType type,
                          int code,
                          int modifiers,
                          bool isSystemKey,
                          bool isAfterJavaScript) OVERRIDE;

  // The child browser window
  CefRefPtr<CefBrowser> m_Browser;

  // The main frame window handle
  CefWindowHandle m_MainHwnd;

  // The child browser window handle
  CefWindowHandle m_BrowserHwnd;

  // Should we handle content resize?
  bool m_AutoResize;

  IMPLEMENT_REFCOUNTING(ClientHandler);
  IMPLEMENT_LOCKING(ClientHandler);

};

#endif  /* end APPJS_CEF_CLIENT_HANDLER_H_ */
