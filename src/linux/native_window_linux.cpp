#include <node.h>
#include <gtk/gtk.h>
#include "appjs.h"
#include "base/native_window.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/cef_handler.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

void destroy_handler(GtkWidget* widget, NativeWindow* window) {
  Handle<Object> handle = window->GetV8Handle();
  Handle<Value> argv[1] = {String::New("close")};
  node::MakeCallback(handle,"emit",1,argv);
}

void NativeWindow::Init(char* url, Settings* settings) {

  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  // Set default icon list
  if( !g_handler->GetBrowserHwnd() ) {

    char* smallerIconPath = icons->getString("smaller","");
    char* smallIconPath = icons->getString("small","");
    char* bigIconPath = icons->getString("big","");
    char* biggerIconPath = icons->getString("bigger","");

    GdkPixbuf* smallerIconBuf = gdk_pixbuf_new_from_file(smallerIconPath,NULL);
    GdkPixbuf* smallIconBuf = gdk_pixbuf_new_from_file(smallIconPath,NULL);
    GdkPixbuf* bigIconBuf = gdk_pixbuf_new_from_file(bigIconPath,NULL);
    GdkPixbuf* biggerIconBuf = gdk_pixbuf_new_from_file(biggerIconPath,NULL);

    GList* iconList;

    iconList = g_list_insert(iconList,smallerIconBuf,1);
    iconList = g_list_insert(iconList,smallIconBuf,0);
    iconList = g_list_insert(iconList,bigIconBuf,2);
    iconList = g_list_insert(iconList,biggerIconBuf,3);

    gtk_window_set_default_icon_list(iconList);

//    delete smallerIconPath;
//    delete smallIconPath;
//    delete bigIconPath;
//    delete biggerIconPath;
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

  if( x > -1 && y > -1 ) {
    gtk_window_move(GTK_WINDOW(window),x,y);
  } else {
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER_ALWAYS);
  }

  g_handler->SetAutoResize(auto_resize);

  GtkWidget* box = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), box);

  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(destroy_handler), this);

  g_object_set_data(G_OBJECT(window),"nativewindow",this);

  Cef::AddWebView(box,url,settings);
}

void NativeWindow::Show() {
  if (browser_) {
    GtkWidget* window = gtk_widget_get_ancestor(GTK_WIDGET(browser_->GetWindowHandle()),
                               GTK_TYPE_WINDOW);
    gtk_widget_show_all(window);
  }
}

void NativeWindow::Hide() {
  if (browser_) {
    GtkWidget* window = gtk_widget_get_ancestor(GTK_WIDGET(browser_->GetWindowHandle()),
                               GTK_TYPE_WINDOW);
    gtk_widget_hide(window);
  }
}

void NativeWindow::Destroy() {
  if (browser_) {
    GtkWidget* window = gtk_widget_get_ancestor(GTK_WIDGET(browser_->GetWindowHandle()),
                               GTK_TYPE_WINDOW);
    gtk_widget_destroy(window);
  }
}

int NativeWindow::ScreenWidth() {
  GdkScreen* screen = gdk_screen_get_default();
  return gdk_screen_get_width(screen);
}

int NativeWindow::ScreenHeight() {
  GdkScreen* screen = gdk_screen_get_default();
  return gdk_screen_get_height(screen);
}


} /* appjs */
