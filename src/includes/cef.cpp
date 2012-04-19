#include "include/cef_app.h"
#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "include/cef_scheme.h"
#include "cef_handler.h"
#include "cef.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

// Implementation of the schema handler for appjs:// requests.
class ClientSchemeHandler : public CefSchemeHandler {
 public:
  ClientSchemeHandler() : offset_(0) {}

  virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
                              CefRefPtr<CefSchemeHandlerCallback> callback)
                              OVERRIDE {
    REQUIRE_IO_THREAD();

    bool handled = false;

    AutoLock lock_scope(this);

    std::string url = request->GetURL();
    if (strstr(url.c_str(), "public/index.html") != NULL) {
      // Build the response html
      data_ = "<html><head><title>Client Scheme Handler</title></head><body>"
              "This contents of this page page are served by the "
              "ClientSchemeHandler class handling the client:// protocol."
              "<br/>You should see an image:"
              "<br/><img src=\"client://tests/client.png\"><pre>";

      data_.append("</pre><br/>Try the test form:"
                   "<form method=\"POST\" action=\"handler.html\">"
                   "<input type=\"text\" name=\"field1\">"
                   "<input type=\"text\" name=\"field2\">"
                   "<input type=\"submit\">"
                   "</form></body></html>");

      handled = true;

      // Set the resulting mime type
      mime_type_ = "text/html";
    }

    if (handled) {
      // Indicate the headers are available.
      callback->HeadersAvailable();
      return true;
    }

    return false;
  }

  virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
                                  int64& response_length,
                                  CefString& redirectUrl) OVERRIDE {
    REQUIRE_IO_THREAD();

    ASSERT(!data_.empty());

    response->SetMimeType(mime_type_);
    response->SetStatus(200);

    // Set the resulting response length
    response_length = data_.length();
  }

  virtual void Cancel() OVERRIDE {
    REQUIRE_IO_THREAD();
  }

  virtual bool ReadResponse(void* data_out,
                            int bytes_to_read,
                            int& bytes_read,
                            CefRefPtr<CefSchemeHandlerCallback> callback)
                            OVERRIDE {
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

 private:
  std::string data_;
  std::string mime_type_;
  size_t offset_;

  IMPLEMENT_REFCOUNTING(ClientSchemeHandler);
  IMPLEMENT_LOCKING(ClientSchemeHandler);
};

// Implementation of the factory for for creating schema handlers.
class ClientSchemeHandlerFactory : public CefSchemeHandlerFactory {
 public:
  // Return a new scheme handler instance to handle the request.
  virtual CefRefPtr<CefSchemeHandler> Create(CefRefPtr<CefBrowser> browser,
                                             const CefString& scheme_name,
                                             CefRefPtr<CefRequest> request)
                                             OVERRIDE {
    REQUIRE_IO_THREAD();
    return new ClientSchemeHandler();
  }

  IMPLEMENT_REFCOUNTING(ClientSchemeHandlerFactory);
};

bool Cef::initialized_;

void Cef::Init() {

  if( !Cef::initialized_ ) {
    CefLoop::Init();

    CefSettings settings;
    CefRefPtr<CefApp> app;

    settings.pack_loading_disabled = true;
    settings.multi_threaded_message_loop = false;

    g_handler = new ClientHandler();
    CefInitialize(settings, app);

    CefRegisterCustomScheme("appjs", true, false, false);
    CefRegisterSchemeHandlerFactory("appjs", "",
        new ClientSchemeHandlerFactory());

    CefBase::Init();

    Cef::initialized_ = true;
  }

};

} /* appjs */

