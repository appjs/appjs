#ifndef APPJS_CEF_H
#define APPJS_CEF_H
#pragma once

#if defined(__LINUX__)
#include "cef_base_gtk.h"
#elif defined(__MAC__)
#include "cef_base_mac.h"
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
  static void Init(char*);
  
protected:
  Cef (const Cef&);
  Cef& operator= (const Cef&);
  static bool initialized_;

};

} /* appjs */

#endif /* end of APPJS_CEF_H */
