#ifndef APPJS_SYNC_HANDLER_H
#define APPJS_SYNC_HANDLER_H
#pragma once

#include <node.h>
#include "include/cef_browser.h"
#include "include/cef_v8.h"

namespace appjs {

class AppjsSyncHandler : public CefV8Handler {
  public:
    AppjsSyncHandler(CefRefPtr<CefBrowser> browser) : browser_(browser) {}
    virtual ~AppjsSyncHandler(){}

    virtual bool Execute(const CefString& name,
                         CefRefPtr<CefV8Value> object,
                         const CefV8ValueList& arguments,
                         CefRefPtr<CefV8Value>& retval,
                         CefString& exception) OVERRIDE;


  protected:
    CefRefPtr<CefBrowser> browser_;
    IMPLEMENT_REFCOUNTING(AppjsSyncHandler);
};


} /* appjs */

#endif /* end of APPJS_SYNC_HANDLER_H */
