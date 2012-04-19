#ifndef APPJS_MAIN_WINDOW_H
#define APPJS_MAIN_WINDOW_H
#pragma once

#include <node.h>

namespace appjs {

class MainWindow {

public:
  MainWindow(v8::Local<v8::Object> settings);
  ~MainWindow();
  void show();
  void hide();
  void stop();
};

} /* appjs */
#endif /* end of APPJS_MAIN_WINDOW_H */
