#include "appjs.h"
#include "include/cef_app.h"
#include "cef_loop.h"
#include <iostream>

namespace appjs {

uv_timer_t CefLoop::timer;

bool CefLoop::initialized_ = false;
bool CefLoop::running_ = false;

void CefLoop::Init() {

  if( CefLoop::initialized_ ) return;

  uv_timer_init(uv_default_loop(),&timer);

  CefLoop::initialized_ = true;
}

void CefLoop::Run() {

  if( !CefLoop::initialized_ || CefLoop::running_ ) return;

  uv_ref(uv_default_loop());
  uv_timer_start(&timer,RunLoop,1,1);

  CefLoop::running_ = true;
}

void CefLoop::Pause() {

  if( !CefLoop::initialized_ || !CefLoop::running_ ) return;

  uv_timer_stop(&timer);
  uv_unref(uv_default_loop());
  
  CefLoop::running_ = false;
}

void CefLoop::Shutdown() {

  if( !CefLoop::initialized_ ) return;


  CefLoop::initialized_ = false;
  CefLoop::running_ = false;

  uv_close((uv_handle_t*)&timer, StopLoop);

}

void CefLoop::StopLoop(uv_handle_t* handle) {
  CefShutdown();
  uv_unref(uv_default_loop());
}

void CefLoop::RunLoop(uv_timer_t* handle, int status) {
  CefDoMessageLoopWork();  
}

} /* appjs */
