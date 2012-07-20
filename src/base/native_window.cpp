#include "includes/cef.h"
#include "includes/cef_handler.h"
#include "base/native_window.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

NativeWindow::NativeWindow(char* url, Settings* settings){
  rect_.width = settings->getNumber("width",800);
  rect_.height = settings->getNumber("height",600);
  rect_.left = settings->getNumber("left",-1);
  rect_.top = settings->getNumber("top",-1);
  opacity_ = settings->getNumber("opacity",1);
  alpha_ = settings->getBoolean("alpha",false);
  show_chrome_ = settings->getBoolean("showChrome",true);
  resizable_ = settings->getBoolean("resizable",true);
  show_resize_grip = settings->getBoolean("showResizeGrip",false);
  auto_resize = settings->getBoolean("autoResize",false);
  fullscreen_ = settings->getBoolean("fullscreen",false);
  icons = new Settings(settings->getObject("icons", Object::New()));
  g_handler->SetAutoResize(auto_resize);

  this->Init(url, settings);

  if (settings->getBoolean("topmost",false)) {
    SetTopmost(true);
  }

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
  rect_.width = width;
  rect_.height = height;
  rect_.left = left;
  rect_.top = top;
}

void NativeWindow::UpdatePosition(appjs_rect rect){
  rect_.width = rect.width;
  rect_.height = rect.height;
  rect_.left = rect.left;
  rect_.top = rect.top;
}

void NativeWindow::SetState(NW_STATE state){
  switch (state) {
    case NW_STATE_NORMAL:
      Restore();
      break;
    case NW_STATE_MINIMIZED:
      Minimize();
      break;
    case NW_STATE_MAXIMIZED:
      Maximize();
      break;
    case NW_STATE_FULLSCREEN:
      Fullscreen();
      break;
  }
}

void NativeWindow::Move(appjs_rect rect){
  Move(rect.top, rect.left, rect.width, rect.height);
}

void NativeWindow::SetWidth(int width){
  if (rect_.width != width) {
    rect_.width = width;
    Resize(rect_.width, rect_.height);
  }
}
void NativeWindow::SetHeight(int height){
  if (rect_.height != height) {
    rect_.height = height;
    Resize(rect_.width, rect_.height);
  }
}
void NativeWindow::SetLeft(int left){
  if (rect_.left != left) {
    rect_.left = left;
    Move(rect_.top, rect_.left);
  }
}
void NativeWindow::SetTop(int top){
  if (rect_.top != top) {
    rect_.top = top;
    Move(rect_.top, rect_.left);
  }
}

int NativeWindow::GetWidth(){
  return rect_.width;
}
int NativeWindow::GetHeight(){
  return rect_.height;
}
int NativeWindow::GetLeft(){
  return rect_.left;
}
int NativeWindow::GetTop(){
  return rect_.top;
}

void NativeWindow::SetTitle(const char* title) {
  ClientHandler::SetWindowTitle(handle_, title);
}

bool NativeWindow::GetTopmost(){
  return topmost_;
}


} /* appjs */
