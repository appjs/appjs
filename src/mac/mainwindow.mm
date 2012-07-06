#import  <Cocoa/Cocoa.h>
#include "appjs.h"
#import  "include/cef_application_mac.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/cef_handler.h"
#include "mac/mainwindow.h"

// The global ClientHandler reference.
extern CefRefPtr<ClientHandler> g_handler;

appjs::Settings* mainWndSettings;
char* mainWndUrl;
// Memory AutoRelease pool.
static NSAutoreleasePool* g_autopool = nil;

// Provide the CefAppProtocol implementation required by CEF.
@interface AppjsApplication : NSApplication<CefAppProtocol> {
@private
  BOOL handlingSendEvent_;
}
@end

@implementation AppjsApplication
- (BOOL)isHandlingSendEvent {
  return handlingSendEvent_;
}

- (void)setHandlingSendEvent:(BOOL)handlingSendEvent {
  handlingSendEvent_ = handlingSendEvent;
}

- (void)sendEvent:(NSEvent*)event {
  CefScopedSendingEvent sendingEventScoper;
  [super sendEvent:event];
}

@end


// Receives notifications from the browser window. Will delete
// itself when done.
@interface AppjsWindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation AppjsWindowDelegate

- (void)windowDidBecomeKey:(NSNotification*)notification {
  if (g_handler.get() && g_handler->GetBrowserHwnd()) {
    // Give focus to the browser window.
    g_handler->GetBrowser()->SetFocus(true);
  }
}

// Called when the window is about to close. Perform the self-destruction
// sequence by getting rid of the window. By returning YES, we allow the window
// to be removed from the screen.
- (BOOL)windowShouldClose:(id)window {

  // If this is the main window, shutdown cef.
  NSView* mainWnd = g_handler->GetBrowserHwnd();

  if( window == [mainWnd window]){
    appjs::Cef::Shutdown();
  };

  // Clean ourselves up after clearing the stack of anything that might have the
  // window on it.
  [self performSelectorOnMainThread:@selector(cleanup:)
                         withObject:window
                      waitUntilDone:NO];

  return YES;
}

// Deletes itself.
- (void)cleanup:(id)window {

  [self release];
}

@end

// Receives notifications from the application. Will delete itself when done.
@interface AppjsAppDelegate : NSObject

@end

@implementation AppjsAppDelegate

// Sent by the default notification center immediately before the application
// terminates.
- (void)applicationWillTerminate:(NSNotification *)aNotification {
  // Shut down CEF.
  appjs::Cef::Shutdown();

  [self release];

  // Release the AutoRelease pool.
  [g_autopool release];
}

@end


namespace appjs {

using namespace v8;

MainWindow::MainWindow (char* url, Settings* settings) {
  mainWndSettings = settings;
  mainWndUrl = url;
  // if it is the first time MainWindow is called, create the Application.
  if(!g_handler->GetBrowser().get()){
    // Initialize the AutoRelease pool.
    g_autopool = [[NSAutoreleasePool alloc] init];
    // Initialize the Application instance.
    [AppjsApplication sharedApplication];

    // Create the application delegate and window.
    AppjsAppDelegate* appDelegate = [[AppjsAppDelegate alloc] init];
    [NSApp setDelegate:appDelegate];

  }

  // Create the delegate for browser window events.
  AppjsWindowDelegate* windowDelegate = [[AppjsWindowDelegate alloc] init];

  int width = settings->getNumber("width",800);
  int height = settings->getNumber("height",600);
  int x = settings->getNumber("x",-1);
  int y = settings->getNumber("y",-1);
  double opacity = settings->getNumber("opacity",1);
  bool show_chrome = settings->getBoolean("showChrome",true);
  bool resizable = settings->getBoolean("resizable",true);
  bool auto_resize = settings->getBoolean("autoResize",false);
  bool fullscreen = settings->getBoolean("fullscreen",false);

  // Set auto_resize on cef handler
  g_handler->SetAutoResize(auto_resize);

  // Center the window if user didn't specified x or y
  if( x < 0 || y < 0 ) {
    x = (appjs::MainWindow::ScreenWidth() - width ) / 2;
    y = (appjs::MainWindow::ScreenHeight() - height ) / 2;
  }

  NSUInteger styles;

  if( ! show_chrome ) {
    styles = NSBorderlessWindowMask;
  } else {
    styles = NSTitledWindowMask |
             NSClosableWindowMask |
             NSMiniaturizableWindowMask;
  }

  if( resizable ) {
    styles |= NSResizableWindowMask;
  }

  // Create the main application window.
  NSRect window_rect = { {x, y} , {width, height} };

  // If we need fullscreen window, remove border and
  // set window rectangle to fit screen.
  // TODO: there should be a better method to fullscreen
  // window. on 10.7 we could use NSFullScreenWindowMask
  if( fullscreen ) {
    styles = NSBorderlessWindowMask;
    window_rect = [[NSScreen mainScreen] frame];
  }

  // Create the window
  NSWindow* mainWnd = [[NSWindow alloc]
                       initWithContentRect:window_rect
                       styleMask:(styles)
                       backing:NSBackingStoreBuffered
                       defer:NO];
  [mainWnd setAlphaValue:opacity];
  [mainWnd setTitle:@"cefclient"];
  [mainWnd setDelegate:windowDelegate];

  // Rely on the window delegate to clean us up rather than immediately
  // releasing when the window gets closed. We use the delegate to do
  // everything from the autorelease pool so the window isn't on the stack
  // during cleanup (ie, a window close from javascript).
  [mainWnd setReleasedWhenClosed:YES];

  // Add browser view to newly created window.
  NSView* contentView = [mainWnd contentView];
  appjs::Cef::AddWebView(contentView,url,settings);

  // Keep an instance of frame, we need it for show/hide methods.
  this->window = contentView;

  // Size the window.
  [mainWnd setFrame:[mainWnd frameRectForContentRect:[mainWnd frame]] display:YES];

  // Run CEF message loop
  appjs::Cef::Run();

}

void MainWindow::OpenDevTools(){
  if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");

  g_handler->GetBrowser()->ShowDevTools();
}

void MainWindow::CloseDevTools(){
  if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");

  g_handler->GetBrowser()->CloseDevTools();
}


void MainWindow::show() {
  if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");

  [[window window] makeKeyAndOrderFront: nil];
};

void MainWindow::hide() {
  if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");

  [[window window] orderOut: nil];
};

int MainWindow::ScreenWidth() {
  NSRect screen_rect = [[NSScreen mainScreen] visibleFrame];
  return screen_rect.size.width;
}

int MainWindow::ScreenHeight() {
  NSRect screen_rect = [[NSScreen mainScreen] visibleFrame];
  return screen_rect.size.height;
}

void MainWindow::destroy() {
  if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");

  [[window window] performSelectorOnMainThread:@selector(performClose:)
                         withObject:nil
                      waitUntilDone:NO];

};

} /* appjs */

