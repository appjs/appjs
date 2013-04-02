#import  <Cocoa/Cocoa.h>
#import  <objc/runtime.h>
#import  "include/cef_application_mac.h"
#include "appjs.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/cef_handler.h"
#include "native_window/native_window.h"

#define MAC_OS_X_VERSION MAC_OS_X_VERSION_MAX_ALLOWED

// Make sure the version number defines exist; when compiling on 10.6, NSAppKitVersionNumber10_6 isn't defined
#ifndef NSAppKitVersionNumber10_6
#define NSAppKitVersionNumber10_6 1038
#endif

// The global ClientHandler reference.
extern CefRefPtr<ClientHandler> g_handler;

appjs::Settings* mainWndSettings;
char* mainWndUrl;
// Memory AutoRelease pool.
static NSAutoreleasePool* g_autopool = nil;

@interface CocoaMultiThreading : NSObject
+ (void)beginMultiThreading;
@end

@implementation CocoaMultiThreading
+ (void) dummyThread:(id)unused {
  (void)unused;
}
+ (void) beginMultiThreading {
  [NSThread detachNewThreadSelector:@selector(dummyThread:)
            toTarget:self
            withObject:nil
  ];
}
@end

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

@interface AppjsWindow : NSWindow
@end

@implementation AppjsWindow
-(BOOL)canBecomeKeyWindow {
  return YES;
}
-(BOOL)canBecomeMainWindow {
  return YES;
}
@end
// Receives notifications from the browser window. Will delete
// itself when done.
@interface AppjsWindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation AppjsWindowDelegate

- (void)windowDidBecomeKey:(NSNotification*)notification {
  NSWindow* mainWnd = (NSWindow*)notification.object;
  appjs::NativeWindow* nativewindow = appjs::NativeWindow::GetWindow([mainWnd contentView]);
  nativewindow->GetBrowser()->SetFocus(true);
}

- (void)windowWillEnterFullScreen:(NSNotification*)notification {
  NSWindow* mainWnd = (NSWindow*)notification.object;
  [mainWnd setFrame:[mainWnd frameRectForContentRect:[[NSScreen mainScreen] frame]] display:YES];
}

- (void)windowDidEnterFullScreen:(NSNotification*)notification {
  NSWindow* window = (NSWindow*)notification.object;
  appjs::NativeWindow* nativewindow = appjs::NativeWindow::GetWindow([window contentView]);
  nativewindow->Emit("fullscreen");
}

- (void)windowDidExitFullScreen:(NSNotification*)notification {
  NSWindow* window = (NSWindow*)notification.object;
  appjs::NativeWindow* nativewindow = appjs::NativeWindow::GetWindow([window contentView]);
  nativewindow->Emit("restore");
}

- (void)windowDidMiniaturize:(NSNotification*)notification {
  NSWindow* window = (NSWindow*)notification.object;
  appjs::NativeWindow* nativewindow = appjs::NativeWindow::GetWindow([window contentView]);
  nativewindow->Emit("minimize");
}

- (void)windowDidDeminiaturize:(NSNotification*)notification {
  NSWindow* window = (NSWindow*)notification.object;
  appjs::NativeWindow* nativewindow = appjs::NativeWindow::GetWindow([window contentView]);
  nativewindow->Emit("restore");
}

- (bool)windowShouldZoom:(NSWindow*)window {
  appjs::NativeWindow* nativewindow = appjs::NativeWindow::GetWindow([window contentView]);
  nativewindow->Emit("maximize");
  return YES;
}

- (NSRect)windowWillUseStandardFrame:(NSWindow*)window:(NSRect)frame {
  appjs::NativeWindow* nativewindow = appjs::NativeWindow::GetWindow([window contentView]);
  nativewindow->Emit("maximize");
   return frame;
}

- (void)windowDidResize:(NSNotification*)notification {
  NSWindow* window = (NSWindow*)notification.object;
  appjs::NativeWindow* nativewindow = appjs::NativeWindow::GetWindow([window contentView]);
  NSRect rect = [window frame];
  if(nativewindow != NULL && nativewindow->GetState() != appjs::NW_STATE_FULLSCREEN)
    nativewindow->Emit("resize",rect.size.width,rect.size.height);
}

- (void)windowDidMove:(NSNotification*)notification {
  NSWindow* window = (NSWindow*)notification.object;
  appjs::NativeWindow* nativewindow = appjs::NativeWindow::GetWindow([window contentView]);
  NSRect rect = [window frame];
  nativewindow->Emit("move",rect.origin.x,rect.origin.y);
}
// Called when the window is about to close. Perform the self-destruction
// sequence by getting rid of the window. By returning YES, we allow the window
// to be removed from the screen.
- (BOOL)windowShouldClose:(id)window {

  appjs::NativeWindow* nativewindow = appjs::NativeWindow::GetWindow([window contentView]);
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
- (id)initWithNativeWindow:(appjs::NativeWindow*)window;
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

- (id)initWithNativeWindow:(appjs::NativeWindow*)window {
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

void AddWebView(CefWindowHandle parent, char* url, Settings* settings) {
  CefWindowInfo windowInfo;
  if (settings->getBoolean("alpha",false)) {
    windowInfo.SetTransparentPainting(true);
  }
  g_handler->browserSettings_.web_security_disabled = settings->getBoolean("disableSecurity", false);
  NSRect contentFrame = [parent frame];
  windowInfo.SetAsChild(parent, 0, 0, contentFrame.size.width, contentFrame.size.height);
  CefBrowser::CreateBrowser(windowInfo, g_handler.get(), url, g_handler->browserSettings_);
}

void NativeWindow::Init (char* url, Settings* settings) {
  mainWndSettings = settings;
  mainWndUrl = url;
  // if it is the first time NativeWindow is called, create the Application.
  if (is_main_window_) {
    [CocoaMultiThreading beginMultiThreading];

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

    if( resizable_ ) {
      styles |= NSResizableWindowMask;
    }

  }

  int screenHeight = [[NSScreen mainScreen] visibleFrame].size.height;
  rect_.top = screenHeight - rect_.height - rect_.top;

  // Create the main application window.
  NSRect window_rect = { {rect_.left, rect_.top} , {rect_.width, rect_.height} };

  // Create the window
  NSWindow* mainWnd = [[AppjsWindow alloc]
                       initWithContentRect:window_rect
                       styleMask:(styles)
                       backing:NSBackingStoreBuffered
                       defer:NO];
  [mainWnd setAlphaValue:opacity_];
  [mainWnd setTitle:@"cefclient"];
  [mainWnd setReleasedWhenClosed:YES];
  [mainWnd setOpaque:NO];
  [mainWnd setBackgroundColor:[NSColor clearColor]];

  Wrapper* wrap = [[Wrapper alloc] initWithNativeWindow:this];
  objc_setAssociatedObject(mainWnd,(char *)("nativewindow"),wrap,OBJC_ASSOCIATION_RETAIN_NONATOMIC);

  // Add browser view to newly created window.
  NSView* contentView = [mainWnd contentView];
  handle_ = contentView;

  if(fullscreen_) {
    fullscreen_ = false;
    Fullscreen();
  }

  AddWebView(contentView,url,settings);

  // Size the window.
  //[mainWnd setFrame:[mainWnd frameRectForContentRect:[mainWnd frame]] display:YES];
  [mainWnd setDelegate:windowDelegate];
  // Run CEF message loop
  appjs::Cef::Run();

}


void NativeWindow::Show() {
  [[handle_ window] makeKeyAndOrderFront: nil];
}

void NativeWindow::Focus() {
  [[handle_ window] makeKeyAndOrderFront: nil];
}

void NativeWindow::Hide() {
  [[handle_ window] orderOut: nil];
}

int NativeWindow::ScreenWidth() {
  NSRect screen_rect = [[NSScreen mainScreen] visibleFrame];
  return screen_rect.size.width;
}

int NativeWindow::ScreenHeight() {
  NSRect screen_rect = [[NSScreen mainScreen] visibleFrame];
  return screen_rect.size.height;
}

void NativeWindow::SetTitle(const char* title) {
  NSWindow* win = [handle_ window];
  [win setTitle: [NSString stringWithUTF8String:title]];
}


NativeWindow* NativeWindow::GetWindow(CefWindowHandle handle){
  NSWindow* win = [handle window];
  Wrapper* wrap = (Wrapper*) objc_getAssociatedObject(win, (char *)"nativewindow");
  NativeWindow* window = [wrap handle];
  return window;
}

NativeWindow* NativeWindow::GetWindow(CefRefPtr<CefBrowser> browser){
  return NativeWindow::GetWindow(browser->GetWindowHandle());
}

void NativeWindow::SetMenuBar(NativeMenu* nativeMenu) {
  NSMenu* menu = [[NSMenu alloc] initWithTitle:@"AppJS Menu"];

  nativeMenu->Attach(menu);
  [NSApp setMainMenu:menu];
}

void NativeWindow::OpenFileDialog(uv_work_t* req) {
  AppjsDialogSettings* settings = (AppjsDialogSettings*)req->data;
  NSWindow*              parent = [settings->me->handle_ window];
  std::string      initialValue = settings->initialValue;
  std::string       acceptTypes = settings->reserveString1;
  bool              multiSelect = settings->reserveBool1;
  bool                dirSelect = settings->reserveBool2;

  NSLock* lock = [NSLock new];
  [lock lock];
  if( settings->type == NW_DIALOGTYPE_FILE_OPEN ) {
    NSOpenPanel* dialog;
    dialog = [[NSOpenPanel openPanel] retain];
    [dialog setTitle: [NSString stringWithUTF8String:settings->title.c_str()] ];
    [dialog setDirectoryURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:initialValue.c_str()]]];
    [dialog setCanChooseFiles:!dirSelect];
    [dialog setCanChooseDirectories:dirSelect];
    [dialog setCanCreateDirectories:dirSelect];
    [dialog setAllowsOtherFileTypes:YES];
    [dialog setAllowsMultipleSelection:multiSelect];
    
    NSArray* allowedFileTypes = [[NSString stringWithUTF8String:acceptTypes.c_str()] componentsSeparatedByString:@";"];
    [dialog setAllowedFileTypes:allowedFileTypes];
    [dialog beginSheetModalForWindow:parent completionHandler:^(NSInteger result){
      if( result == NSFileHandlingPanelOKButton) {
        settings->result =  [dialog URLs];
        ProcessFileDialog(req);
      }
      [dialog release];
    }];

  } else {
    NSSavePanel* dialog;
    dialog = [[NSSavePanel savePanel] retain];
    [dialog setTitle: [NSString stringWithUTF8String:settings->title.c_str()] ];
    [dialog setDirectoryURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:initialValue.c_str()]]];
    [dialog setAllowsOtherFileTypes:YES];
    [dialog setCanCreateDirectories:YES];
    [dialog setCanSelectHiddenExtension:YES];
    
    // causes confirmation dialog which then throws exception: Invalid Window
    //NSArray* allowedFileTypes = [[NSString stringWithUTF8String:acceptTypes.c_str()] componentsSeparatedByString:@";"];
    //[dialog setAllowedFileTypes:allowedFileTypes];
    [dialog beginSheetModalForWindow:parent completionHandler:^(NSInteger result){
      if( result == NSFileHandlingPanelOKButton) {
        settings->result   =  [NSArray arrayWithObject: [dialog URL]];
        ProcessFileDialog(req);
      }
      [dialog release];
    }];
  }

  [lock unlock];
}

void NativeWindow::ProcessFileDialog(uv_work_t* req) {

  AppjsDialogSettings* settings = (AppjsDialogSettings*)req->data;
  void*                  result = settings->result;
  Persistent<Function>       cb = settings->cb;

  if( result != NULL ) {
    NSArray*      filenames = (NSArray*) result;
    uint             length = [filenames count];
    Local<Array>     newObj = Array::New(length);
    v8::Handle<Value> error = Undefined();

    for (NSUInteger i = 0; i < length; i++) {
       newObj->Set( i, String::New( [[[filenames objectAtIndex:i] path] UTF8String] ) );
    }

    const int argc  = 2;
    Local<Value> argv[argc] = { Local<Value>::New(error), Local<Value>::New(newObj) };
    cb->Call( settings->me->GetV8Handle(), argc, argv );
  } else {
    const int argc  = 1;
    Local<Value> argv[argc] = { Local<Value>::New(String::New("canceled")) };
    cb->Call( settings->me->GetV8Handle(), argc, argv );
  }

  NativeWindow::DialogClosed();
}

void NativeWindow::OpenColorDialog(uv_work_t* req) {

}

void NativeWindow::ProcessColorDialog(uv_work_t* req) {

}

void NativeWindow::OpenFontDialog(uv_work_t* req) {

}

void NativeWindow::ProcessFontDialog(uv_work_t* req) {

}

void NativeWindow::Destroy() {
  [[handle_ window] performSelectorOnMainThread:@selector(performClose:)
                         withObject:nil
                      waitUntilDone:NO];
}

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

    if (floor(NSAppKitVersionNumber) <= NSAppKitVersionNumber10_6) {
      NSUInteger styles = NSTitledWindowMask |
                          NSClosableWindowMask |
                          NSMiniaturizableWindowMask;
      [win setStyleMask:(styles)];
      [[NSApplication sharedApplication] setPresentationOptions: NSApplicationPresentationDefault];
      NSRect window_rect = { {rect_.left, rect_.top} , {rect_.width, rect_.height} };
      [win setFrame:[win frameRectForContentRect: window_rect] display:YES];
      this->Emit("restore");
    } else {
#if MAC_OS_X_VERSION >= 1070 // Allows you to build on 10.6
      [win toggleFullScreen: nil];
#endif
    }

    fullscreen_ = false;
  }

  if( [win isMiniaturized]) {
    [win performSelectorOnMainThread:@selector(deminiaturize:)
                         withObject:nil
                      waitUntilDone:NO];
  }

}


void NativeWindow::Fullscreen(){

  if(fullscreen_) return;

  fullscreen_ = true;
  NSWindow* win = [handle_ window];

  if (floor(NSAppKitVersionNumber) <= NSAppKitVersionNumber10_6) {
    NSUInteger styles = NSBorderlessWindowMask;
    [win setStyleMask:(styles)];
    [[NSApplication sharedApplication] setPresentationOptions: NSApplicationPresentationAutoHideDock | NSApplicationPresentationAutoHideMenuBar];

    if(this->GetBrowser().get())
      this->Emit("fullscreen");
  } else {
#if MAC_OS_X_VERSION >= 1070 // Allows you to build on 10.6
    [win setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary];
    [win toggleFullScreen: nil];
#endif
  }

  [win setFrame:[win frameRectForContentRect:[[NSScreen mainScreen] frame]] display:YES];
}


void NativeWindow::Drag() {
  NSWindow* win = [handle_ window];
  NSPoint origin = [win frame].origin;
  NSPoint current = [NSEvent mouseLocation];
  NSPoint offset;
  origin.x -= current.x;
  origin.y -= current.y;

  while (YES) {
    NSEvent* event = [win nextEventMatchingMask:(NSLeftMouseDraggedMask | NSLeftMouseUpMask)];
    if ([event type] == NSLeftMouseUp) break;
    current = [win convertBaseToScreen:[event locationInWindow]];
    offset = origin;
    offset.x += current.x;
    offset.y += current.y;
    [win setFrameOrigin:offset];
    [win displayIfNeeded];
  }
}

void NativeWindow::Move(int left, int top, int width, int height) {
  if(fullscreen_) return;

  int screenHeight = [[NSScreen mainScreen] visibleFrame].size.height;
  top = screenHeight - height - top;

  NSRect windowRect = { { left  , top } , { width , height} };
  [[handle_ window] setFrame:[[handle_ window] frameRectForContentRect: windowRect] display:YES];
}

void NativeWindow::Move(int left, int top) {
  if(fullscreen_) return;

  NSRect windowRect = [[handle_ window] frame];
  int screenHeight = [[NSScreen mainScreen] visibleFrame].size.height;
  top = screenHeight - windowRect.size.height - top;

  NSPoint origin = {left,top};
  [[handle_ window] setFrameOrigin:origin];
}

void NativeWindow::Resize(int width, int height) {
  NSRect windowRect = [[handle_ window] frame];
  rect_.width  = windowRect.size.width = width;
  rect_.height = windowRect.size.height = height;
  [[handle_ window] setFrame:[[handle_ window] frameRectForContentRect: windowRect] display:YES];
}

void NativeWindow::UpdatePosition(){
  NSRect rect = [[handle_ window] frame];

  int screenHeight = [[NSScreen mainScreen] visibleFrame].size.height;

  rect_.width  = rect.size.width;
  rect_.height = rect.size.height;
  rect_.top    = screenHeight - rect_.height - rect.origin.y;
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
  resizable_ = resizable;
  [win setStyleMask:(resizable ? [win styleMask] | NSResizableWindowMask
                               : [win styleMask] & ~NSResizableWindowMask)];
}

bool NativeWindow::GetResizable() {
  return [[handle_ window] styleMask] & NSResizableWindowMask;
}

void NativeWindow::SetShowChrome(bool showChrome) {
  NSWindow*    win = [handle_ window];
  NSUInteger style =  NSTitledWindowMask |
                      NSClosableWindowMask |
                      NSMiniaturizableWindowMask;

  if( resizable_ ) {
    style |= NSResizableWindowMask;
  }

  if ( showChrome ) {
    [win setStyleMask:(style)];
  } else {
    [win setStyleMask:(NSBorderlessWindowMask)];
  }


}

bool NativeWindow::GetShowChrome() {
  // NSBorderlessWindowMask is equal to zero. so we can not use
  // bitwise and to determine it.
  return [[handle_ window] styleMask] == NSBorderlessWindowMask;
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

