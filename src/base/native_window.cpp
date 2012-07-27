#include "includes/cef.h"
#include "includes/cef_handler.h"
#include "base/native_window.h"

extern CefRefPtr<ClientHandler> g_handler;

bool initialized = false;


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
  auto_resize_ = settings->getBoolean("autoResize",false);
  resizable_ = settings->getBoolean("resizable",true);
  show_resize_grip = settings->getBoolean("showResizeGrip",false);
  fullscreen_ = settings->getBoolean("fullscreen",false);
  icons = new Settings(settings->getObject("icons", Object::New()));

  if (initialized) {
    is_main_window_ = false;
  } else {
    is_main_window_ = true;
    initialized = true;
  }

  closed_ = false;
  Init(url, settings);

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

void NativeWindow::PrepareClose(){
  Emit("close");
  closed_ = true;
}

bool NativeWindow::IsClosed(){
  return closed_;
}

bool NativeWindow::IsMainWindow(){
  return is_main_window_;
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
  SetWindowTitle(handle_, title);
}

bool NativeWindow::GetTopmost(){
  return topmost_;
}

bool NativeWindow::GetAutoResize(){
  return auto_resize_;
}

void NativeWindow::SetAutoResize(bool autoResize) {
  auto_resize_ = autoResize;
}

appjs_rect NativeWindow::GetRect() {
  return rect_;
}

void NativeWindow::Emit(Local<Value>* args){
  if (!closed_) {
    node::MakeCallback(v8handle_, "emit", ARRAY_SIZE(args), args);
  }
}

void NativeWindow::Emit(const char* event){
  Local<Value> args[1] = { String::New(event) };
  Emit(args);
}

void NativeWindow::Emit(const char* event, Local<Value> arg){
  Local<Value> args[2] = { String::New(event), arg };
  Emit(args);
}

void NativeWindow::Emit(const char* event, Local<Value> arg1, Local<Value> arg2){
  Local<Value> args[3] = { String::New(event), arg1, arg2 };
  Emit(args);
}

void NativeWindow::Emit(const char* event, int arg1, int arg2){
  Local<Value> args[3] = { String::New(event), Integer::New(arg1), Integer::New(arg2) };
  Emit(args);
}



} /* appjs */
