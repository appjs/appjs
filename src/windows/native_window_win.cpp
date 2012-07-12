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


void UpdateStyle(HWND hwnd, int index, LONG value){
  SetWindowLong(hwnd, index, value);
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


  if(!MyRegisterClass(hInstance)){
    //TODO send error to node
    if( GetLastError() != 1410 ) { //1410: Class Already Registered
      fprintf(stderr,"Error occurred: ");
      fprintf(stderr,"%d\n",GetLastError());
      return;
    }
  };

  DWORD commonStyle = WS_OVERLAPPEDWINDOW;
  DWORD style;

  if( !resizable ) {
    style &= ~(WS_THICKFRAME);
  }
  // set chrome
  if( show_chrome ) {
    commonStyle |= style;
  } else {
    commonStyle |= WS_POPUP;
  }

  if( left_ < 0 || top_ < 0 ) {
    left_ = (GetSystemMetrics(SM_CXSCREEN) - width_) / 2;
    top_ = (GetSystemMetrics(SM_CYSCREEN) - height_) / 2;
  }
  // Perform application initialization
  browser_ = NULL;
  handle_ = CreateWindowEx(WS_EX_APPWINDOW, szWindowClass,"", WS_THICKFRAME, top_, left_, width_, height_, NULL, NULL, hInstance, NULL);

  if (!handle_) {
    //TODO send error to node
    fprintf(stderr,"Error occurred: ");
    fprintf(stderr,"%d\n",GetLastError());
    return;
  }

  SetWindowLongPtr(handle_,GWLP_USERDATA, (LONG)this);

  if (alpha) {
    SetNCWidth(handle_, -1);
  }

  UpdateWindow(handle_);

  Cef::Run();
};


int NativeWindow::ScreenWidth() {
  return GetSystemMetrics(SM_CXSCREEN);
}

int NativeWindow::ScreenHeight() {
  return GetSystemMetrics(SM_CYSCREEN);
}

void NativeWindow::Minimize() {
  ShowWindowAsync(handle_, SW_MINIMIZE);
}

void NativeWindow::Maximize() {
  ShowWindowAsync(handle_, SW_MAXIMIZE);
}

void NativeWindow::Restore() {
  ShowWindowAsync(handle_, SW_RESTORE);
}

void NativeWindow::Show() {
  ShowWindowAsync(handle_, SW_SHOW);
}

void NativeWindow::Hide() {
  ShowWindowAsync(handle_, SW_HIDE);
}

void NativeWindow::Destroy() {
  CloseWindow(handle_);
}

void NativeWindow::Drag() {
  ReleaseCapture();
  SendMessage(handle_, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}
void NativeWindow::SetPosition(int top, int left, int width, int height) {
  if (handle_) {
    UpdatePosition(top, left, width, height);
    SetWindowPos(handle_, NULL, top, left, width, height, NULL);
  }
}

void NativeWindow::SetPosition(int top, int left) {
  if (handle_) {
    top_ = top;
    left_ = left;
    SetWindowPos(handle_, NULL, top, left, NULL, NULL, SWP_NOSIZE);
  }
}

void NativeWindow::SetSize(int width, int height) {
  if (handle_) {
    width_ = width;
    height_ = height;
    SetWindowPos(handle_, NULL, NULL, NULL, width, height, SWP_NOMOVE);
  }
}

void NativeWindow::UpdatePosition(){
  RECT rect;
  GetClientRect(handle_, &rect);
  width_ = rect.right - rect.left;
  height_ = rect.bottom - rect.top;
  left_ = rect.left;
  top_ = rect.top;
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

  // Callback for the main window
  switch (message) {
    case WM_CREATE: {
      NativeWindow* window = ClientHandler::GetWindow(hwnd);
      RECT rect;
      GetClientRect(hwnd, &rect);

      // Initialize window info to the defaults for a child window
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
    /*case WM_SETFOCUS:
      if (g_handler.get() && g_handler->GetBrowserHwnd()) {
        // Pass focus to the browser window
        PostMessage(g_handler->GetBrowserHwnd(), WM_SETFOCUS, wParam, NULL);
      }
      return 0;*/

    case WM_SIZE: {
      NativeWindow* window = ClientHandler::GetWindow(hwnd);
      if (window->GetBrowser()) {
        RECT r;
        GetClientRect(hwnd, &r);
        HDWP hdwp = BeginDeferWindowPos(1);
        hdwp = DeferWindowPos(hdwp, window->GetBrowser()->GetWindowHandle(), NULL, r.left, r.top, r.right - r.left, r.bottom - r.top, SWP_NOZORDER);
        EndDeferWindowPos(hdwp);
      }
      break;
    }
    case WM_ERASEBKGND:
      return 1;
    // case WM_ERASEBKGND: {
    //   NativeWindow* window = ClientHandler::GetWindow(hwnd);
    //   if (window->GetBrowser()) {
    //     // Dont erase the background if the browser window has been loaded
    //     // (this avoids flashing)
    //     return 0;
    //   }
    case WM_CLOSE: {
      NativeWindow* window = ClientHandler::GetWindow(hwnd);
      if (window->GetBrowser()) {
        window->GetBrowser()->ParentWindowWillClose();
      }
    }
    break;
    //case WM_DESTROY:
    //  PostQuitMessage(0);

  }

  return DefWindowProc(hwnd, message, wParam, lParam);
}

} /* appjs */
