#include <node.h>
#include <windows.h>
#include "windows/cef.h"
#include "windows/mainwindow.h"
#include "appjs_util.h"

namespace appjs {

using namespace v8;

static void destroy_handler (int status = 0) {
  Cef::Shutdown();
};


MainWindow::MainWindow (v8::Local<v8::Object> settings) {

  Cef::Init();

  

};

void MainWindow::Show () {
  Cef::Run();
};

void MainWindow::Stop () {
  Cef::Shutdown();
};

} /* appjs */

