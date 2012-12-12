#include <node.h>
#include <windows.h>
#include <algorithm>
#define min(left,right) std::min(left,right)
#define max(left,right) std::max(left,right)
#include <gdiplus.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "appjs.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/util_win.h"
#include "includes/cef_handler.h"
#include "native_status_icon/native_status_icon.h"
#include "native_menu/native_menu.h"

extern CefRefPtr<ClientHandler> g_handler;


namespace status_icon {

#define MAX_LOADSTRING 100
#define SWP_STATECHANGED 0x8000

TCHAR szWindowClass[MAX_LOADSTRING];
LRESULT CALLBACK StatusIconProc(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass(HINSTANCE hInstance);
HINSTANCE hInstance;
HWND hWnd;

LRESULT CALLBACK StatusIconProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch(message) {
    case WM_MENUCOMMAND: {
      HMENU menu = (HMENU)lParam;
      MENUITEMINFO menuItem;
      menuItem.cbSize = sizeof(MENUITEMINFO);
      menuItem.fMask = MIIM_DATA;
      int idx = wParam;
      GetMenuItemInfo(menu,idx,TRUE,&menuItem);

      appjs::appjs_action_callback* actionCallback = (appjs::appjs_action_callback*) menuItem.dwItemData;
      appjs::NativeMenu* nativeMenu = actionCallback->menu;

      if( actionCallback == NULL) {
        return 0;
      }

      v8::Persistent<v8::Object> action = actionCallback->action;

      if(action->IsCallable()) {
        const int argc = 1;
        v8::Handle<v8::Value> argv[argc] = {actionCallback->item};
        action->CallAsFunction(nativeMenu->GetV8Handle(),argc,argv);
      }

      nativeMenu->Emit("select",v8::Local<v8::Object>::New(actionCallback->item));
      return 0;
    }
    case WM_USER:
      switch(lParam) {
        case WM_LBUTTONDBLCLK: {
          appjs::NativeStatusIcon* nativeStatusIcon = (appjs::NativeStatusIcon*) GetWindowLongPtr(hWnd, GWLP_USERDATA);
          nativeStatusIcon->Emit("dblclick");
          return true;
        }
        case WM_RBUTTONUP: {

          HMENU   hPop;
          int     i = 0;
          WORD    cmd;
          POINT   pt;
          POINT*  curpos;

          appjs::NativeStatusIcon* nativeStatusIcon = (appjs::NativeStatusIcon*) GetWindowLongPtr(hWnd, GWLP_USERDATA);
          appjs::NativeMenu* nativeMenu = nativeStatusIcon->GetMenu();
          nativeMenu->Attach(hPop);

          HMENU popup = CreatePopupMenu();
          int loop = GetMenuItemCount(hPop);
          for(int i = 0; i < loop; i++) {
            TCHAR* menuTitle = new TCHAR[256];
            GetMenuString(hPop,i,menuTitle,256,MF_BYPOSITION);
            if(IsMenu(GetSubMenu(hPop,i))) {
              AppendMenu(popup,MF_POPUP, (UINT)GetSubMenu(hPop,i), menuTitle); 
            } else {
              MENUITEMINFO menuItem;
              menuItem.cbSize = sizeof(MENUITEMINFO) - 4;
              menuItem.fMask = MIIM_DATA | MIIM_STRING;
              menuItem.dwItemData = NULL;
              menuItem.dwTypeData = NULL;
              GetMenuItemInfo(hPop,i,TRUE,&menuItem);
              menuItem.dwTypeData = menuTitle;
              InsertMenuItem(popup,i,TRUE,&menuItem);
            }
          }

          MENUINFO menuInfo;
          memset(&menuInfo, 0, sizeof(menuInfo));
          menuInfo.cbSize = sizeof(menuInfo);
          menuInfo.fMask = MIM_STYLE;
          menuInfo.dwStyle = MNS_NOTIFYBYPOS | MNS_AUTODISMISS;
          SetMenuInfo(popup,&menuInfo);

          GetCursorPos( &pt );
          curpos = &pt;

          SendMessage( hWnd, WM_INITMENUPOPUP, (WPARAM)popup, 0 );

          TrackPopupMenu( popup, TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                                curpos->x, curpos->y, 0, hWnd, NULL );

          nativeMenu->Detach(hPop);
//          SendMessage( hWnd, WM_MENUCOMMAND, cmd, (UINT)popup );

          nativeStatusIcon->Emit("rightclick");
          return true;
        }
        case WM_LBUTTONUP: {
          appjs::NativeStatusIcon* nativeStatusIcon = (appjs::NativeStatusIcon*) GetWindowLongPtr(hWnd, GWLP_USERDATA);
          nativeStatusIcon->Emit("click");
          return true;
        }
      }
    break;
  }
  return DefWindowProc(hwnd, message, wParam, lParam);
}

HMODULE GetCurrentModuleHandle() {
  HMODULE module = NULL;
  GetModuleHandleExW(6, reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle), &module);
  return module;
}

ATOM MyRegisterClass(HINSTANCE hInst) {
  WNDCLASSEX wcex = {0};
  wcex.cbSize        = sizeof(WNDCLASSEX);
  wcex.style         = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc   = StatusIconProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = hInst;
  wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wcex.lpszMenuName  = NULL;
  wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wcex.lpszClassName = szWindowClass;
  return RegisterClassEx(&wcex);
}

}

namespace appjs {

using namespace v8;
using namespace status_icon;

void NativeStatusIcon::Init(Settings* settings) {
  TCHAR* iconFile = settings->getString("icon",TEXT(""));
  TCHAR* tooltip  = settings->getString("tooltip",TEXT(""));
  bool   visible  = settings->getBoolean("visible",true);

  hInstance = (HINSTANCE)GetCurrentModuleHandle();
  wcscpy(szWindowClass, TEXT("AppjsNotifyWindow"));
  MyRegisterClass(hInstance);

  hWnd = CreateWindowEx(NULL, szWindowClass, TEXT(""), WS_OVERLAPPEDWINDOW,
                           0, 0, 0, 0,
                           NULL, NULL, hInstance, NULL);
  SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)this);

  // TODO local or persistent?
  Local<Object> menu = settings->getObject("menu");
  NativeMenu* nativeMenu;
  NOTIFYICONDATA statusIcon;
  statusIcon.cbSize = sizeof(statusIcon);
  statusIcon.hWnd = hWnd;
  statusIcon.uID = 0;
  statusIcon.uFlags = NIF_MESSAGE;
  statusIcon.uVersion = NOTIFYICON_VERSION;
  statusIcon.uCallbackMessage = WM_USER;

  if( !visible ) {
    statusIcon.uFlags |= NIF_STATE;
    statusIcon.dwState = NIS_HIDDEN;
  }

  if( settings->has("icon") ) {
    HICON icon;
    Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(iconFile);
    if (bitmap->GetWidth()) {
      bitmap->GetHICON(&icon);
      delete bitmap;
    }
    statusIcon.uFlags |= NIF_ICON;
    statusIcon.hIcon = icon;
  } else {
    // Use predefined system icons
    statusIcon.hIcon = LoadIcon(hInstance,IDC_NO);
  }

  if( settings->has("tooltip") ) {
    statusIcon.uFlags |=  NIF_TIP;
    wcscpy(statusIcon.szTip,tooltip);
  }

  if( settings->has("menu") ) {
    menu_ = (NativeMenu*)menu->GetPointerFromInternalField(0);
  }

  statusIconHandle_ = statusIcon;

  Shell_NotifyIcon(NIM_ADD, &statusIcon);
}

void NativeStatusIcon::Show(){
  Shell_NotifyIcon(NIM_ADD, &statusIconHandle_);
  this->Emit("show");
}

void NativeStatusIcon::Hide(){
  Shell_NotifyIcon(NIM_DELETE, &statusIconHandle_);
  this->Emit("hide");
}

} /* appjs */
