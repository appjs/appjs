#include <node.h>
#include <windows.h>
#include <algorithm>
#define min(left,right) std::min(left,right)
#define max(left,right) std::max(left,right)
#include <gdiplus.h>
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

  DWORD commonStyle = WS_CLIPCHILDREN;
  DWORD style;

  // set resizable
  if( !resizable ) {
    style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
  } else {
    style = WS_OVERLAPPEDWINDOW;
  }

  // set chrome
  if( show_chrome ) {
    commonStyle |= style;
  } else {
    commonStyle |= WS_POPUP;
  }

  if( x < 0 || y < 0 ) {
    x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
  }
  // Perform application initialization
  browser_ = NULL;
  handle_ = CreateWindowEx(NULL, szWindowClass,"", commonStyle, x, y, width, height, NULL, NULL, hInstance, NULL);

  if (!handle_) {
	  //TODO send error to node
	  fprintf(stderr,"Error occurred: ");
	  fprintf(stderr,"%d\n",GetLastError());
	  return;
  }

  //set window opacity
  SetWindowLong(handle_, GWL_EXSTYLE, GetWindowLong(handle_, GWL_EXSTYLE) | WS_EX_LAYERED);
  SetWindowLongPtr(handle_,GWLP_USERDATA, (LONG)this);
  SetLayeredWindowAttributes(handle_, 0, 255 * opacity, LWA_ALPHA);

  UpdateWindow(handle_);

  Cef::Run();
};


int NativeWindow::ScreenWidth() {
  return GetSystemMetrics(SM_CXSCREEN);
}

int NativeWindow::ScreenHeight() {
  return GetSystemMetrics(SM_CYSCREEN);
}

void NativeWindow::Show() {
  if (browser_) {
    ShowWindow(handle_, SW_SHOW);
  }
}

void NativeWindow::Hide() {
  if (browser_) {
    ShowWindow(handle_, SW_HIDE);
  }
}

void NativeWindow::Destroy() {
  if (browser_) {
    CloseWindow(handle_);
  }
}

// Register Class
ATOM MyRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style         = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc   = WndProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = hInstance;
  wcex.hIcon         = bigIcon;
  wcex.hIconSm       = smallIcon;
  wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName  = NULL;
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
        // Resize the browser window to match the new frame
        // window size

        //TODO this code only resizes main browser. What if we have multiple browsers?
        /*RECT rect;
        GetClientRect(handle_, &rect);

        HDWP hdwp = BeginDeferWindowPos(1);
        hdwp = DeferWindowPos(hdwp, g_handler->GetBrowserHwnd(), NULL,
          rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
          SWP_NOZORDER);
        EndDeferWindowPos(hdwp);*/
      }
      break;
    }
    case WM_ERASEBKGND: {
      NativeWindow* window = ClientHandler::GetWindow(hwnd);
      if (window->GetBrowser()) {
        // Dont erase the background if the browser window has been loaded
        // (this avoids flashing)
        return 0;
      }
      break;
    }
    case WM_CLOSE: {
      NativeWindow* window = ClientHandler::GetWindow(hwnd);
      if (window->GetBrowser()) {
        window->GetBrowser()->ParentWindowWillClose();
      }
      break;
    }
      /*case WM_DESTROY:
        //PostQuitMessage(0);
      return 0;*/
  }

  return DefWindowProc(hwnd, message, wParam, lParam);
}

} /* appjs */
