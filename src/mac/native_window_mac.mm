#import  <Cocoa/Cocoa.h>
#import  <objc/runtime.h>
#import  "include/cef_application_mac.h"
#include "appjs.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/cef_handler.h"
#include "mac/native_window.h"

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

struct Wrap;
@interface Wrapper : NSObject {

Wrap* object_;

}
- (id)initWithV8Object:(appjs::NativeWindow*)window;
@property (nonatomic,readwrite,assign) appjs::NativeWindow* handle;

@end

@interface Wrapper ()
@property (nonatomic,readwrite,assign) Wrap* object;
@end

@implementation Wrapper

@synthesize object = object_;

struct Wrap {
public:
  Wrap(appjs::NativeWindow* obj):handle(obj){};
  appjs::NativeWindow* handle;
};

- (id)initWithV8Object:(appjs::NativeWindow*)window {
  self = [super init];
  if(self != nil){
    self.object = new Wrap(window);
  }
  return self;
}

- (appjs::NativeWindow*)handle {
  return self.object->handle;
}

- (void)setHandle:(appjs::NativeWindow*)window {
  object_->handle = window;
}

@end

namespace appjs {

using namespace v8;

NativeWindow::NativeWindow (char* url, Settings* settings) {
  mainWndSettings = settings;
  mainWndUrl = url;
  // if it is the first time NativeWindow is called, create the Application.
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
    x = (appjs::NativeWindow::ScreenWidth() - width ) / 2;
    y = (appjs::NativeWindow::ScreenHeight() - height ) / 2;
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
  this->window_ = contentView;
  Wrapper* wrap = [[Wrapper alloc] initWithV8Object:this];
  objc_setAssociatedObject(mainWnd,"nativewindow",wrap,OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  appjs::Cef::AddWebView(contentView,url,settings);

  // Keep an instance of frame, we need it for show/hide methods.

  //[wrap setV8Handle:this->getV8Handle()];

  // Size the window.
  [mainWnd setFrame:[mainWnd frameRectForContentRect:[mainWnd frame]] display:YES];

  // Run CEF message loop
  appjs::Cef::Run();

}


void NativeWindow::Show() {
  if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");

  [[window_ window] makeKeyAndOrderFront: nil];
};

void NativeWindow::Hide() {
  if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");

  [[window_ window] orderOut: nil];
};

int NativeWindow::ScreenWidth() {
  NSRect screen_rect = [[NSScreen mainScreen] visibleFrame];
  return screen_rect.size.width;
}

int NativeWindow::ScreenHeight() {
  NSRect screen_rect = [[NSScreen mainScreen] visibleFrame];
  return screen_rect.size.height;
}

void NativeWindow::Destroy() {
  if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");

  [[window_ window] performSelectorOnMainThread:@selector(performClose:)
                         withObject:nil
                      waitUntilDone:NO];
};

void NativeWindow::setV8Handle(v8::Handle<v8::Object> obj) {
  this->v8handle_ = obj;
}

v8::Handle<v8::Object> NativeWindow::getV8Handle() {
  return this->v8handle_;
}


} /* appjs */

