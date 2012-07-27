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


NativeWindow* ClientHandler::GetWindow(CefWindowHandle handle){
  NSWindow* win = [handle window];
  Wrapper* wrap = (Wrapper*) objc_getAssociatedObject(win, (char *)"nativewindow");
  NativeWindow* window = [wrap handle];
  return window;
}

CefWindowHandle ClientHandler::GetContainer(CefRefPtr<CefBrowser> browser){
  return browser->GetWindowHandle();
}

CefRefPtr<ClientHandler> g_handler;
