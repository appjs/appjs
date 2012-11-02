#import  <Cocoa/Cocoa.h>
#import  <objc/runtime.h>
#import  "include/cef_application_mac.h"
#include "appjs.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/cef_handler.h"
#include "native_status_icon/native_status_icon.h"


@interface StatusIconActionManager : NSObject {
  appjs::NativeStatusIcon* me_;
}
- (id)initWithObject2:(appjs::NativeStatusIcon*)me_;

@end

@interface StatusIconActionManager ()
@property (nonatomic,readwrite,assign)appjs::NativeStatusIcon* me;
@end

@implementation StatusIconActionManager

@synthesize me = me_;

- (id)initWithObject2:(appjs::NativeStatusIcon*)obj {
  self = [super init];
  if(self != nil){
    self.me = obj;
  }
  return self;
}

- (void)emitClick:(id)sender {
  self.me->Emit("click");
}

- (void)emitPopup:(id)sender {
  self.me->Emit("rightclick");
}
@end

// The global ClientHandler reference.
extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

void NativeStatusIcon::Init(Settings* settings) {
  NSString* iconFile = [NSString stringWithUTF8String:settings->getString("icon","")];
  NSString* tooltip  = [NSString stringWithUTF8String:settings->getString("tooltip","")];
  bool  visible      = settings->getBoolean("visible",true);

  // TODO local or persistent?
  v8::Local<v8::Object> menu = settings->getObject("menu");

  NativeMenu* nativeMenu;
  NSStatusBar* bar = [NSStatusBar systemStatusBar];
  NSStatusItem* statusIcon  = [bar statusItemWithLength:NSSquareStatusItemLength];
  [statusIcon retain];

  if( settings->has("icon") ) {
    [statusIcon setImage:[[NSImage alloc] initByReferencingFile:iconFile]];
  } else {
    //?
  }

  statusIconHandle_ = statusIcon;

  if( settings->has("tooltip") ) {
    [statusIcon setTitle:tooltip];
  }

  if( settings->has("menu") ) {

    NSMenu* nsmenu = [[NSMenu alloc] initWithTitle:tooltip];
    nativeMenu = (NativeMenu*)menu->GetPointerFromInternalField(0);
    nativeMenu->Attach(nsmenu);

    [statusIcon setMenu:nsmenu];

  }

  StatusIconActionManager* statusIconManager = [[StatusIconActionManager alloc] initWithObject2:this];

  [statusIcon setTarget:statusIconManager];
  [statusIcon setAction:@selector(emitClick:)];
  [statusIcon setDoubleAction:@selector(emitPopup:)];
  [statusIcon setEnabled:visible];
}

void NativeStatusIcon::Show(){
  [statusIconHandle_ setEnabled:NO];
  this->Emit("show");
}

void NativeStatusIcon::Hide(){
  [statusIconHandle_ setEnabled:YES];
  this->Emit("hide");
}

} /* appjs */

