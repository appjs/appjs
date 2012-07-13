#include "includes/cef.h"
#include "includes/cef_handler.h"
#include "base/native_window.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

NativeWindow::NativeWindow(char* url, Settings* settings){
  width_ = settings->getNumber("width",800);
  height_ = settings->getNumber("height",600);
  left_ = settings->getNumber("left",-1);
  top_ = settings->getNumber("top",-1);
  opacity = settings->getNumber("opacity",1);
  alpha = settings->getBoolean("alpha",false);
  show_chrome = settings->getBoolean("showChrome",true);
  resizable = settings->getBoolean("resizable",true);
  show_resize_grip = settings->getBoolean("showResizeGrip",false);
  auto_resize = settings->getBoolean("autoResize",false);
  fullscreen = settings->getBoolean("fullscreen",false);
  icons = new Settings(settings->getObject("icons", Object::New()));
  g_handler->SetAutoResize(auto_resize);
  this->Init(url, settings);

  Cef::Run();
}

NativeWindow::~NativeWindow(){
  browser_ = NULL;
}

void NativeWindow::OpenDevTools(){
  if (browser_) {
    browser_->ShowDevTools();
  }
}

void NativeWindow::CloseDevTools(){
  if (browser_) {
    browser_->CloseDevTools();
  }
}

void NativeWindow::RunInBrowser(char* script){
  if (browser_) {
    browser_->GetMainFrame()->ExecuteJavaScript(script, "", 0);
  }
}

void NativeWindow::SetBrowser(CefRefPtr<CefBrowser> browser) {
  browser_ = browser;
}

void NativeWindow::SetV8Handle(Handle<Object> v8handle) {
  v8handle_ = v8handle;
}

CefRefPtr<CefBrowser> NativeWindow::GetBrowser() {
  return browser_;
}

Handle<Object> NativeWindow::GetV8Handle() {
  return v8handle_;
}

void NativeWindow::UpdatePosition(int top, int left, int width, int height){
  width_ = width;
  height_ = height;
  left_ = left;
  top_ = top;
}

void NativeWindow::SetWidth(int width){
  if (width_ != width) {
    width_ = width;
    SetSize(width_, height_);
  }
}
void NativeWindow::SetHeight(int height){
  if (height_ != height) {
    height_ = height;
    SetSize(width_, height_);
  }
}
void NativeWindow::SetLeft(int left){
  if (left_ != left) {
    left_ = left;
    SetPosition(top_, left_);
  }
}
void NativeWindow::SetTop(int top){
  if (top_ != top) {
    top_ = top;
    SetPosition(top_, left_);
  }
}

int NativeWindow::GetWidth(){
  return width_;
}
int NativeWindow::GetHeight(){
  return height_;
}
int NativeWindow::GetLeft(){
  return left_;
}
int NativeWindow::GetTop(){
  return top_;
}


} /* appjs */
