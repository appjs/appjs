#include "include/cef_app.h"
#include "cef/impl_cefloop.h"
#include <iostream>

namespace appjs {

uv_timer_t CefLoop::timer;

void CefLoop::Init() {
  uv_timer_init(uv_default_loop(),&timer);
}

void CefLoop::Run() {
  uv_ref(uv_default_loop());
  uv_timer_start(&timer,Start,1,1);
}

void CefLoop::Pause() {
  uv_timer_stop(&timer);
}

void CefLoop::Shutdown() {
  uv_close((uv_handle_t*)&timer, Stop);
}

void CefLoop::Stop(uv_handle_t* handle) {
   CefShutdown();
   uv_unref(uv_default_loop());
}

void CefLoop::Start(uv_timer_t* handle, int status) {
  CefDoMessageLoopWork();  
}

} /* appjs */
