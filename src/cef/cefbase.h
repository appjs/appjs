#ifndef APPJS_CEF_CEFBASE_H
#define APPJS_CEF_CEFBASE_H
#pragma once

#include "cef/impl_cefloop.h"

namespace appjs {

class CefBase : public CefLoop {
  
public:
  static void Init();
  static void AddWebView();
  
protected:
  CefBase (const CefBase&);
  CefBase& operator= (const CefBase&);

};

}

#endif /* end of APPJS_CEF_CEFBASE_H */
