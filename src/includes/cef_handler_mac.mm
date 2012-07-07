#import  <Cocoa/Cocoa.h>
#import  <objc/runtime.h>
#include <node.h>
#include "include/cef_browser.h"
#include "includes/cef.h"
#include "includes/cef_handler.h"
#include "mac/mainwindow.h"

using namespace appjs;

struct Wrap;
@interface Wrapper : NSObject {

Wrap* object_;

}
- (id)initWithV8Object:(appjs::MainWindow*)window;
@property (nonatomic,readwrite,assign) appjs::MainWindow* handle;

@end


CefWindowHandle ClientHandler::GetMainHwnd(){
  return m_MainHwnd;
}

v8::Handle<v8::Object> ClientHandler::CreatedBrowser(CefRefPtr<CefBrowser> browser) {
  NSView* view = (NSView*)browser->GetWindowHandle();
  NSWindow* win = [view window];
  Wrapper* wrap = (Wrapper*) objc_getAssociatedObject(win,"mainwindow");
  MainWindow* mainwindow = [wrap handle];
  mainwindow.setBrowser(browser);

  return mainwindow->getV8Handle();
}

v8::Handle<v8::Object> ClientHandler::GetV8WindowHandle(CefRefPtr<CefBrowser> browser) {
  NSView* view = (NSView*)browser->GetWindowHandle();
  NSWindow* win = [view window];
  Wrapper* wrap = (Wrapper*) objc_getAssociatedObject(win,"mainwindow");
  MainWindow* mainwindow = [wrap handle];

  return mainwindow->getV8Handle();
}

MainWindow* ClientHandler::WindowFromHandle(CefWindowHandle handle){
  NSWindow* win = (NSWindow*)handle;
  Wrapper* wrap = (Wrapper*) objc_getAssociatedObject(win, "mainwindow");
  MainWindow* mainwindow = [wrap handle];
  return mainwindow;
}


void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title)
{
  REQUIRE_UI_THREAD();

  // Set the frame window title bar
  NSView* view = (NSView*)browser->GetWindowHandle();
  NSWindow* window = [view window];
  std::string titleStr(title);
  NSString* str = [NSString stringWithUTF8String:titleStr.c_str()];
  [window setTitle:str];
}

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

CefRefPtr<ClientHandler> g_handler;
