#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#pragma once

namespace appjs {

class MainWindow {

public:
  MainWindow();
  ~MainWindow();
  void Show();
  void Stop();
  static void Stop(int);

};

} /* appjs */
#endif /* end of MAIN_WINDOW_H */

