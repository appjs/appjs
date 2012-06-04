#ifndef APPJS_CEF_SCHEME_HANDLER_H
#define APPJS_CEF_SCHEME_HANDLER_H
#pragma once

#include <node.h>
#include "include/cef_scheme.h"
#include "include/cef_task.h"

namespace appjs {

class AppjsSchemeHandler
  : public CefSchemeHandler,
    public CefTask 
{
public:
  AppjsSchemeHandler() {}
  ~AppjsSchemeHandler() {};

  virtual void Execute(CefThreadId) OVERRIDE;
  virtual bool ProcessRequest(CefRefPtr<CefRequest>,
                              CefRefPtr<CefSchemeHandlerCallback>) OVERRIDE;
  virtual void GetResponseHeaders(CefRefPtr<CefResponse>,
                                  int64&,
                                  CefString&) OVERRIDE;
  virtual bool ReadResponse(void*,
                            int,
                            int&,
                            CefRefPtr<CefSchemeHandlerCallback>) OVERRIDE;
  virtual void Cancel() OVERRIDE;


  static v8::Handle<v8::Value> NodeCallback(const v8::Arguments&);

protected:
  CefRefPtr<CefRequest> request_;
  CefRefPtr<CefSchemeHandlerCallback> callback_;
  std::string status_text_;
  std::string mime_type_;
  char*       data_;
  size_t      data_length_;
  size_t      offset_;
  int         status_;

  IMPLEMENT_REFCOUNTING(AppjsSchemeHandler);
  IMPLEMENT_LOCKING(AppjsSchemeHandler);

};

class AppjsSchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
  // Return a new scheme handler instance to handle the request.
  virtual CefRefPtr<CefSchemeHandler> Create(CefRefPtr<CefBrowser>,
                                             const CefString&,
                                             CefRefPtr<CefRequest>) OVERRIDE;

  IMPLEMENT_REFCOUNTING(AppjsSchemeHandlerFactory);
};

} /* appjs */

#endif /* end of APPJS_CEF_SCHEME_HANDLER_H */