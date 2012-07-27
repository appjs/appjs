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

void drag_handler( GtkWidget* widget, GdkEvent* event, NativeWindow* window ) {
  gtk_window_begin_move_drag(GTK_WINDOW(widget), event->type, event->button.x_root,event->button.y_root,event->button.time);
  g_signal_handler_disconnect(G_OBJECT(widget), window->GetDragHandlerId());
}

void configure_handler( GtkWidget* widget, GdkEvent* event, NativeWindow* window ) {
  int x = event->configure.x;
  int y = event->configure.y;
  int width = event->configure.width;
  int height = event->configure.height;
  appjs_rect rect = window->GetRect();

  if ( rect.left != x || rect.top != y ) {
    window->Emit("move", x, y);
    rect.left = x;
    rect.top = y;
  }
  if( rect.width != width || rect.height != height ) {
    window->Emit("resize", width, height);
    rect.width = width;
    rect.height = height;
  }
  window->UpdatePosition(rect);
}

void state_handler( GtkWidget* widget,GdkEventWindowState* event, NativeWindow* window ) {
  if( event->new_window_state & GDK_WINDOW_STATE_ICONIFIED ) {
    window->Emit("minimize");
  } else if ( event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED ) {
    window->Emit("maximize");
  } else if ( event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN ) {
    window->Emit("fullscreen");
  } else if ( event->changed_mask == GDK_WINDOW_STATE_FULLSCREEN ||
              event->changed_mask == GDK_WINDOW_STATE_MAXIMIZED ||
              event->changed_mask == GDK_WINDOW_STATE_ICONIFIED )
  {
    window->Emit("restore");
  }
}

void NativeWindow::Init(char* url, Settings* settings) {
  handle_ = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  GtkWindow* window = (GtkWindow*)handle_;

  // Set default icon list
  if (is_main_window_) {

    GList* iconList;
    bool setIconFlag = false;

    char* smallerIconPath = icons->getString("smaller","");
    char* smallIconPath = icons->getString("small","");
    char* bigIconPath = icons->getString("big","");
    char* biggerIconPath = icons->getString("bigger","");

    if(smallerIconPath != (char*)"") {
      GdkPixbuf* smallerIconBuf = gdk_pixbuf_new_from_file(smallerIconPath,NULL);
      iconList = g_list_insert(iconList,smallerIconBuf,0);
      setIconFlag = true;
    }

    if(smallIconPath != (char*)"") {
      GdkPixbuf* smallIconBuf = gdk_pixbuf_new_from_file(smallIconPath,NULL);
      iconList = g_list_insert(iconList,smallIconBuf,1);
      setIconFlag = true;
    }

    if(bigIconPath != (char*)"") {
      GdkPixbuf* bigIconBuf = gdk_pixbuf_new_from_file(bigIconPath,NULL);
      iconList = g_list_insert(iconList,bigIconBuf,2);
      setIconFlag = true;
    }

    if(biggerIconPath != (char*)"") {
      GdkPixbuf* biggerIconBuf = gdk_pixbuf_new_from_file(biggerIconPath,NULL);
      iconList = g_list_insert(iconList,biggerIconBuf,3);
      setIconFlag = true;
    }

    if( setIconFlag )
      gtk_window_set_default_icon_list(iconList);

  }

  gtk_window_set_default_size(window, rect_.width, rect_.height);
  gtk_window_set_opacity(window, opacity_);
  gtk_window_set_decorated(window, show_chrome_);

  #if defined(__UBUNTU__) && !GTK_CHECK_VERSION(2, 24, 10)
    if(gtk_check_version(2, 24, 10))
      gtk_window_set_has_resize_grip(window, show_resize_grip);
  #endif

  if( fullscreen_ ) {
    Fullscreen();
  } else {
    gtk_window_set_resizable(window, resizable_);
  }

  if( !resizable_ ) {
    gtk_widget_set_size_request(handle_,rect_.width,rect_.height);
  }

  if( rect_.left == -1 ) {
    rect_.left = (ScreenWidth() - rect_.width) / 2;
  }

  if ( rect_.top == -1 ) {
    rect_.top = (ScreenHeight() - rect_.height) / 2;
  }

  gtk_window_move(window,rect_.left,rect_.top);

  GtkWidget* box = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(handle_), box);

  gtk_widget_add_events(handle_,  GDK_POINTER_MOTION_MASK |
                                  GDK_BUTTON_PRESS_MASK |
                                  GDK_BUTTON_RELEASE_MASK |
                                  GDK_CONFIGURE);

  g_signal_connect_after(G_OBJECT(handle_), "configure-event",G_CALLBACK(configure_handler), this );
  g_signal_connect_after(G_OBJECT(handle_), "window-state-event",G_CALLBACK(state_handler), this );

  g_object_set_data(G_OBJECT(handle_),"nativewindow",this);

  Cef::AddWebView(box,url,settings);
}

long NativeWindow::GetDragHandlerId() {
  return drag_handler_id;
}

int NativeWindow::ScreenWidth() {
  GdkScreen* screen = gdk_screen_get_default();
  return gdk_screen_get_width(screen);
}

int NativeWindow::ScreenHeight() {
  GdkScreen* screen = gdk_screen_get_default();
  return gdk_screen_get_height(screen);
}

void NativeWindow::SetWindowTitle(CefWindowHandle handle, const char* title) {
  gtk_window_set_title((GtkWindow*)handle, title);
}

void NativeWindow::Minimize() {
  gtk_window_iconify((GtkWindow*)handle_);
}

void NativeWindow::Maximize() {
  gtk_window_maximize((GtkWindow*)handle_);
}

void NativeWindow::Restore() {

  if( this->GetState() == NW_STATE_MAXIMIZED ) {
    gtk_window_unmaximize((GtkWindow*)handle_);
  } else if( this->GetState() == NW_STATE_FULLSCREEN ) {
    gtk_window_unfullscreen((GtkWindow*)handle_);
  } else {
    gtk_window_deiconify((GtkWindow*)handle_);
  }

}

void NativeWindow::Show() {
  gtk_widget_show_all(handle_);
}

void NativeWindow::Hide() {
  gtk_widget_hide(handle_);
}

void NativeWindow::Destroy() {
  gtk_widget_destroy(handle_);
}

void NativeWindow::Drag() {
  // TODO we need to emit mouse button press manually
  // after connecting the signal below. the problem is
  // about emitting that event.
  //drag_handler_id = g_signal_connect(G_OBJECT(handle_),"button-press-event",G_CALLBACK(drag_handler), this);
  /*GdkEvent* event = gdk_event_new (GDK_BUTTON_PRESS);
  event->button.window = handle_->window;
  event->button.send_event = TRUE;
  event->button.x_root = 10;
  event->button.y_root = 10;
  event->button.x = 10;
  event->button.y = 10;
  event->button.time = GDK_CURRENT_TIME;
  event->button.button = 1;*/

}

void NativeWindow::Move(int top, int left, int width, int height) {
  GtkWindow* window = (GtkWindow*)handle_;
  gtk_window_move(window,top,left);
  gtk_window_resize(window,width,height);
}

void NativeWindow::Move(int top, int left) {
  rect_.top = top;
  rect_.left = left;
  gtk_window_move((GtkWindow*)handle_,top,left);
}

void NativeWindow::Resize(int width, int height) {
  rect_.width = width;
  rect_.height = height;
  gtk_window_resize((GtkWindow*)handle_,width,height);
}

const char* NativeWindow::GetTitle() {
  char* title = (char*) gtk_window_get_title((GtkWindow*)handle_);
  if( title == NULL ) return "";
  return title;
}

void NativeWindow::Fullscreen(){
  gtk_window_fullscreen((GtkWindow*)handle_);
}

NW_STATE NativeWindow::GetState(){
  gint state = gdk_window_get_state(handle_->window);
  if (state & GDK_WINDOW_STATE_FULLSCREEN) {
    return NW_STATE_FULLSCREEN;
  } else if (state & GDK_WINDOW_STATE_MAXIMIZED) {
    return NW_STATE_MAXIMIZED;
  } else if (state & GDK_WINDOW_STATE_ICONIFIED) {
    return NW_STATE_MINIMIZED;
  } else {
    return NW_STATE_NORMAL;
  }
}

void NativeWindow::SetTopmost(bool ontop){
  gtk_window_set_keep_above((GtkWindow*)handle_, ontop);
  topmost_ = ontop;
}


void NativeWindow::SetResizable(bool resizable) {
  resizable_ = resizable;
  gtk_window_set_resizable((GtkWindow*)handle_, resizable);
}

bool NativeWindow::GetResizable() {
  return gtk_window_get_resizable((GtkWindow*)handle_);
}

void NativeWindow::SetShowChrome(bool showChrome) {
  show_chrome_ = showChrome;
  gtk_window_set_decorated((GtkWindow*)handle_, showChrome);
}

bool NativeWindow::GetShowChrome() {
  return gtk_window_get_decorated((GtkWindow*)handle_);
}

void NativeWindow::SetAlpha(bool alpha) {
  fprintf(stderr, "%s\n", "appjs-warning: alpha compositing is not available in linux");
}

bool NativeWindow::GetAlpha() {
  return false;
}

void NativeWindow::SetOpacity(double opacity) {
  opacity_ = opacity;
  gtk_window_set_opacity((GtkWindow*)handle_, opacity);
}

double NativeWindow::GetOpacity() {
  return gtk_window_get_opacity((GtkWindow*)handle_);
}

} /* appjs */
