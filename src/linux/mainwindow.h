#ifndef APPJS_MAIN_WINDOW_H
#define APPJS_MAIN_WINDOW_H
#pragma once

#include <node.h>
#include "includes/util.h"

namespace appjs {

class MainWindow {

public:
  MainWindow(Settings*);
  ~MainWindow();
  void show();
  void hide();
  void destroy();
};

} /* appjs */
#endif /* end of APPJS_MAIN_WINDOW_H */
