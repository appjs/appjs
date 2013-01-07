#ifndef APPJS_CEF_H
#define APPJS_CEF_H
#pragma once

#include "util.h"
#include "cef_loop.h"

#ifdef __WIN__
#include <windows.h>
#include <algorithm>
#define min(left,right) std::min(left,right)
#define max(left,right) std::max(left,right)
#include <gdiplus.h>
#undef min
#undef max
#include <shlobj.h>
#include <shlwapi.h>

typedef struct _BLURBEHIND {
  DWORD dwFlags;
  BOOL  fEnable;
  HRGN  hRgnBlur;
  BOOL  fTransitionOnMaximized;
} BLURBEHIND, *PBLURBEHIND;

typedef struct _MARGINS {
  int cxLeftWidth;
  int cxRightWidth;
  int cyTopHeight;
  int cyBottomHeight;
} MARGINS, *PMARGINS;

typedef HRESULT (WINAPI *DWMEFICA)(HWND, MARGINS*);
typedef HRESULT (WINAPI *DWMEBBW)(HWND, BLURBEHIND*);
typedef BOOL (WINAPI *DWMWP)(HWND, UINT, WPARAM, LPARAM, LRESULT*);

extern DWMEFICA DwmExtendFrameIntoClientAreaL;
extern DWMEBBW DwmEnableBlurBehindWindow;
extern DWMWP DwmDefWindowProc;
extern HMODULE dwmapiDLL;

#endif

namespace appjs {

// ###################################################
// ### DWM functions that don't exist in WindowsXP ###
// ###################################################

class Cef : public CefLoop {

public:
  static void Init(Settings* initSettings);

protected:
  Cef (const Cef&);
  Cef& operator= (const Cef&);
  static bool initialized_;

};

} /* appjs */

#endif /* end of APPJS_CEF_H */
