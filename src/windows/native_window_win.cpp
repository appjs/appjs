#include <node.h>
#include <windows.h>
#include <algorithm>
#define min(left,right) std::min(left,right)
#define max(left,right) std::max(left,right)
#include <gdiplus.h>
#include <dwmapi.h>
#include "appjs.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/cef_handler.h"
#include "base/native_window.h"

#define MAX_LOADSTRING 100

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

TCHAR szWindowClass[MAX_LOADSTRING];
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass(HINSTANCE hInstance);
HICON smallIcon;
HICON bigIcon;
Settings* browserSettings;
char* url_;


// #################################
// ### Windows Utility Functions ###
// #################################

void UpdateStyle(HWND hwnd, int index, LONG value){
  SetWindowLongPtr(hwnd, index, value);
  SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

void BlurBehind(HWND hwnd, bool enable){
  DWM_BLURBEHIND bb = {0};
  bb.fEnable = enable;
  bb.hRgnBlur = NULL;
  bb.dwFlags = DWM_BB_ENABLE;
  DwmEnableBlurBehindWindow(hwnd, &bb);
}

void SetNCWidth(HWND hwnd, int left, int right, int top, int bottom){
  MARGINS margins = {left, right, top, bottom};
  DwmExtendFrameIntoClientArea(hwnd, &margins);
}

void SetNCWidth(HWND hwnd, int size){
  MARGINS margins = {size, size, size, size};
  DwmExtendFrameIntoClientArea(hwnd, &margins);
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

// ############################
// ### NativeWindow methods ###
// ############################

void NativeWindow::Init(char* url, Settings* settings) {
  url_ = url;

  if( !g_handler->GetBrowserHwnd() ) {

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WCHAR* wSmallIconPath = icons->getString("small",L"");
    WCHAR* wBigIconPath = icons->getString("big",L"");

    Gdiplus::Bitmap* smallIconBitmap = Gdiplus::Bitmap::FromFile(wSmallIconPath);
    Gdiplus::Bitmap* bigIconBitmap = Gdiplus::Bitmap::FromFile(wBigIconPath);

    if( smallIconBitmap->GetWidth() ) {
      smallIconBitmap->GetHICON(&smallIcon);
      delete[] wSmallIconPath;
      delete smallIconBitmap;
    }

    if( bigIconBitmap->GetWidth() ) {
      bigIconBitmap->GetHICON(&bigIcon);
      delete[] wBigIconPath;
      delete bigIconBitmap;
    }
  }

  HINSTANCE hInstance = GetModuleHandle(NULL);
  strcpy(szWindowClass,"AppjsWindow");
  browserSettings = settings;

  MyRegisterClass(hInstance);

  if (rect_.left < 0 || rect_.top < 0) {
    rect_.left = (GetSystemMetrics(SM_CXSCREEN) - rect_.width) / 2;
    rect_.top = (GetSystemMetrics(SM_CYSCREEN) - rect_.height) / 2;
  }
  browser_ = NULL;
  handle_ = CreateWindowEx(NULL, szWindowClass,"", WS_OVERLAPPEDWINDOW, rect_.top, rect_.left, rect_.width, rect_.height, NULL, NULL, hInstance, NULL);

  SetWindowLongPtr(handle_, GWLP_USERDATA, (LONG)this);

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

  Cef::Run();
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
    SetWindowLongPtr(handle_, GWL_STYLE, restoreStyle);
  } else {
    ShowWindow(handle_, SW_RESTORE);
  }
}

void SendKeyEvent(WORD key, DWORD flag){
  INPUT input = { INPUT_KEYBOARD };
  input.ki.wVk = key;
  input.ki.wScan = MapVirtualKey(key, 0);
  input.ki.dwFlags = flag;
  SendInput(1, &input, sizeof(input));
}

void NativeWindow::Show() {
  ShowWindow(handle_, SW_NORMAL);
  SendKeyEvent(VK_MENU, 0);
  SetForegroundWindow(handle_);
  SendKeyEvent(VK_MENU, KEYEVENTF_KEYUP);
}

void NativeWindow::Hide() {
  ShowWindow(handle_, SW_HIDE);
}

void NativeWindow::Destroy() {
  CloseWindow(handle_);
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
    restoreStyle = GetWindowLong(handle_, GWL_STYLE);
    SetWindowLongPtr(handle_, GWL_STYLE, restoreStyle & ~(WS_CAPTION | WS_SIZEBOX));
    SetWindowPos(handle_, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    HDC hDC = GetWindowDC(NULL);
    SetWindowPos(handle_, NULL, 0, 0, GetDeviceCaps(hDC, HORZRES), GetDeviceCaps(hDC, VERTRES), SWP_FRAMECHANGED);
    UpdatePosition();
  }
}



const char* NativeWindow::GetTitle() {
  TCHAR title[80];
  GetWindowText(handle_, title, 80);
  return title;
}


void NativeWindow::Move(int top, int left, int width, int height) {
  UpdatePosition(top, left, width, height);
  SetWindowPos(handle_, NULL, left, top, width, height, NULL);
}

void NativeWindow::Move(int top, int left) {
  rect_.top = top;
  rect_.left = left;
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
  rect_.width = rect.right - rect.left;
  rect_.height = rect.bottom - rect.top;
  rect_.left = rect.left;
  rect_.top = rect.top;
}

void NativeWindow::SetTopmost(bool ontop){
  long current = GetWindowLong(handle_, GWL_EXSTYLE);
  UpdateStyle(handle_, GWL_EXSTYLE, ontop ? current | WS_EX_TOPMOST : current & ~WS_EX_TOPMOST);
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
  UpdateStyle(handle_, GWL_STYLE, resizable
    ? current | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX
    : current & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX)
  );
}

bool NativeWindow::GetResizable() {
  return GetWindowLongPtr(handle_, GWL_STYLE) & WS_SIZEBOX;
}

void NativeWindow::SetShowChrome(bool showChrome) {
  show_chrome_ = showChrome;
  long current = GetWindowLongPtr(handle_, GWL_STYLE);
  UpdateStyle(handle_, GWL_STYLE, showChrome ? current | WS_CAPTION | WS_SIZEBOX : current & ~(WS_CAPTION | WS_SIZEBOX));
}

bool NativeWindow::GetShowChrome() {
  long current = GetWindowLongPtr(handle_, GWL_STYLE);
  return (current & WS_CAPTION) || (current & WS_SIZEBOX);
}

void NativeWindow::SetAlpha(bool alpha) {
  alpha_ = alpha;
  SetNCWidth(handle_, alpha ? -1 : 0);
}

bool NativeWindow::GetAlpha() {
  return alpha_;
}

long NativeWindow::GetStyle(bool extended) {
  return GetWindowLong(handle_, extended ? GWL_EXSTYLE : GWL_STYLE);
}

void NativeWindow::SetStyle(long style, bool extended) {
  UpdateStyle(handle_, extended ? GWL_EXSTYLE : GWL_STYLE, style);
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
  wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wcex.lpszClassName = szWindowClass;
  return RegisterClassEx(&wcex);
}

// Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  NativeWindow* window = ClientHandler::GetWindow(hwnd);
  CefRefPtr<CefBrowser> browser;
  if (window) {
    browser = window->GetBrowser();
  }

  switch (message) {
    case WM_CREATE: {
      RECT rect;
      GetClientRect(hwnd, &rect);
      Cef::AddWebView(hwnd, rect, url_, browserSettings);
      return 0;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc;
      hdc = BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
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
        if (wParam & SIZE_MAXIMIZED) {
          window->Emit("maximize");
        } else if (wParam & SIZE_MINIMIZED) {
          window->Emit("minimize");
        } else if (wParam & SIZE_RESTORED) {
          window->Emit("restore");
        } else if (!(wParam & SIZE_MAXHIDE || wParam & SIZE_MAXSHOW)) {
          window->Emit("resize", (int)LOWORD(lParam), (int)HIWORD(lParam));
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
    // case WM_ERASEBKGND:
    //   return 1;
    case WM_ERASEBKGND:
      if (browser.get()) {
        return 0;
      }
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

} /* appjs */
