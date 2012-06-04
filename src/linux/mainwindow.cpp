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

  // Set default icon list
  if( !g_handler->GetBrowserHwnd() ) {
    Settings icons(settings->getObject("icons",Object::New()));
    
    char* smallerIconPath = icons.getString("smaller","");
    char* smallIconPath = icons.getString("small","");
    char* bigIconPath = icons.getString("big","");
    char* biggerIconPath = icons.getString("bigger","");

    GdkPixbuf* smallerIconBuf = gdk_pixbuf_new_from_file(smallerIconPath,NULL);
    GdkPixbuf* smallIconBuf = gdk_pixbuf_new_from_file(smallIconPath,NULL);
    GdkPixbuf* bigIconBuf = gdk_pixbuf_new_from_file(bigIconPath,NULL);
    GdkPixbuf* biggerIconBuf = gdk_pixbuf_new_from_file(biggerIconPath,NULL);

    GList* iconList;

    iconList = g_list_insert(iconList,smallerIconBuf,0);
    iconList = g_list_insert(iconList,smallIconBuf,1);
    iconList = g_list_insert(iconList,bigIconBuf,2);
    iconList = g_list_insert(iconList,biggerIconBuf,3);

    gtk_window_set_default_icon_list(iconList);

    delete smallerIconPath;
    delete smallIconPath;
    delete bigIconPath;
    delete biggerIconPath;
  }

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

  GtkWidget* box = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), box);
  this->window = window;
  Cef::AddWebView(box,url,settings);

  Cef::Run();
};

void MainWindow::show() {
  if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");

  gtk_widget_show_all(GTK_WIDGET(window));
};

void MainWindow::hide() {
  if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");
  
  gtk_widget_hide(GTK_WIDGET(window));
};

void MainWindow::destroy() {
 if (!g_handler.get() || !g_handler->GetBrowserHwnd())
    NODE_ERROR("Browser window not available or not ready.");
  
  gtk_widget_destroy(GTK_WIDGET(window));

};

} /* appjs */

