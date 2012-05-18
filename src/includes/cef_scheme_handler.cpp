#include <node.h>
#include "includes/cef_scheme_handler.h"
#include "includes/cef_handler.h"
#include "includes/util.h"

namespace appjs {

using namespace v8;

Handle<Value> WrapObject(void* obj) {

  HandleScope scope;

  Persistent<ObjectTemplate> obj_template = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
  obj_template->SetInternalFieldCount(1);

  Handle<Object> self = obj_template->NewInstance();

  self->SetInternalField(0, External::New(obj));

  return scope.Close(self);
}

void *UnwrapObject(Handle<Value> data) {
  v8::Handle<v8::Object> obj = data->ToObject();
  v8::Handle<v8::External> obj2 = v8::Handle<v8::External>::Cast(obj->GetInternalField(0));

  return obj2->Value();
}

// Implementation of the schema handler for appjs:// requests.
void AppjsSchemeHandler::Execute(CefThreadId threadId) {

  REQUIRE_UI_THREAD();

  HandleScope scope;

  Local<Object>  global = Context::GetCurrent()->Global();
  Local<Object> process = global->Get(String::NewSymbol("process"))->ToObject();
  Local<Object> emitter = Local<Object>::Cast(process->Get(String::NewSymbol("AppjsEmitter")));

  const int argc = 3;

  Handle<Value>    self = WrapObject(this);
  Local<Function>    cb = FunctionTemplate::New(NodeCallback,self)->GetFunction();
  Local<Object>     req = Object::New();
  Local<String>    post = String::New("");
  Local<Object> headers = Object::New();
  Local<String>   files = String::New("");

  CefRequest::HeaderMap headerMap;
  request_->GetHeaderMap(headerMap);

  if (headerMap.size() > 0) {
    CefRequest::HeaderMap::const_iterator it = headerMap.begin();
    for ( ; it != headerMap.end(); ++it) {
      headers->Set(String::New((uint16_t*)(*it).first.c_str()),String::New((uint16_t*)(*it).second.c_str()));
    }
  }

  CefRefPtr<CefPostData> postData = request_->GetPostData();

  if(postData.get()){

    CefPostData::ElementVector elements;

    postData->GetElements(elements);

    if (elements.size() > 0) {
      CefRefPtr<CefPostDataElement> element;
      CefPostData::ElementVector::const_iterator it = elements.begin();

      for ( ; it != elements.end(); ++it) {
        element = (*it);
        if (element->GetType() == PDE_TYPE_BYTES && element->GetBytesCount()) {

          // retrieve the data.
          size_t size = element->GetBytesCount();
          char* bytes = new char[size];
          element->GetBytes(size, bytes);
          post = String::New(bytes);
          delete [] bytes;

        } else if (element->GetType() == PDE_TYPE_FILE) {
          //TODO Needs testing
          files = String::New((uint16_t*)element->GetFile().c_str());
        }
      }

    }
  }

  Handle<Value> method = String::New((uint16_t*)request_->GetMethod().c_str());
  Handle<Value> url = String::New((uint16_t*)request_->GetURL().c_str());

  req->Set(String::NewSymbol("method"),method);
  req->Set(String::NewSymbol("url"),url);
  req->Set(String::NewSymbol("post"),post);
  req->Set(String::NewSymbol("headers"),headers);
  req->Set(String::NewSymbol("files"),files);

  Handle<Value> argv[argc] = {String::New("request"),req,cb};
  node::MakeCallback(emitter,"emit",argc,argv);

}

Handle<Value> AppjsSchemeHandler::NodeCallback(const Arguments& args) {

  AppjsSchemeHandler* me = static_cast<AppjsSchemeHandler *>(UnwrapObject(args.Data()));

  AutoLock lock_scope(me);

  me->status_      = args[0]->NumberValue();
  me->status_text_ = appjs::V8StringToChar(args[1]->ToString());
  me->mime_type_   = appjs::V8StringToChar(args[2]->ToString());
  me->data_        = appjs::V8StringToChar(args[3]->ToString());

  me->callback_->HeadersAvailable();

  return args.This();
}

bool AppjsSchemeHandler::ProcessRequest(CefRefPtr<CefRequest> request,
                              CefRefPtr<CefSchemeHandlerCallback> callback)
{

  REQUIRE_IO_THREAD();

  AutoLock lock_scope(this);

  status_      = 404;
  status_text_ = "Not Found";
  mime_type_   = "";
  data_        = "";
  offset_      = 0;
  request_     = request;
  callback_    = callback;

  CefPostTask(TID_UI, this);

  return true;
}

void AppjsSchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
                                  int64& response_length,
                                  CefString& redirectUrl)
{
  REQUIRE_IO_THREAD();

  ASSERT(!data_.empty());

  response->SetStatus(status_);
  response->SetStatusText(status_text_);
  response->SetMimeType(mime_type_);

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
  return new AppjsSchemeHandler();
}

} /* appjs */
