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
#include "native_window/native_window.h"

#define MAX_LOADSTRING 100
#define SWP_STATECHANGED 0x8000

#if defined(__WIN__)
extern DWMEFICA DwmExtendFrameIntoClientArea;
extern DWMEBBW DwmEnableBlurBehindWindow;
extern DWMWP DwmDefWindowProc;
extern HMODULE dwmapiDLL;
#endif

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

TCHAR szWindowClass[MAX_LOADSTRING];
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass(HINSTANCE hInstance);
HICON smallIcon;
HICON bigIcon;
Settings* browserSettings;
HINSTANCE hInstance;
char* url_;
bool emitFullscreen = false;

// #################################
// ### Windows Utility Functions ###
// #################################


void UpdateStyle(HWND hwnd, int index, LONG value){
  SetWindowLongPtr(hwnd, index, value < 0 ? GetWindowLongPtr(hwnd, index) & value : GetWindowLongPtr(hwnd, index) | value);
  SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

void NewStyle(HWND hwnd, int index, LONG value){
  SetWindowLongPtr(hwnd, index, value);
  SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

void BlurBehind(HWND hwnd, bool enable){
  if (DwmEnableBlurBehindWindow != NULL) {
    BLURBEHIND bb = {0};
    bb.fEnable = enable;
    bb.hRgnBlur = NULL;
    bb.dwFlags = 1;
    DwmEnableBlurBehindWindow(hwnd, &bb);
  }
}

void MakeIcon(HICON* icon, TCHAR* path) {
  Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(path);
  if (bitmap->GetWidth()) {
    bitmap->GetHICON(icon);
    delete bitmap;
  }
}

HICON MakeIcon(TCHAR* path) {
  HICON icon;
  MakeIcon(&icon, path);
  return icon;
}


void SetNCWidth(HWND hwnd, int left, int right, int top, int bottom){
  if (DwmExtendFrameIntoClientArea != NULL) {
    MARGINS margins = {left, right, top, bottom};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
  }
}

void SetNCWidth(HWND hwnd, int size){
  SetNCWidth(hwnd, size, size, size, size);
}

void ForceForegroundWindow(HWND hwnd) {
  DWORD lockTime = 0;
  DWORD localTID = GetCurrentThreadId();
  DWORD activeTID = GetWindowThreadProcessId(GetForegroundWindow(), 0);

  if (localTID != activeTID) {
    AttachThreadInput(localTID, activeTID, true);
    SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &lockTime, 0);
    SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, 0, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
    AllowSetForegroundWindow(ASFW_ANY);
  }

  SetForegroundWindow(hwnd);

  if (localTID != activeTID) {
    SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (PVOID)lockTime, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
    AttachThreadInput(localTID, activeTID, false);
  }
}


HMODULE GetCurrentModuleHandle() {
  HMODULE module = NULL;
  GetModuleHandleExW(6, reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle), &module);
  return module;
}


// #####################################
// ### Static NativeWindow Functions ###
// #####################################

int NativeWindow::ScreenWidth() {
  return GetSystemMetrics(SM_CXSCREEN);
}

int NativeWindow::ScreenHeight() {
  return GetSystemMetrics(SM_CYSCREEN);
}

NativeWindow* NativeWindow::GetWindow(CefWindowHandle handle){
  return (NativeWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
}

NativeWindow* NativeWindow::GetWindow(CefRefPtr<CefBrowser> browser){
  return GetWindow(GetParent(browser->GetWindowHandle()));
}

void AddWebView(CefWindowHandle parent, RECT windowRect, char* url, Settings* settings) {
  CefWindowInfo windowInfo;
  windowInfo.SetAsChild(parent, windowRect);
  if(settings->getBoolean("alpha",false)) {
    windowInfo.SetTransparentPainting(true);
  }
  g_handler->browserSettings_.web_security_disabled = settings->getBoolean("disableSecurity", false);
  CefBrowser::CreateBrowser(windowInfo, static_cast<CefRefPtr<CefClient>>(g_handler), url, g_handler->browserSettings_);
}


// ############################
// ### NativeWindow methods ###
// ############################

void NativeWindow::Init(char* url, Settings* settings) {
  url_ = url;

  if (is_main_window_) {

    TCHAR* wSmallIconPath = icons->getString("small", TEXT(""));
    TCHAR* wBigIconPath = icons->getString("big", TEXT(""));

    smallIcon = MakeIcon(wSmallIconPath);
    bigIcon = MakeIcon(wSmallIconPath);

    delete[] wSmallIconPath;
    delete[] wBigIconPath;

    hInstance = (HINSTANCE)GetCurrentModuleHandle();
    wcscpy(szWindowClass, TEXT("AppjsWindow"));
    MyRegisterClass(hInstance);
  }

  browserSettings = settings;

  if (rect_.left < 0 || rect_.top < 0) {
    rect_.left = (GetSystemMetrics(SM_CXSCREEN) - rect_.width) / 2;
    rect_.top = (GetSystemMetrics(SM_CYSCREEN) - rect_.height) / 2;
  }
  browser_ = NULL;
  handle_ = CreateWindowEx(NULL, szWindowClass, TEXT(""), WS_OVERLAPPEDWINDOW,
                           rect_.left, rect_.top, rect_.width, rect_.height,
                           NULL, NULL, hInstance, NULL);

  SetWindowLongPtr(handle_, GWLP_USERDATA, (LONG)this);

  if (!show_on_taskbar_) {
    long style= GetWindowLong(handle_, GWL_EXSTYLE);
    style |= WS_EX_TOOLWINDOW;
    style &= ~(WS_EX_APPWINDOW);
    ShowWindow(handle_, SW_HIDE); 
    SetWindowLongPtr(handle_, GWL_EXSTYLE, style);
    ShowWindow(handle_, SW_HIDE);
  }

  if (alpha_) {
    SetAlpha(true);
  }

  SetResizable(resizable_);

  if (fullscreen_) {
    fullscreen_ = false;
    Fullscreen();
  } else if (!show_chrome_) {
    SetShowChrome(false);
  }

  UpdateWindow(handle_);

};


void NativeWindow::Minimize() {
  ShowWindow(handle_, SW_MINIMIZE);
}

void NativeWindow::Maximize() {
  ShowWindow(handle_, SW_MAXIMIZE);
}

void NativeWindow::Restore() {
  if (fullscreen_) {
    fullscreen_ = false;
    Move(restoreRect_);
    SetWindowLongPtr(handle_, GWL_STYLE, restoreStyle_);
    SetWindowLongPtr(handle_, GWL_EXSTYLE, restoreExStyle_);
  } else {
    ShowWindow(handle_, SW_RESTORE);
  }
}

void NativeWindow::Show() {
  ShowWindow(handle_, SW_SHOWNORMAL);
  ForceForegroundWindow(handle_);
}

void NativeWindow::Focus() {
  SetActiveWindow(handle_);
}

void NativeWindow::Hide() {
  ShowWindow(handle_, SW_HIDE);
}

void NativeWindow::Destroy() {
  DestroyWindow(handle_);
}

void NativeWindow::Drag() {
  ReleaseCapture();
  SendMessage(handle_, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

void NativeWindow::Fullscreen(){
  if (!fullscreen_) {
    fullscreen_ = true;
    UpdatePosition();
    restoreRect_.left = rect_.left;
    restoreRect_.top = rect_.top;
    restoreRect_.width = rect_.width;
    restoreRect_.height = rect_.height;
    restoreStyle_ = GetWindowLong(handle_, GWL_STYLE);
    restoreExStyle_ = GetWindowLong(handle_, GWL_EXSTYLE);
    SetWindowLongPtr(handle_, GWL_STYLE, restoreStyle_ & ~(WS_CAPTION | WS_SIZEBOX));
    SetWindowLongPtr(handle_, GWL_EXSTYLE, restoreExStyle_ & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
    SetWindowPos(handle_, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    HDC hDC = GetWindowDC(NULL);
    emitFullscreen = true;
    SetWindowPos(handle_, NULL, 0, 0, GetDeviceCaps(hDC, HORZRES), GetDeviceCaps(hDC, VERTRES), SWP_FRAMECHANGED);
    UpdatePosition();
  }
}


// void NativeWindow::SetAppIcon(NW_ICONSIZE size, wchar_t* path) {
//   int flag;
//   switch (size) {
//     case NW_ICONSIZE_SMALLER: flag = ICON_SMALL; break;
//     case NW_ICONSIZE_SMALL:   flag = ICON_SMALL; break;
//     case NW_ICONSIZE_BIG:     flag = ICON_BIG; break;
//     case NW_ICONSIZE_BIGGER:  flag = ICON_BIG; break;
//   }
//   SetClassLongPtr(hwnd, (int)large, (LONG_PTR)MakeIcon(path));
// }


void NativeWindow::SetIcon(NW_ICONSIZE size, TCHAR* path) {
  int flag;
  switch (size) {
    case NW_ICONSIZE_SMALLER: flag = ICON_SMALL; break;
    case NW_ICONSIZE_SMALL:   flag = ICON_SMALL; break;
    case NW_ICONSIZE_BIG:     flag = ICON_BIG; break;
    case NW_ICONSIZE_BIGGER:  flag = ICON_BIG; break;
  }
  SendMessage(handle_, WM_SETICON, flag, (LPARAM)MakeIcon(path));
}

const TCHAR* NativeWindow::GetTitle() {
  TCHAR title[80];
  GetWindowText(handle_, title, 80);
  return title;
}

void NativeWindow::SetTitle(const TCHAR* title) {
  SetWindowText(handle_, title);
}

void NativeWindow::Move(int left, int top, int width, int height) {
  UpdatePosition(left, top, width, height);
  SetWindowPos(handle_, NULL, left, top, width, height, NULL);
}

void NativeWindow::Move(int left, int top) {
  rect_.left = left;
  rect_.top = top;
  SetWindowPos(handle_, NULL, left, top, NULL, NULL, SWP_NOSIZE);
}

void NativeWindow::Resize(int width, int height) {
  rect_.width = width;
  rect_.height = height;
  SetWindowPos(handle_, NULL, NULL, NULL, width, height, SWP_NOMOVE);
}

void NativeWindow::UpdatePosition(){
  RECT rect;
  GetWindowRect(handle_, &rect);
  rect_.left = rect.left;
  rect_.top = rect.top;
  rect_.width = rect.right - rect.left;
  rect_.height = rect.bottom - rect.top;
}

void NativeWindow::SetTopmost(bool ontop){
  long current = GetWindowLong(handle_, GWL_EXSTYLE);
  NewStyle(handle_, GWL_EXSTYLE, ontop ? current | WS_EX_TOPMOST : current & ~WS_EX_TOPMOST);
  SetWindowPos(handle_, ontop ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
  topmost_ = ontop;
}


NW_STATE NativeWindow::GetState(){
  long style = GetWindowLongPtr(handle_, GWL_STYLE);
  if (fullscreen_) {
    return NW_STATE_FULLSCREEN;
  } else if (style & WS_MAXIMIZE) {
    return NW_STATE_MAXIMIZED;
  } else if (style & WS_MINIMIZE) {
    return NW_STATE_MINIMIZED;
  } else {
    return NW_STATE_NORMAL;
  }
}


void NativeWindow::SetResizable(bool resizable) {
  resizable_ = resizable;
  long current = GetWindowLongPtr(handle_, GWL_STYLE);
  NewStyle(handle_, GWL_STYLE, resizable
    ? current | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX
    : current & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX)
  );
}

bool NativeWindow::GetResizable() {
  return (GetWindowLongPtr(handle_, GWL_STYLE) & WS_SIZEBOX) > 0;
}

void NativeWindow::SetShowChrome(bool showChrome) {
  show_chrome_ = showChrome;
  long current = GetStyle();
  NewStyle(handle_, GWL_STYLE, showChrome ? current | WS_CAPTION | WS_SIZEBOX : current & ~(WS_CAPTION | WS_SIZEBOX));
}

bool NativeWindow::GetShowChrome() {
  long current = GetStyle();
  return (current & WS_CAPTION) || (current & WS_SIZEBOX);
}

void NativeWindow::SetAlpha(bool alpha) {
  alpha_ = alpha;
  long current = GetStyle(TRUE);
  SetStyle( alpha ? current | WS_EX_LAYERED | WS_EX_COMPOSITED : current & ~(WS_EX_LAYERED | WS_EX_COMPOSITED),TRUE);
  BlurBehind(handle_, alpha);
  SetNCWidth(handle_, alpha ? -1 : 0);
}

bool NativeWindow::GetAlpha() {
  return alpha_;
}

long NativeWindow::GetStyle(bool extended) {
  return GetWindowLong(handle_, extended ? GWL_EXSTYLE : GWL_STYLE);
}

void NativeWindow::SetStyle(long style, bool extended) {
  NewStyle(handle_, extended ? GWL_EXSTYLE : GWL_STYLE, style);
}

void NativeWindow::SetOpacity(double opacity) {
  opacity_ = opacity;
  UpdateStyle(handle_, GWL_EXSTYLE, opacity < 1 ? WS_EX_LAYERED : ~WS_EX_LAYERED);
  SetLayeredWindowAttributes(handle_, NULL, (char)(opacity * 255), LWA_ALPHA);
}

double NativeWindow::GetOpacity() {
  return opacity_;
}

ATOM MyRegisterClass(HINSTANCE hInst) {
  WNDCLASSEX wcex = {0};
  wcex.cbSize        = sizeof(WNDCLASSEX);
  wcex.style         = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc   = WndProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = hInst;
  wcex.hIcon         = bigIcon;
  wcex.hIconSm       = smallIcon;
  wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wcex.lpszMenuName  = NULL;
  wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wcex.lpszClassName = szWindowClass;
  return RegisterClassEx(&wcex);
}

// Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  NativeWindow* window = NativeWindow::GetWindow(hwnd);
  CefRefPtr<CefBrowser> browser;
  if (window) {
    browser = window->GetBrowser();
  }

  switch (message) {
    case WM_CREATE: {
      RECT rect;
      GetClientRect(hwnd, &rect);
      AddWebView(hwnd, rect, url_, browserSettings);
      return 0;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc;
      hdc = BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_MENUCOMMAND: {
      HMENU menu = (HMENU)lParam;
      MENUITEMINFO menuItem;
      menuItem.cbSize = sizeof(MENUITEMINFO);
      menuItem.fMask = MIIM_DATA;
      int idx = wParam;
      GetMenuItemInfo(menu,idx,TRUE,&menuItem);
      appjs_action_callback* actionCallback = (appjs_action_callback*) menuItem.dwItemData;
      Persistent<Object> action = actionCallback->action;
      NativeMenu* nativeMenu = actionCallback->menu;

      if(action->IsCallable()) {
        const int argc = 1;
        Handle<Value> argv[argc] = {actionCallback->item};
        action->CallAsFunction(nativeMenu->GetV8Handle(),argc,argv);
      }

      nativeMenu->Emit("select",Local<Object>::New(actionCallback->item));
      return 0;
    }
    case WM_SETFOCUS:
      if (browser.get()) {
        PostMessage(browser->GetWindowHandle(), WM_SETFOCUS, wParam, NULL);
        return 0;
      }
    case WM_SIZE: {
      window->UpdatePosition();
      if (browser.get()) {
        RECT rect;
        GetClientRect(hwnd, &rect);
        HDWP hdwp = BeginDeferWindowPos(1);
        hdwp = DeferWindowPos(hdwp, browser->GetWindowHandle(), NULL,
                              rect.left, rect.top, rect.right - rect.left,
                              rect.bottom - rect.top, SWP_NOZORDER);
        EndDeferWindowPos(hdwp);
        if (emitFullscreen) {
          emitFullscreen = false;
          window->Emit("fullscreen");
        } else {
          window->Emit("resize", (int)LOWORD(lParam), (int)HIWORD(lParam));
        }
      }
      break;
    }
    case WM_WINDOWPOSCHANGING: {
      WINDOWPOS *position;
      position = (WINDOWPOS*)lParam;
      if (position->flags & SWP_STATECHANGED) {
        if (IsIconic(window->handle_)) {
          window->Emit("minimize");
        } else if (IsZoomed(window->handle_)) {
          window->Emit("maximize");
        } else {
          window->Emit("restore");
        }
      }
      break;
    }
    case WM_MOVE:
      window->UpdatePosition();
      if (browser.get()) {
        window->Emit("move", (int)LOWORD(lParam), (int)HIWORD(lParam));
      }
      break;
    case WM_NCHITTEST: {
      LRESULT result;
      if (DwmDefWindowProc != NULL) {
        if (DwmDefWindowProc(hwnd, message, wParam, lParam, &result)) {
          return result;
        }
      }
      break;
    }
    case WM_ERASEBKGND:
      if (browser.get()) {
        return 0;
      }
      break;
    case WM_CLOSE:
      if (browser.get()) {
        browser->ParentWindowWillClose();
      }
      break;
    //case WM_DESTROY:
    //  PostQuitMessage(0);

  }

  return DefWindowProc(hwnd, message, wParam, lParam);
}



int CALLBACK DirectorySelectHook(HWND hwnd, UINT msg, LPARAM lParam, LPARAM data) {
  switch (msg) {
    case BFFM_INITIALIZED:
      HWND parent = GetParent(hwnd);
      RECT r;
      GetWindowRect(parent, &r);
      r.right = r.right - r.left + 200;
      r.bottom = r.bottom - r.top + 200;
      r.left = (NativeWindow::ScreenWidth() - r.right) / 2 | 0;
      r.top = (NativeWindow::ScreenHeight() - r.bottom) / 2 | 0;
      SetWindowPos(parent, NULL, r.left, r.top, r.right, r.bottom, SWP_NOZORDER);
      SendMessage(hwnd, BFFM_SETSELECTION, true, data);
      break;
  }
  return 0;
}

void NativeWindow::SetMenuBar(NativeMenu* nativeMenu) {
  HMENU menu;
  nativeMenu->Attach(menu);
  SetMenu(handle_,menu);
}

void NativeWindow::OpenFileDialog(uv_work_t* req) {
  AppjsDialogSettings* settings = (AppjsDialogSettings*)req->data;
  tstring           acceptTypes = settings->reserveString1;
  bool              multiSelect = settings->reserveBool1;
  bool                dirSelect = settings->reserveBool2;

  settings->result = NULL;
  TCHAR filename[MAX_PATH*10];
  ZeroMemory(&filename, sizeof(filename));

  if (dirSelect) {
    LPMALLOC pMalloc = NULL;
    SHGetMalloc(&pMalloc);

    BROWSEINFO bi;
    bi.hwndOwner = NULL;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = filename;
    bi.lpszTitle = settings->title.c_str();
    bi.ulFlags = BIF_USENEWUI | BIF_BROWSEFILEJUNCTIONS | BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS;
    bi.lParam = (LPARAM)settings->initialValue.c_str();
    bi.iImage = -1;
    bi.lpfn = DirectorySelectHook;

    LPITEMIDLIST item;
    if (item = SHBrowseForFolder(&bi)) {
      TCHAR dir[MAX_PATH];
      if (SHGetPathFromIDList(item, dir)) {
        std::vector<TCHAR*> paths;
        paths.push_back(dir);
        settings->result = &paths;
      }
      pMalloc->Free(item);
    }
  } else {
    std::replace(acceptTypes.begin(), acceptTypes.end(), ':', '\0');
    std::replace(acceptTypes.begin(), acceptTypes.end(), ',', '\0');
    acceptTypes += L'\0';

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.hInstance = 0;
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrTitle = settings->title.c_str();
    ofn.Flags = OFN_NOCHANGEDIR | OFN_FORCESHOWHIDDEN;
    ofn.lpstrFilter = acceptTypes.c_str();
    ofn.nMaxFile = MAX_PATH;
    if (PathIsDirectory(settings->initialValue.c_str())) {
      ofn.lpstrInitialDir = settings->initialValue.c_str();
      ofn.lpstrFile = filename;
    } else {
      wcscpy(filename, settings->initialValue.c_str());
      ofn.lpstrFile = filename;
    }

    BOOL result;
    if (settings->type == NW_DIALOGTYPE_FILE_SAVE) {
      multiSelect = false;
      result = GetSaveFileName(&ofn);
    } else if (settings->type == NW_DIALOGTYPE_FILE_OPEN) {
      if (multiSelect) {
        ofn.Flags |= OFN_ALLOWMULTISELECT | OFN_EXPLORER;
      }
      result = GetOpenFileName(&ofn);
    }

    if (result) {
      settings->result = filename;
    }
  }

}

void NativeWindow::ProcessFileDialog(uv_work_t* req) {
  AppjsDialogSettings* settings = (AppjsDialogSettings*)req->data;
  Persistent<Function> cb = settings->cb;
  void* result = settings->result;

  if (result != NULL) {

    std::vector<TCHAR*> filenames;
    TCHAR* offset = (TCHAR*)result;

    do {
      filenames.push_back(offset);
      offset += wcslen(offset) + 1;
    } while (offset[0] != '\0');

    std::vector<TCHAR*>::iterator file = filenames.begin();
    Handle<String> base = String::New((uint16_t*)*file);
    Handle<Value> error = Undefined();
    Local<Array> files;

    if (filenames.size() == 1) {
      files = Array::New(1);
      files->Set(0, base);
    } else {
      files = Array::New(filenames.size() - 1);
      base = String::Concat(base, String::New("\\"));
      int index = 0;

      for (file++; file != filenames.end(); ++file) {
        files->Set(index, String::Concat(base, String::New((uint16_t*)*file)));
        index++;
      }
    }

    Local<Value> argv[2] = { Local<Value>::New(error), Local<Value>::New(files) };
    cb->Call(settings->me->GetV8Handle(), 2, argv);
  } else {
    Local<Value> argv[1] = { Local<Value>::New(String::New("canceled")) };
    cb->Call(settings->me->GetV8Handle(), 1, argv);
  }

  cb.Dispose();
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


} /* appjs */
