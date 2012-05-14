#include <node.h>
#include <gtk/gtk.h>
#include "appjs.h"
#include "linux/mainwindow.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/cef_handler.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

static void destroy_handler (int status = 0) {
  g_handler->GetBrowser()->CloseBrowser();
};

MainWindow::MainWindow (char* url, Settings* settings) {

  Cef::Init();

  int width = settings->getNumber("width",800);
  int height = settings->getNumber("height",600);
  double opacity = settings->getNumber("opacity",1);
  bool show_chrome = settings->getBoolean("showChrome",true);
  bool resizable = settings->getBoolean("resizable",true);
  bool show_resize_grip = settings->getBoolean("showResizeGrip",false);
  bool auto_resize = settings->getBoolean("autoResize",true);
  bool fullscreen = settings->getBoolean("fullscreen",false);

  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_window_set_default_size(GTK_WINDOW(window), width, height);
  gtk_window_set_resizable(GTK_WINDOW(window), resizable);
  gtk_window_set_opacity(GTK_WINDOW(window), opacity);
  gtk_window_set_decorated(GTK_WINDOW(window), show_chrome);

  #if defined(__UBUNTU__) && !GTK_CHECK_VERSION(2, 24, 10)
    if(gtk_check_version(2, 24, 10))
      gtk_window_set_has_resize_grip(GTK_WINDOW(window), show_resize_grip);
  #endif
  
  if( fullscreen ) {
    gtk_window_fullscreen(GTK_WINDOW(window));
  }
  
  if( !resizable ) {
    gtk_widget_set_size_request(window,width,height);
  }

  g_handler->SetAutoResize(auto_resize);

  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(&destroy_handler), NULL);

  GtkWidget* box = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), box);

  Cef::AddWebView(box,url);

  // Install an signal handler so we clean up after ourselves.
  signal(SIGINT, destroy_handler);
  signal(SIGTERM, destroy_handler);
  Cef::Run();
};

void MainWindow::show() {
  if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");

  gtk_widget_show_all(GTK_WIDGET(g_handler->GetMainHwnd()));
};

void MainWindow::hide() {
  if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");
  
  gtk_widget_hide(GTK_WIDGET(g_handler->GetMainHwnd()));
};

void MainWindow::destroy() {
 if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");
  
  g_handler->GetBrowser()->CloseBrowser();
};

} /* appjs */

