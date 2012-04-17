#include <node.h>
#include <node_buffer.h>
#include <gtk/gtk.h>
#include "linux/cef.h"
#include "linux/mainwindow.h"
#include "appjs_util.h"

namespace appjs {

using namespace v8;

static void destroy_handler (int status = 0) {
  Cef::Shutdown();
};


MainWindow::MainWindow (v8::Local<v8::Object> settings) {

  Cef::Init();

  GdkGeometry hints;
 
  int min_width = settings->Get(String::New("minWidth"))->NumberValue();
  int min_height = settings->Get(String::New("minHeight"))->NumberValue();
  int max_width = settings->Get(String::New("maxWidth"))->NumberValue();
  int max_height = settings->Get(String::New("maxHeight"))->NumberValue();
  int width = settings->Get(String::New("width"))->NumberValue();
  int height = settings->Get(String::New("height"))->NumberValue();
  char* title = appjs::StringToChar(settings->Get(String::New("title"))->ToString());
  bool has_chrome = settings->Get(String::New("hasChrome"))->BooleanValue();
  bool resizable = settings->Get(String::New("resizable"))->BooleanValue();
  bool scrollable = settings->Get(String::New("scrollable"))->BooleanValue();
  bool show_resize_grip = settings->Get(String::New("showResizeGrip"))->BooleanValue();
  bool fullscreen = settings->Get(String::New("fullscreen"))->BooleanValue();
  double opacity = settings->Get(String::New("opacity"))->NumberValue();


  hints.min_width = min_width;
  hints.min_height = min_height;
  hints.max_width = max_width;
  hints.max_height = max_height;

  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title(GTK_WINDOW(window), title);
  gtk_window_set_default_size(GTK_WINDOW(window), width, height);
  gtk_window_set_resizable(GTK_WINDOW(window), resizable);
  gtk_window_set_opacity(GTK_WINDOW(window), opacity);
  gtk_window_set_has_resize_grip(GTK_WINDOW(window), show_resize_grip);
  gtk_window_set_decorated(GTK_WINDOW(window), has_chrome);
  
  if( fullscreen ) {
    gtk_window_fullscreen(GTK_WINDOW(window));
  }
  
  if( !resizable ) {
    gtk_widget_set_size_request(window,width,height);
  }

  if(min_width > 0 && min_height > 0 ) {
    //gtk_widget_set_size_request(window, min_width, min_height);
//    GdkWindowHints mask = GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE;
//    gtk_window_set_geometry_hints(GTK_WINDOW(window),window,&hints,GDK_HINT_MIN_SIZE);
  }

  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(gtk_widget_destroyed), &window);
  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(&destroy_handler), NULL);

  GtkWidget* box = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), box);
  Cef::AddWebView(box);
  
  gtk_widget_show_all(GTK_WIDGET(window));

  // Install an signal handler so we clean up after ourselves.
  signal(SIGINT, destroy_handler);
  signal(SIGTERM, destroy_handler);

};

void MainWindow::Show () {
  Cef::Run();
};

void MainWindow::Stop () {
  Cef::Shutdown();
};

} /* appjs */

