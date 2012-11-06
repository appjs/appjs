#import  <Cocoa/Cocoa.h>
#import  <objc/runtime.h>
#import  "include/cef_application_mac.h"
#include "appjs.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/cef_handler.h"
#include "native_menu/native_menu.h"

@interface MenuActionManager : NSObject {
  appjs::appjs_action_callback* actionCb_;
}
- (id)initWithAction:(appjs::appjs_action_callback*)actionCb_;

@end

@interface MenuActionManager ()
@property (nonatomic,readwrite,assign)appjs::appjs_action_callback* actionCb;
@end

@implementation MenuActionManager

@synthesize actionCb = actionCb_;

- (id)initWithAction:(appjs::appjs_action_callback*)action {
  self = [super init];
  if(self != nil){
    self.actionCb = action;
  }
  return self;
}

- (void)handleAction:(id)sender {
  v8::Persistent<v8::Object> action = self.actionCb->action;
  appjs::NativeMenu* menu = self.actionCb->menu;

  if(action->IsCallable()) {
    const int argc = 1;
    v8::Handle<v8::Value> argv[argc] = {self.actionCb->item};
    action->CallAsFunction(menu->GetV8Handle(),argc,argv);
  }

  menu->Emit("select",v8::Local<v8::Object>::New(self.actionCb->item));
}
@end
// The global ClientHandler reference.
extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

void NativeMenu::Init(Settings* settings) {
  //NSArray* menuItems = [NSArray alloc];
  NSMenu* menu = [[NSMenu alloc] initWithTitle:@"Sample"];
  AddSubMenu(menu,settings);

  NSArray* menuItems = [menu itemArray];

  //for(NSMenuItem* menuItem in menuItems){
  	menuItems_ = menuItems;
  //}
  [menu removeAllItems];
}

int NativeMenu::AddSubMenu(NSMenu* menu,Settings* settings){
  int length = settings->getInteger("length",0);

  Settings* item;
  NSMenuItem* menuItem;
  NSString* label;
  NSString* icon;
  appjs_action_callback* actionCb;
  MenuActionManager* actionManager;

  for( int i = 0; i < length; i++ ) {

    item = new Settings( settings->getObject( i ) );
    label  = [NSString stringWithUTF8String:item->getString("label","")];
    icon   = [NSString stringWithUTF8String:item->getString("icon","")];
    actionCb = new appjs_action_callback();
    actionCb->action = v8::Persistent<v8::Object>::New( item->getObject("action") );
    actionCb->item = v8::Persistent<v8::Object>::New( settings->getObject( i ) );
    actionCb->menu = this;
    actionManager = [[MenuActionManager alloc] initWithAction:actionCb];

    if( [label length] == 0 ) {
      menuItem = [NSMenuItem separatorItem];
    } else if( [icon length] == 0 ) {
      menuItem = [[NSMenuItem alloc] initWithTitle:label action:NULL keyEquivalent:@""];
    } else {
      menuItem = [[NSMenuItem alloc] initWithTitle:label action:NULL keyEquivalent:@""];
      [menuItem setImage:[[NSImage alloc] initByReferencingFile:icon]];
    }
    [menuItem setEnabled:YES];
    [menuItem setTitleWithMnemonic:label];

    Settings* subsettings = new Settings(item->getObject("submenu"));
    NSMenu* submenu = [[NSMenu alloc] initWithTitle:label];

    if(AddSubMenu(submenu,subsettings)){
      [menuItem setSubmenu:submenu];
    } else {
      [menuItem setTarget:actionManager];
      [menuItem setAction:@selector(handleAction:)];
    }

    [menu addItem:menuItem];
  }

  return length;
}

bool NativeMenu::Attach(NSMenu* menu) {
  if(!attached_) {
    for(NSMenuItem* menuItem in menuItems_) {
      [menu addItem:menuItem];
    }

    this->Emit("attached");

    return true;
  }

  return FALSE;
}

} /* appjs */

