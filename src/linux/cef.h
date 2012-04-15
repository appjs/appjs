#ifndef APPJS_CEF_H
#define APPJS_CEF_H
#pragma once


#include "include/cef_browser.h"
#include "cef/cefbase.h"

namespace appjs {

class Cef : public CefBase {

public:
  static void Init();
  static void AddWebView(CefWindowHandle ParentWidget);

private:
  Cef(const Cef&);
  Cef& operator= (const Cef&);
  static bool initialized_;

};

} /* appjs */

#endif /* end of APPJS_CEF_H */

