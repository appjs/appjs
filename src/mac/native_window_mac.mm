#import  <Cocoa/Cocoa.h>
#import  <objc/runtime.h>
#import  "include/cef_application_mac.h"
#include "appjs.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/cef_handler.h"
#include "base/native_window.h"

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
  NSWindow* mainWnd = (NSWindow*)notification.object;
  appjs::NativeWindow* nativewindow = g_handler->GetWindow([mainWnd contentView]);
  nativewindow->GetBrowser()->SetFocus(true);
}

- (void)windowWillEnterFullScreen:(NSNotification*)notification {
  NSWindow* mainWnd = (NSWindow*)notification.object;
  [mainWnd setFrame:[mainWnd frameRectForContentRect:[[NSScreen mainScreen] frame]] display:YES];
}
// Called when the window is about to close. Perform the self-destruction
// sequence by getting rid of the window. By returning YES, we allow the window
// to be removed from the screen.
- (BOOL)windowShouldClose:(id)window {

  appjs::NativeWindow* nativewindow = g_handler->GetWindow([window contentView]);
  nativewindow->GetBrowser()->ParentWindowWillClose();

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

void NativeWindow::Init (char* url, Settings* settings) {
  mainWndSettings = settings;
  mainWndUrl = url;
  // if it is the first time NativeWindow is called, create the Application.
  if (is_main_window_) {
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

  // Center the window if user didn't specified x or y
  if( rect_.top < 0 || rect_.left < 0 ) {
    rect_.left = (appjs::NativeWindow::ScreenWidth() - rect_.width ) / 2;
    rect_.top = (appjs::NativeWindow::ScreenHeight() - rect_.height ) / 2;
  }

  NSUInteger styles;

  if( ! show_chrome_ ) {
    styles = NSBorderlessWindowMask;
  } else {
    styles = NSTitledWindowMask |
             NSClosableWindowMask |
             NSMiniaturizableWindowMask;
  }

  if( resizable_ ) {
    styles |= NSResizableWindowMask;
  }

  // Create the main application window.
  NSRect window_rect = { {rect_.left, rect_.top} , {rect_.width, rect_.height} };

  // Create the window
  NSWindow* mainWnd = [[NSWindow alloc]
                       initWithContentRect:window_rect
                       styleMask:(styles)
                       backing:NSBackingStoreBuffered
                       defer:NO];
  [mainWnd setAlphaValue:opacity_];
  [mainWnd setTitle:@"cefclient"];
  [mainWnd setDelegate:windowDelegate];
  [mainWnd setReleasedWhenClosed:YES];
  [mainWnd setOpaque:NO];
  [mainWnd setBackgroundColor:[NSColor clearColor]];

  Wrapper* wrap = [[Wrapper alloc] initWithV8Object:this];
  objc_setAssociatedObject(mainWnd,(char *)("nativewindow"),wrap,OBJC_ASSOCIATION_RETAIN_NONATOMIC);

  if(fullscreen_) {
    Fullscreen();
  }

  // Add browser view to newly created window.
  NSView* contentView = [mainWnd contentView];
  handle_ = contentView;
  appjs::Cef::AddWebView(contentView,url,settings);

  // Size the window.
  //[mainWnd setFrame:[mainWnd frameRectForContentRect:[mainWnd frame]] display:YES];
  // Run CEF message loop
  appjs::Cef::Run();

}


void NativeWindow::Show() {
  [[handle_ window] makeKeyAndOrderFront: nil];
};

void NativeWindow::Hide() {
  [[handle_ window] orderOut: nil];
};

int NativeWindow::ScreenWidth() {
  NSRect screen_rect = [[NSScreen mainScreen] visibleFrame];
  return screen_rect.size.width;
}

int NativeWindow::ScreenHeight() {
  NSRect screen_rect = [[NSScreen mainScreen] visibleFrame];
  return screen_rect.size.height;
}

void NativeWindow::SetWindowTitle(CefWindowHandle handle, const char* title) {
  NSWindow* win = [handle window];
  [win setTitle: [NSString stringWithUTF8String:title]];
}


void NativeWindow::Destroy() {
  [[handle_ window] performSelectorOnMainThread:@selector(performClose:)
                         withObject:nil
                      waitUntilDone:NO];
};

const char* NativeWindow::GetTitle() {
  return [[[handle_ window] title] cStringUsingEncoding:NSASCIIStringEncoding];
}


void NativeWindow::Minimize() {
  [[handle_ window] performSelectorOnMainThread:@selector(performMiniaturize:)
                         withObject:nil
                      waitUntilDone:NO];
}

void NativeWindow::Maximize() {
  [[handle_ window] performSelectorOnMainThread:@selector(performZoom:)
                         withObject:nil
                      waitUntilDone:NO];
}

void NativeWindow::Restore() {
  NSWindow* win = [handle_ window];
  if( [win isZoomed] ){
    [win performSelectorOnMainThread:@selector(zoom:)
                         withObject:nil
                      waitUntilDone:NO];
  }
  if ( fullscreen_ ) {

#ifdef NSAppKitVersionNumber10_6
    if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6) {
      [win toggleFullScreen: nil];
    } else {
#endif
      NSUInteger styles = NSTitledWindowMask |
                          NSClosableWindowMask |
                          NSMiniaturizableWindowMask;
      [win setStyleMask:(styles)];
      [[NSApplication sharedApplication] setPresentationOptions: NSApplicationPresentationDefault];
      NSRect window_rect = { {rect_.left, rect_.top} , {rect_.width, rect_.height} };
      [win setFrame:[win frameRectForContentRect: window_rect] display:YES];
#ifdef NSAppKitVersionNumber10_6
    }
#endif
    fullscreen_ = false;

  }
  if( [win isMiniaturized]) {
    [win performSelectorOnMainThread:@selector(deminiaturize:)
                         withObject:nil
                      waitUntilDone:NO];
  }

}


void NativeWindow::Fullscreen(){
  NSWindow* win = [handle_ window];

  if(fullscreen_) return;

#ifdef NSAppKitVersionNumber10_6
  if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6) {
    [win setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary];
    [win toggleFullScreen: nil];
  } else {
#endif
    NSUInteger styles = NSBorderlessWindowMask;
    [win setStyleMask:(styles)];
    [[NSApplication sharedApplication] setPresentationOptions: NSApplicationPresentationAutoHideDock | NSApplicationPresentationAutoHideMenuBar];
#ifdef NSAppKitVersionNumber10_6
  }
#endif
  [win setFrame:[win frameRectForContentRect:[[NSScreen mainScreen] frame]] display:YES];
  fullscreen_ = true;
}


void NativeWindow::Drag() {

}

void NativeWindow::Move(int top, int left, int width, int height) {
  NSRect windowRect = { { left  , top } , { width , height} };
  [[handle_ window] setFrame:[[handle_ window] frameRectForContentRect: windowRect] display:YES];
}

void NativeWindow::Move(int top, int left) {
  NSRect windowRect = [[handle_ window] frame];
  windowRect.origin.y = top;
  windowRect.origin.y = left;
  [[handle_ window] setFrame:[[handle_ window] frameRectForContentRect: windowRect] display:YES];
}

void NativeWindow::Resize(int width, int height) {
  NSRect windowRect = [[handle_ window] frame];
  windowRect.size.width = width;
  windowRect.size.height = height;
  [[handle_ window] setFrame:[[handle_ window] frameRectForContentRect: windowRect] display:YES];
}

void NativeWindow::UpdatePosition(){
  NSRect rect = [[handle_ window] frame];
  rect_.width  = rect.size.width;
  rect_.height = rect.size.height;
  rect_.top    = rect.origin.y;
  rect_.left   = rect.origin.x;
}

NW_STATE NativeWindow::GetState(){
  NSWindow* win = [handle_ window];
  if (fullscreen_) {
    return NW_STATE_FULLSCREEN;
  } else if ([win isZoomed]) {
    return NW_STATE_MAXIMIZED;
  } else if ([win isMiniaturized]) {
    return NW_STATE_MINIMIZED;
  } else {
    return NW_STATE_NORMAL;
  }
}

void NativeWindow::SetTopmost(bool ontop){
  NSWindow* win = [handle_ window];
  if (ontop) {
    [win setLevel:NSScreenSaverWindowLevel];
  } else {
    [win setLevel:NSNormalWindowLevel];
  }
  topmost_ = ontop;
}

void NativeWindow::SetResizable(bool resizable) {
  NSWindow* win = [handle_ window];
  [win setStyleMask:(resizable ? [win styleMask] | NSResizableWindowMask
                               : [win styleMask] & ~NSResizableWindowMask)];
}

bool NativeWindow::GetResizable() {
  return [[handle_ window] styleMask] & NSResizableWindowMask;
}

void NativeWindow::SetShowChrome(bool showChrome) {
  NSWindow* win = [handle_ window];
  [win setStyleMask:(showChrome ? [win styleMask] | NSBorderlessWindowMask
                               : [win styleMask] & ~NSBorderlessWindowMask)];

}

bool NativeWindow::GetShowChrome() {
  return [[handle_ window] styleMask] & NSBorderlessWindowMask;
}

void NativeWindow::SetAlpha(bool alpha) {
  [[handle_ window] setOpaque:alpha];
}

bool NativeWindow::GetAlpha() {
  return [[handle_ window] isOpaque];
}

void NativeWindow::SetOpacity(double opacity) {
  [[handle_ window] setAlphaValue:opacity];
}

double NativeWindow::GetOpacity() {
  return [[handle_ window] alphaValue];
}

} /* appjs */

