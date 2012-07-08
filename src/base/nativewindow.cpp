#include "includes/cef.h"
#include "includes/cef_handler.h"
#include "base/nativewindow.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

NativeWindow::NativeWindow(char* url,Settings* settings){
  this->url = url;
  width = settings->getNumber("width",800);
  height = settings->getNumber("height",600);
  x = settings->getNumber("x",-1);
  y = settings->getNumber("y",-1);
  opacity = settings->getNumber("opacity",1);
  show_chrome = settings->getBoolean("showChrome",true);
  resizable = settings->getBoolean("resizable",true);
  show_resize_grip = settings->getBoolean("showResizeGrip",false);
  auto_resize = settings->getBoolean("autoResize",false);
  fullscreen = settings->getBoolean("fullscreen",false);
  Settings icons(settings->getObject("icons",Object::New()));

  this->init(url,settings);

  Cef::Run();
}

void NativeWindow::openDevTools(){
  if (browser_) {
    browser_->ShowDevTools();
  }
}

void NativeWindow::closeDevTools(){
  if (browser_) {
    browser_->CloseDevTools();
  }
}

void NativeWindow::setBrowser(CefRefPtr<CefBrowser> browser) {
  browser_ = browser;
}

void NativeWindow::setV8Handle(Handle<Object> v8handle) {
  v8handle_ = v8handle;
}

CefRefPtr<CefBrowser> NativeWindow::getBrowser() {
  return browser_;
}

Handle<Object> NativeWindow::getV8Handle() {
  return v8handle_;
}

} /* appjs */
