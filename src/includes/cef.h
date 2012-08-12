#ifndef APPJS_CEF_H
#define APPJS_CEF_H
#pragma once

#include "util.h"
#include "cef_loop.h"

namespace appjs {

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
