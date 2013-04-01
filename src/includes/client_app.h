// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFCLIENT_CLIENT_APP_H_
#define CEF_TESTS_CEFCLIENT_CLIENT_APP_H_
#pragma once

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "include/cef_app.h"
#include "includes/cef_sync_handler.h"
#include "native_window/native_window.h"
#include "node.h"
#include "node_internals.h"

namespace appjs {

class ClientApp : public CefApp,
                  public CefBrowserProcessHandler,
                  public CefRenderProcessHandler {
 public:
  // Interface for renderer delegates. All RenderDelegates must be returned via
  // CreateRenderDelegates. Do not perform work in the RenderDelegate
  // constructor.
  class RenderDelegate : public virtual CefBase {
   public:
    // Called after the render process main thread has been created.
    virtual void OnRenderThreadCreated(CefRefPtr<ClientApp> app) {
      fprintf(stderr, "%s\n", "renderer created.");
    }

    // Called when WebKit is initialized. Used to register V8 extensions.
    virtual void OnWebKitInitialized(CefRefPtr<ClientApp> app) {
      fprintf(stderr, "%s\n", "webkit init");
    }

    // Called when a V8 context is created. Used to create V8 window bindings
    // and set message callbacks. RenderDelegates should check for unique URLs
    // to avoid interfering with each other.
    virtual void OnContextCreated(CefRefPtr<ClientApp> app,
                                  CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefV8Context> context) {
      fprintf(stderr, "%s\n", "context created");
      if (!browser->IsPopup() && frame->IsMain()) {
        context->Enter();
        CefRefPtr<CefV8Value> appjsObj = CefV8Value::CreateObject(NULL);
        CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("send", new AppjsSyncHandler(browser->GetHost()));
        context->GetGlobal()->SetValue("appjs", appjsObj, V8_PROPERTY_ATTRIBUTE_NONE);
        appjsObj->SetValue("send", func, V8_PROPERTY_ATTRIBUTE_NONE);
        context->Exit();
        //NativeWindow::GetWindow(browser)->Emit("context-created");
      }

    }

    // Called when a V8 context is released. Used to clean up V8 window
    // bindings. RenderDelegates should check for unique URLs to avoid
    // interfering with each other.
    virtual void OnContextReleased(CefRefPtr<ClientApp> app,
                                   CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefV8Context> context) {
      fprintf(stderr, "%s\n", "context released");
    }

    // Called when the focused node in a frame has changed.
    virtual void OnFocusedNodeChanged(CefRefPtr<ClientApp> app,
                                      CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefDOMNode> node) {
    }

    // Called when a process message is received. Return true if the message was
    // handled and should not be passed on to other handlers. RenderDelegates
    // should check for unique message names to avoid interfering with each
    // other.
    virtual bool OnProcessMessageReceived(
        CefRefPtr<ClientApp> app,
        CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) {
      return false;
    }
  };

  typedef std::set<CefRefPtr<RenderDelegate> > RenderDelegateSet;

  ClientApp();

 private:
  // Creates all of the RenderDelegate objects. Implemented in
  // client_app_delegates.
  static void CreateRenderDelegates(RenderDelegateSet& delegates);

  // Registers custom schemes. Implemented in client_app_delegates.
  static void RegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar,
                                    std::vector<CefString>& cookiable_schemes);

  // CefApp methods.
  virtual void OnRegisterCustomSchemes(
      CefRefPtr<CefSchemeRegistrar> registrar) OVERRIDE {
    RegisterCustomSchemes(registrar, cookieable_schemes_);
  }
  virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
      OVERRIDE { return this; }
  virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()
      OVERRIDE { return this; }

  // CefBrowserProcessHandler methods.
  virtual void OnContextInitialized();

  // CefRenderProcessHandler methods.
  virtual void OnRenderThreadCreated() OVERRIDE;
  virtual void OnWebKitInitialized() OVERRIDE;
  virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) OVERRIDE;
  virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context) OVERRIDE;
  virtual void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefDOMNode> node) OVERRIDE;
  virtual bool OnProcessMessageReceived(
      CefRefPtr<CefBrowser> browser,
      CefProcessId source_process,
      CefRefPtr<CefProcessMessage> message) OVERRIDE;

  // Map of message callbacks.
  typedef std::map<std::pair<std::string, int>,
                   std::pair<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value> > >
                   CallbackMap;
  CallbackMap callback_map_;

  // Set of supported RenderDelegates.
  RenderDelegateSet render_delegates_;

  // Schemes that will be registered with the global cookie manager.
  std::vector<CefString> cookieable_schemes_;

  IMPLEMENT_REFCOUNTING(ClientApp);
};

};
#endif  // CEF_TESTS_CEFCLIENT_CLIENT_APP_H_
