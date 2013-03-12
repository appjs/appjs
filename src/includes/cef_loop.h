#ifndef APPJS_CEF_LOOP_H
#define APPJS_CEF_LOOP_H
#pragma once

#include <node.h>
#include "includes/util.h"

namespace appjs {

class CefLoop {

public:
  static uv_thread_t tid;
  static void Init(Settings*);
  static void Init(void* param);
  static void Run();
  static void Run(void*);
  static void Pause();
  static void Shutdown();

protected:
  CefLoop (const CefLoop&);
  CefLoop& operator= (const CefLoop&);
  static void StopLoop(uv_handle_t* handle);
  static void RunLoop(uv_timer_t* handle, int status);
  static bool running_;
  static bool initialized_;
  static Settings* options_;

};

} /* appjs */
#endif /* end of APPJS_CEF_LOOP_H */

