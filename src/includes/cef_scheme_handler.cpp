#include <node.h>
#include "includes/cef_scheme_handler.h"
#include "includes/cef_handler.h"
#include "includes/util.h"

namespace appjs {

using namespace v8;

CefRefPtr<CefRequest> AppjsSchemeHandler::request_;
CefRefPtr<CefSchemeHandlerCallback> AppjsSchemeHandler::callback_;
std::string AppjsSchemeHandler::data_;
std::string AppjsSchemeHandler::mime_type_;
size_t AppjsSchemeHandler::offset_;
AppjsSchemeHandler* AppjsSchemeHandler::instance_ = NULL;

// Implementation of the schema handler for appjs:// requests.
void AppjsSchemeHandler::Execute(CefThreadId threadId) {

  if(CefCurrentlyOn(TID_UI)){

    HandleScope scope;

    Local<Object> global = Context::GetCurrent()->Global();
    Local<Object> process = global->Get(String::NewSymbol("process"))->ToObject();
    Local<Object> emitter = Local<Object>::Cast(process->Get(String::NewSymbol("AppjsEmitter")));

    const int argc = 3;

    Local<Function> cb = FunctionTemplate::New(NodeCallback)->GetFunction();
    Local<Object> req = Object::New();
    Handle<Value> method = String::New(request_->GetMethod().c_str());
    Handle<Value> url = String::New(request_->GetURL().c_str());
    req->Set(String::NewSymbol("method"),method);
    req->Set(String::NewSymbol("url"),url);
    Handle<Value> argv[argc] = {String::New("route"),req,cb};
    node::MakeCallback(emitter,"emit",argc,argv);

  }

}

Handle<Value> AppjsSchemeHandler::NodeCallback(const Arguments& args) {
  mime_type_ = appjs::V8StringToChar(args[0]->ToString());
  data_ = appjs::V8StringToChar(args[1]->ToString());
  callback_->HeadersAvailable();
  return args.This();
}

bool AppjsSchemeHandler::ProcessRequest(CefRefPtr<CefRequest> request,
                              CefRefPtr<CefSchemeHandlerCallback> callback)
{

  REQUIRE_IO_THREAD();

  AutoLock lock_scope(this);
  data_ = "";
  mime_type_ = "";
  offset_ = 0;
  request_ = request;
  callback_ = callback;

  CefPostTask(TID_UI, this);

  return true;
}

void AppjsSchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
                                  int64& response_length,
                                  CefString& redirectUrl)
{
  REQUIRE_IO_THREAD();

  ASSERT(!data_.empty());

  response->SetMimeType(mime_type_);
  response->SetStatus(200);
  // Set the resulting response length
  response_length = data_.length();
}

void AppjsSchemeHandler::Cancel() {
  REQUIRE_IO_THREAD();
}

bool AppjsSchemeHandler::ReadResponse(void* data_out,
                            int bytes_to_read,
                            int& bytes_read,
                            CefRefPtr<CefSchemeHandlerCallback> callback)
{

  REQUIRE_IO_THREAD();

  bool has_data = false;
  bytes_read = 0;

  AutoLock lock_scope(this);

  if (offset_ < data_.length()) {
    // Copy the next block of data into the buffer.
    int transfer_size =
        std::min(bytes_to_read, static_cast<int>(data_.length() - offset_));
    memcpy(data_out, data_.c_str() + offset_, transfer_size);
    offset_ += transfer_size;

    bytes_read = transfer_size;
    has_data = true;
  }

  return has_data;
}

// Implementation of the factory for for creating schema handlers.
// Return a new scheme handler instance to handle the request.
CefRefPtr<CefSchemeHandler> AppjsSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
                                             const CefString& scheme_name,
                                             CefRefPtr<CefRequest> request)
{
  REQUIRE_IO_THREAD();
  return AppjsSchemeHandler::GetInstance();
}

} /* appjs */