#include <node.h>
#include <windows.h>
#include <algorithm>
#define min(left,right) std::min(left,right)
#define max(left,right) std::max(left,right)
#include <gdiplus.h>
#include "appjs.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/util_win.h"
#include "includes/cef_handler.h"
#include "native_menu/native_menu.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

void NativeMenu::Init(Settings* settings) {
  HMENU menu = CreateMenu();
  
  MENUINFO menuInfo;
  memset(&menuInfo, 0, sizeof(menuInfo));
  menuInfo.cbSize = sizeof(menuInfo);
  menuInfo.fMask = MIM_STYLE;
  menuInfo.dwStyle = MNS_NOTIFYBYPOS;
  SetMenuInfo(menu,&menuInfo);
  AddSubMenu(menu,settings);
  menu_ = menu;
}

int NativeMenu::AddSubMenu(HMENU& menu,Settings* settings){
  int length = settings->getInteger("length",0);

  Settings* item;
  TCHAR* label;
  TCHAR* icon;
  appjs_action_callback* actionCb;

  for( int i = 0; i < length; i++ ) {

    item = new Settings( settings->getObject( i ) );
    label  = item->getString("label",TEXT(""));
    icon   = item->getString("icon",TEXT(""));
    actionCb = new appjs_action_callback();
    actionCb->action = Persistent<Object>::New( item->getObject("action") );
    actionCb->item = Persistent<Object>::New( settings->getObject( i ) );
    actionCb->menu = this;

    Settings* subsettings = new Settings(item->getObject("submenu"));
    HMENU submenu = CreateMenu();
    if(AddSubMenu(submenu,subsettings)) { // has submenu
      MENUINFO menuInfo;
      memset(&menuInfo, 0, sizeof(menuInfo));
      menuInfo.cbSize = sizeof(menuInfo);
      menuInfo.fMask = MIM_STYLE;
      menuInfo.dwStyle = MNS_NOTIFYBYPOS;
      SetMenuInfo(submenu,&menuInfo);
      AppendMenu(menu,MF_POPUP,(UINT)submenu,label);
    } else { // does not have submenu
      MENUITEMINFO menuItemInfo;
      menuItemInfo.cbSize = sizeof(MENUITEMINFO);
      menuItemInfo.fMask = MIIM_DATA;
      menuItemInfo.dwTypeData = label;
      menuItemInfo.dwItemData =(ULONG_PTR) actionCb;
      menuItemInfo.cch = wcslen(label);

      if( wcslen(label) == 0 ) {
        menuItemInfo.fMask |= MIIM_TYPE;
        menuItemInfo.fType = MF_SEPARATOR;
      } else {
        menuItemInfo.fMask |= MIIM_STRING;
        if(item->has("icon")) {
          menuItemInfo.fMask |= MIIM_BITMAP;
          HBITMAP bitmap;
          Gdiplus::Color color;
          Gdiplus::Bitmap* img = Gdiplus::Bitmap::FromFile(icon);
          img->GetHBITMAP(color, &bitmap);
          menuItemInfo.hbmpItem = bitmap;
        }
      }
      InsertMenuItem(menu,i,false,&menuItemInfo);
    }
  }

  return length;
}

bool NativeMenu::Attach(HMENU& menuBar) {
  if(!attached_) {
    menuBar = menu_;
    attached_ = true;
    this->Emit("attached");
    return true;
  } else {
    // already attached
    return false;
  }
}


bool NativeMenu::Detach(HMENU& menuBar) {
  if(attached_) {
    attached_ = false;
    this->Emit("detached");
    return true;
  } else {
    // already detached
    return false;
  }
}

} /* appjs */
