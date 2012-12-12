#include "includes/cef.h"
#include "includes/cef_handler.h"
#include "native_window/native_window.h"

extern CefRefPtr<ClientHandler> g_handler;

bool initialized = false;

namespace appjs {

using namespace v8;

static uv_work_t dialog_work;
static AppjsDialogSettings dialog_settings;
static bool dialog_in_progress = false;

NativeWindow::NativeWindow(Settings* settings){
  is_main_window_ = !initialized;
  initialized = true;
  closed_ = false;

  rect_.width      = settings->getInteger("width", 800);
  rect_.height     = settings->getInteger("height", 600);
  rect_.left       = settings->getInteger("left", -1);
  rect_.top        = settings->getInteger("top", -1);
  opacity_         = settings->getNumber("opacity", 1);
  alpha_           = settings->getBoolean("alpha", false);
  show_chrome_     = settings->getBoolean("showChrome", true);
  auto_resize_     = settings->getBoolean("autoResize", false);
  resizable_       = settings->getBoolean("resizable", true);
  show_resize_grip = settings->getBoolean("showResizeGrip", false);
  fullscreen_      = settings->getBoolean("fullscreen", false);
  show_on_taskbar_ = settings->getBoolean("showOnTaskbar", true);
  show_on_pager_   = settings->getBoolean("showOnPager", true);
  icons            = new Settings(settings->getObject("icons"));

  Init(settings->getString("url", (char *)""), settings);

  if (settings->getBoolean("topmost", false)) {
    SetTopmost(true);
  }

  Cef::Run();
}

NativeWindow::~NativeWindow(){
  browser_ = NULL;
}

void NativeWindow::OpenDialog(Settings* settings,Persistent<Function> cb) {

  if( dialog_in_progress ) {
    return;
  }

  dialog_settings.me           = this;
  dialog_settings.cb           = cb;
  dialog_settings.type         = (NW_DIALOGTYPE) settings->getInteger("type",NW_DIALOGTYPE_FILE_OPEN);
  dialog_settings.title        = tstring(settings->getString("title",(TCHAR*)this->GetTitle()));
  // initialValue is the filename which the dialog should select by default.
  // it can be empty.
  dialog_settings.initialValue = tstring(settings->getString("initialValue",TEXT("")));

  dialog_in_progress = true;
  dialog_work.data = &dialog_settings;

  if( dialog_settings.type == NW_DIALOGTYPE_FILE_SAVE || dialog_settings.type == NW_DIALOGTYPE_FILE_OPEN) {

    // acceptTypes is comma-separated MIME types such as "text/plain,text/html".
    // defaults to everything
    dialog_settings.reserveString1 = tstring(settings->getString("acceptTypes",TEXT("All Files:*.*")));
    // multiSelect allows multiple item selection in dialog box.
    dialog_settings.reserveBool1   = settings->getBoolean("multiSelect",false);
    // dirSelect is a boolean indicating directory selectation state.
    // if set to true, users can select a directory instead of a file.
    dialog_settings.reserveBool2   = settings->getBoolean("dirSelect",false);
#ifndef __WIN__
    OpenFileDialog(&dialog_work);
#else
    uv_queue_work(uv_default_loop(), &dialog_work, OpenFileDialog, ProcessFileDialog);
#endif
  } else if( dialog_settings.type == NW_DIALOGTYPE_FONT ) {

    dialog_settings.reserveString1 = tstring(settings->getString("sampleText",TEXT("Sample")));
    uv_queue_work(uv_default_loop(), &dialog_work, OpenFontDialog, ProcessFontDialog);

  } else if( dialog_settings.type == NW_DIALOGTYPE_COLOR ) {

    dialog_settings.reserveString1 = tstring(settings->getString("previousColor",TEXT("")));
    dialog_settings.reserveBool1   = settings->getBoolean("opacity",false);
    uv_queue_work(uv_default_loop(), &dialog_work, OpenColorDialog, ProcessColorDialog);

  }
}

void NativeWindow::DialogClosed() {
  dialog_in_progress = false;
  dialog_work.data = NULL;
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
  browser_ = NULL;
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

void NativeWindow::SetV8Handle(v8::Handle<Object> v8handle) {
  v8handle_ = v8handle;
}

CefRefPtr<CefBrowser> NativeWindow::GetBrowser() {
  return browser_;
}

v8::Handle<Object> NativeWindow::GetV8Handle() {
  return v8handle_;
}

void NativeWindow::UpdatePosition(int left, int top, int width, int height){
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
  Move(rect.left, rect.top, rect.width, rect.height);
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
    Move(rect_.left, rect_.top);
  }
}
void NativeWindow::SetTop(int top){
  if (rect_.top != top) {
    rect_.top = top;
    Move(rect_.left, rect_.top);
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

void NativeWindow::Emit(v8::Handle<Value>* args,int length = 1){
  if (!closed_) {
    node::MakeCallback(v8handle_, "emit", length, args);
  }
}

void NativeWindow::Emit(const char* event){
  v8::Handle<Value> args[1] = { String::New(event) };
  Emit(args,1);
}

void NativeWindow::Emit(const char* event, v8::Handle<Value> arg){
  v8::Handle<Value> args[2] = {
    String::New(event),
    arg
  };
  Emit(args,2);
}

void NativeWindow::Emit(const char* event, v8::Handle<Value> arg1, v8::Handle<Value> arg2){
  v8::Handle<Value> args[3] = {
    String::New(event),
    arg1,
    arg2
  };
  Emit(args,3);
}

void NativeWindow::Emit(const char* event, int arg1, int arg2){
  v8::Handle<Value> args[3] = {
    String::New(event),
    Integer::New(arg1),
    Integer::New(arg2)
  };
  Emit(args,3);
}

void NativeWindow::Emit(const char* event, const int arg1, const int arg2, const int arg3){
  v8::Handle<Value> args[4] = {
    String::New(event),
    Integer::New(arg1),
    Integer::New(arg2),
    Integer::New(arg3)
  };
  if (!closed_) {
    node::MakeCallback(v8handle_, "emit", 4, args);
  }
}


long NativeWindow::JSResult(){
  Local<Value> result = v8handle_->Get(String::NewSymbol("result"));
  if (result->IsUndefined() || result->IsNull()) {
    return 0;
  } else {
    v8handle_->Set(String::NewSymbol("result"), Undefined());
    return result->Int32Value();
  }
}


} /* appjs */
