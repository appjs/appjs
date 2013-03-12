#include "include/cef_app.h"
#include "includes/client_app.h"
#include "includes/cef_scheme_handler.h"
#include "includes/cef_handler.h"
#include "includes/cef.h"

extern CefRefPtr<ClientHandler> g_handler;

#if defined(__WIN__)
DWMEFICA DwmExtendFrameIntoClientArea;
DWMEBBW DwmEnableBlurBehindWindow;
DWMWP DwmDefWindowProc;
HMODULE dwmapiDLL;
#endif


namespace appjs {

bool Cef::initialized_;

void Cef::Init(Settings* initOptions) {
  if (!Cef::initialized_) {
    CefLoop::Init(initOptions);

#if defined(__LINUX__)
    g_thread_init(NULL);
    gdk_threads_init();
    gtk_init(NULL,NULL);
#endif

#if defined(__WIN__)
    dwmapiDLL = LoadLibrary(TEXT("dwmapi.dll"));
    if (dwmapiDLL != NULL) {
      DwmExtendFrameIntoClientArea = (DWMEFICA)GetProcAddress(dwmapiDLL, "DwmExtendFrameIntoClientArea");
      DwmEnableBlurBehindWindow = (DWMEBBW)GetProcAddress(dwmapiDLL, "DwmEnableBlurBehindWindow");
      DwmDefWindowProc = (DWMWP)GetProcAddress(dwmapiDLL, "DwmDefWindowProc");
    }

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
#endif

    Cef::initialized_ = true;
  }
}

} /* appjs */
