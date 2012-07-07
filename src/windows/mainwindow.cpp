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
#include "windows/mainwindow.h"

#define MAX_LOADSTRING 100

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

TCHAR szWindowClass[MAX_LOADSTRING];
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass(HINSTANCE hInstance);
char* browserUrl;
HICON smallIcon;
HICON bigIcon;
Settings* browserSettings;


MainWindow::MainWindow (char* url, Settings* settings) {

  //TODO Take settings into account.
  int width = settings->getNumber("width",800);
  int height = settings->getNumber("height",600);
  int x = settings->getNumber("x",-1);
  int y = settings->getNumber("y",-1);
  double opacity = settings->getNumber("opacity",1);
  bool show_chrome = settings->getBoolean("showChrome",true);
  bool resizable = settings->getBoolean("resizable",true);
  bool show_resize_grip = settings->getBoolean("showResizeGrip",false);
  bool auto_resize = settings->getBoolean("autoResize",false);
  bool fullscreen = settings->getBoolean("fullscreen",false);


  if( !g_handler->GetBrowserHwnd() ) {

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    Settings icons(settings->getObject("icons",Object::New()));
    WCHAR* wSmallIconPath = icons.getString("small",L"");
    WCHAR* wBigIconPath = icons.getString("big",L"");

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
  browserUrl = url;
  browserSettings = settings;
  g_handler->SetAutoResize(auto_resize);


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
  handle_ = CreateWindowEx(NULL, szWindowClass,"",
                           commonStyle, x, y, width,
                           height, NULL, NULL, hInstance, NULL);

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


void MainWindow::OpenDevTools(){
  if (browser_) {
    browser_->ShowDevTools();
  }
}

void MainWindow::CloseDevTools(){
  if (browser_) {
    browser_->CloseDevTools();
  }
}

void MainWindow::show() {
  ShowWindow(handle_, SW_SHOW);
};

void MainWindow::hide() {
  ShowWindow(handle_, SW_HIDE);
};


int MainWindow::ScreenWidth() {
  return GetSystemMetrics(SM_CXSCREEN);
}

int MainWindow::ScreenHeight() {
  return GetSystemMetrics(SM_CYSCREEN);
}

void MainWindow::destroy() {
 if (!handle_)
    NODE_ERROR("Browser window not available or not ready.");

  CloseWindow(handle_);
};

void MainWindow::setBrowser(CefRefPtr<CefBrowser> browser) {
  browser_ = browser;
}

CefRefPtr<CefBrowser> MainWindow::getBrowser() {
  return browser_;
}

void MainWindow::setV8Handle(Handle<Object> v8handle) {
  v8handle_ = v8handle;
}

Handle<Object> MainWindow::getV8Handle() {
  return v8handle_;
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
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam,
                         LPARAM lParam) {
  int wmId, wmEvent;
  PAINTSTRUCT ps;
  HDC hdc;
  // Callback for the main window
  switch (message) {
    case WM_CREATE: {
      // Create the child windows used for navigation
  	  RECT rect;
  	  int x = 0;

	  GetClientRect(hwnd, &rect);

      // Initialize window info to the defaults for a child window
	    Cef::AddWebView(hwnd, rect, browserUrl, browserSettings);

      return 0;
    }

    case WM_COMMAND: {
      wmId    = LOWORD(wParam);
      wmEvent = HIWORD(wParam);

      break;
    }

    case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
      return 0;

    /*case WM_SETFOCUS:
      if (g_handler.get() && g_handler->GetBrowserHwnd()) {
        // Pass focus to the browser window
        PostMessage(g_handler->GetBrowserHwnd(), WM_SETFOCUS, wParam, NULL);
      }
      return 0;*/

    case WM_SIZE: {
      MainWindow* win = ClientHandler::WindowFromHandle(hwnd);
      if (win->getBrowser()) {
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
      MainWindow* win = ClientHandler::WindowFromHandle(hwnd);
      if (win->getBrowser()) {
        // Dont erase the background if the browser window has been loaded
        // (this avoids flashing)
        return 0;
      }
      break;
    }
    /*case WM_CLOSE:
      if (g_handler.get()) {
        CefRefPtr<CefBrowser> browser = g_handler->GetBrowser();
        if (browser.get()) {
          // Let the browser window know we are about to destroy it.
          browser->ParentWindowWillClose();
        }
      }
      break;

      case WM_DESTROY:
        //PostQuitMessage(0);*/
      return 0;
    }


    return DefWindowProc(hwnd, message, wParam, lParam);
  }

} /* appjs */
