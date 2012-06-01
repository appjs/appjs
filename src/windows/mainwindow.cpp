#include <node.h>
#include <windows.h>
#include "appjs.h"
#include "windows/mainwindow.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/cef_handler.h"

#define MAX_LOADSTRING 100

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

TCHAR szWindowClass[MAX_LOADSTRING];
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass(HINSTANCE hInstance);
char* browserUrl;
Settings* browserSettings;

MainWindow::MainWindow (char* url, Settings* settings) {
  Cef::Init();

  //TODO Take settings into account.
  int width = settings->getNumber("width",800);
  int height = settings->getNumber("height",600);
  double opacity = settings->getNumber("opacity",1);
  bool show_chrome = settings->getBoolean("showChrome",true);
  bool resizable = settings->getBoolean("resizable",true);
  bool show_resize_grip = settings->getBoolean("showResizeGrip",false);
  bool auto_resize = settings->getBoolean("autoResize",true);
  bool fullscreen = settings->getBoolean("fullscreen",false);

  HINSTANCE hInstance = GetModuleHandle(NULL);
  strcpy(szWindowClass,"MainWindowClass");
  browserUrl = url;
  browserSettings = settings;
  
  if(!MyRegisterClass(hInstance)){
	  //TODO send error to node
	  fprintf(stderr,"Error occurred: ");
	  fprintf(stderr,"%d\n",GetLastError());
  };

  // Perform application initialization
  HWND hWnd = CreateWindowEx(NULL, szWindowClass,"",
                      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT,CW_USEDEFAULT,width,
                      height, NULL, NULL, hInstance, NULL);

  if (!hWnd) {
	  //TODO send error to node
	  fprintf(stderr,"Error occurred: ");
	  fprintf(stderr,"%d\n",GetLastError());
	  return;
  }

  UpdateWindow(hWnd);

  Cef::Run();
};

void MainWindow::show() {
  if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");

  ShowWindow(g_handler->GetMainHwnd(), SW_SHOW);
};

void MainWindow::hide() {
  if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");
  
  ShowWindow(g_handler->GetMainHwnd(), SW_HIDE);
};

void MainWindow::destroy() {
 if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");
  
  g_handler->GetBrowser()->CloseBrowser();
};

// Register Class
ATOM MyRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style         = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc   = WndProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = hInstance;
  wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION );
  wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION );
  wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName  = NULL;
  wcex.lpszClassName = szWindowClass;

  return RegisterClassEx(&wcex);
}

// Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                         LPARAM lParam) {
  int wmId, wmEvent;
  PAINTSTRUCT ps;
  HDC hdc;
  // Callback for the main window
  switch (message) {
    case WM_CREATE: {
      // Create the single static handler class instance
      g_handler = new ClientHandler();
      g_handler->SetMainHwnd(hWnd);

      // Create the child windows used for navigation
	  RECT rect;
	  int x = 0;

	  GetClientRect(hWnd, &rect);

      // Initialize window info to the defaults for a child window
	  Cef::AddWebView(hWnd, rect,browserUrl,browserSettings);

      return 0;
    }

    case WM_COMMAND: {
      CefRefPtr<CefBrowser> browser;
      if (g_handler.get())
        browser = g_handler->GetBrowser();

      wmId    = LOWORD(wParam);
      wmEvent = HIWORD(wParam);

      break;
    }

    case WM_PAINT:
      hdc = BeginPaint(hWnd, &ps);
      EndPaint(hWnd, &ps);
      return 0;

    case WM_SETFOCUS:
      if (g_handler.get() && g_handler->GetBrowserHwnd()) {
        // Pass focus to the browser window
        PostMessage(g_handler->GetBrowserHwnd(), WM_SETFOCUS, wParam, NULL);
      }
      return 0;

    case WM_SIZE:
      if (g_handler.get() && g_handler->GetBrowserHwnd()) {
        // Resize the browser window and address bar to match the new frame
        // window size
        RECT rect;
        GetClientRect(hWnd, &rect);

        HDWP hdwp = BeginDeferWindowPos(1);
        hdwp = DeferWindowPos(hdwp, g_handler->GetBrowserHwnd(), NULL,
          rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
          SWP_NOZORDER);
        EndDeferWindowPos(hdwp);
      }
      break;

    case WM_ERASEBKGND:
      if (g_handler.get() && g_handler->GetBrowserHwnd()) {
        // Dont erase the background if the browser window has been loaded
        // (this avoids flashing)
        return 0;
      }
      break;

    case WM_CLOSE:
      if (g_handler.get()) {
        CefRefPtr<CefBrowser> browser = g_handler->GetBrowser();
        if (browser.get()) {
          // Let the browser window know we are about to destroy it.
          browser->ParentWindowWillClose();
        }
      }
      break;

    case WM_DESTROY:
      // The frame window has exited
      PostQuitMessage(0);
      return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
  }

} /* appjs */
