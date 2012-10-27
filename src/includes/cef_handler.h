#ifndef APPJS_CEF_CLIENT_HANDLER_H
#define APPJS_CEF_CLIENT_HANDLER_H
#pragma once

#include <assert.h>
#include <node.h>
#include <map>
#include <set>
#include "include/cef_task.h"
#include "include/cef_client.h"
#include "includes/cef.h"
#include "native_window/native_window.h"

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
                      public CefKeyboardHandler {

public:
  // Interface for process message delegates. Do not perform work in the
  // RenderDelegate constructor.
  class ProcessMessageDelegate : public virtual CefBase {
   public:
    // Called when a process message is received. Return true if the message was
    // handled and should not be passed on to other handlers.
    // ProcessMessageDelegates should check for unique message names to avoid
    // interfering with each other.
    virtual bool OnProcessMessageReceived(
        CefRefPtr<ClientHandler> handler,
        CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) {
      return false;
    }
  };

  typedef std::set<CefRefPtr<ProcessMessageDelegate> >
      ProcessMessageDelegateSet;

  // Interface for request handler delegates. Do not perform work in the
  // RequestDelegate constructor.
  class RequestDelegate : public virtual CefBase {
   public:
    // Called to retrieve a resource handler.
    virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
        CefRefPtr<ClientHandler> handler,
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request) {
      return NULL;
    }
  };

  typedef std::set<CefRefPtr<RequestDelegate> > RequestDelegateSet;

  ClientHandler(CefBrowserSettings browserSettings);
  virtual ~ClientHandler();

  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefLifeSpanHandler> GetRenderProcessHandler() OVERRIDE {
    fprintf(stderr, "%s\n", "me");
    return this;
  }
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
    return this;
  }
  
  virtual bool HasMainWindow();
  virtual void Shutdown();
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual void OnTitleChange(CefRefPtr<CefBrowserHost> browser,
                             const CefString& title) OVERRIDE;

  virtual void OnContextCreated(CefRefPtr<CefBrowserHost> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) OVERRIDE;

  virtual void OnContextReleased(CefRefPtr<CefBrowserHost> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context) OVERRIDE;

  virtual void OnContentsSizeChange(CefRefPtr<CefBrowserHost> browser,
                                    CefRefPtr<CefFrame> frame,
                                    int width,
                                    int height) OVERRIDE;

  virtual void OnLoadEnd(CefRefPtr<CefBrowserHost> browser,
                         CefRefPtr<CefFrame> frame,
                         int httpStatusCode) OVERRIDE;

  CefRefPtr<CefBrowserHost> mainBrowserHandle;
  CefBrowserSettings browserSettings_;

  IMPLEMENT_REFCOUNTING(ClientHandler);
  IMPLEMENT_LOCKING(ClientHandler);
};

#endif  /* end APPJS_CEF_CLIENT_HANDLER_H_ */
