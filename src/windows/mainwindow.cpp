#include <node.h>
#include <windows.h>
#include "includes/cef.h"
#include "windows/mainwindow.h"

namespace appjs {

using namespace v8;

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

