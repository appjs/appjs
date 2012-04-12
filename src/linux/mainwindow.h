#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#pragma once

namespace appjs {

class MainWindow {

public:
  MainWindow();
  ~MainWindow();
  void Show();
  static void Stop();
  
private:
  static void Run(uv_timer_t* handle, int status);
};

} /* appjs */
#endif /* end of MAIN_WINDOW_H */

