#import  <Cocoa/Cocoa.h>
#import  <objc/runtime.h>
#include "include/cef_browser.h"
#include "includes/cef.h"
#include "includes/cef_handler.h"

using namespace appjs;

struct Wrap;
@interface Wrapper : NSObject {

Wrap* object_;

}
- (id)initWithV8Object:(appjs::NativeWindow*)window;
@property (nonatomic,readwrite,assign) appjs::NativeWindow* handle;

@end


CefWindowHandle ClientHandler::GetMainHwnd(){
  return m_MainHwnd;
}

NativeWindow* ClientHandler::GetWindow(CefWindowHandle handle){
  NSWindow* win = [handle window];
  Wrapper* wrap = (Wrapper*) objc_getAssociatedObject(win, "nativewindow");
  NativeWindow* window = [wrap handle];
  return window;
}

CefWindowHandle ClientHandler::GetContainer(CefRefPtr<CefBrowser> browser){
  return browser->GetWindowHandle();
}


bool ClientHandler::OnKeyEvent(CefRefPtr<CefBrowser> browser, KeyEventType type, int code,
                               int modifiers, bool isSystemKey, bool isAfterJavaScript) {

  //TODO detect key squence to show dev tools.
  if (!browser->IsPopup() && type == KEYEVENT_RAWKEYDOWN) {
  //  browser->ShowDevTools();
    //return true;
  }
  return false;
};

void ClientHandler::OnContentsSizeChange(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    int width,
                                    int height)
{
  REQUIRE_UI_THREAD();

  if(this->m_AutoResize) {
    // Size the window.
    NSView* view = (NSView*)browser->GetWindowHandle();
    NSWindow* window = [view window];
    NSRect r = [window contentRectForFrameRect:[window frame]];
    r.size.width = width;
    r.size.height = height;
    [window setFrame:[window frameRectForContentRect:r] display:YES];
  }
}
void ClientHandler::CloseMainWindow() {

  REQUIRE_UI_THREAD();
  appjs::Cef::Shutdown();
}


void ClientHandler::SetWindowTitle(CefWindowHandle handle, const char* title) {
  NSWindow* win = [handle window];
  [win setTitle: [NSString stringWithUTF8String:title]];
}


CefRefPtr<ClientHandler> g_handler;
