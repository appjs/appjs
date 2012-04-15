#include <node.h>
#include <gtk/gtk.h>
#include "linux/cef.h"
#include "linux/mainwindow.h"

namespace appjs {

MainWindow::MainWindow () {

  Cef::Init();

  GtkWidget* vbox = gtk_vbox_new(FALSE, 0);
  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

  Cef::AddWebView(vbox);

  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(gtk_widget_destroyed), &window);
//  g_signal_connect(G_OBJECT(window), "destroy",
//                   G_CALLBACK(Stop), NULL);

  gtk_container_add(GTK_CONTAINER(window), vbox);
  gtk_widget_show_all(GTK_WIDGET(window));

  // Install an signal handler so we clean up after ourselves.
  signal(SIGINT, Stop);
  signal(SIGTERM, Stop);

};

void MainWindow::Show () {
  Cef::Run();
};


void MainWindow::Stop () {
  Cef::Shutdown();
};

void MainWindow::Stop (int status) {
  Cef::Shutdown();
};

} /* appjs */

