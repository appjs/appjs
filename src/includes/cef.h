#ifndef APPJS_CEF_H
#define APPJS_CEF_H
#pragma once

#if defined(__LINUX__)
#include "cef_base_gtk.h"
#elif defined(__WIN__)
#include "cef_base_win.h"
#endif

#include "cef_loop.h"

namespace appjs {

class Cef 
  : public CefBase,
    public CefLoop
{
  
public:
  static void Init();
  
protected:
  Cef (const Cef&);
  Cef& operator= (const Cef&);
  static bool initialized_;

};

}

#endif /* end of APPJS_CEF_H */
