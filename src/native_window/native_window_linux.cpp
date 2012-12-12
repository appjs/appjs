#include <node.h>
#include <gtk/gtk.h>
#include <vector>
#include <sstream>

#include "appjs.h"
#include "native_window/native_window.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/cef_handler.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

static void dialog_response_handler( GtkDialog *dialog, gint response_id, gpointer data ) {

  uv_work_t* req = (uv_work_t*) data;
  AppjsDialogSettings* settings = (AppjsDialogSettings*)req->data;

  if( response_id == GTK_RESPONSE_ACCEPT ) {
    GSList* filenames = gtk_file_chooser_get_filenames( GTK_FILE_CHOOSER(dialog) );
    settings->result  = filenames;
  } else {
    settings->result = NULL;
  }

  g_object_unref (dialog);
  gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void dialog_destroy_handler( GtkDialog *dialog, gpointer data ) {
  uv_work_t* req = (uv_work_t*) data;
  NativeWindow::ProcessFileDialog(req);
}

static void drag_handler( GtkWidget* widget, GdkEvent* event, NativeWindow* window ) {
  gtk_window_begin_move_drag(GTK_WINDOW(widget), event->type, event->button.x_root,event->button.y_root,event->button.time);
  g_signal_handler_disconnect(G_OBJECT(widget), window->GetDragHandlerId());
}

static void configure_handler( GtkWidget* widget, GdkEvent* event, NativeWindow* window ) {
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

static void state_handler( GtkWidget* widget,GdkEventWindowState* event, NativeWindow* window ) {
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

static void destroy_handler(int status = 0) {
  g_handler->Shutdown();
}

void AddWebView(CefWindowHandle& parent, char* url, Settings* settings) {
  CefWindowInfo windowInfo;
  windowInfo.SetAsChild(parent);
  g_handler->browserSettings_.web_security_disabled = settings->getBoolean("disableSecurity", false);
  CefBrowser::CreateBrowser(windowInfo, static_cast<CefRefPtr<CefClient>>(g_handler), url, g_handler->browserSettings_);

  if (!g_handler->HasMainWindow()) {
    signal(SIGINT, destroy_handler);
    signal(SIGTERM, destroy_handler);
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
  gtk_window_set_skip_taskbar_hint(window, !show_on_taskbar_);
  gtk_window_set_skip_pager_hint(window, !show_on_pager_);

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

  AddWebView(box,url,settings);
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

NativeWindow* NativeWindow::GetWindow(CefWindowHandle handle){
  return (NativeWindow*)g_object_get_data(G_OBJECT(handle), "nativewindow");
}

NativeWindow* NativeWindow::GetWindow(CefRefPtr<CefBrowser> browser){
  return GetWindow(gtk_widget_get_ancestor(GTK_WIDGET(browser->GetWindowHandle()), GTK_TYPE_WINDOW));
}

void NativeWindow::SetMenuBar(NativeMenu* nativeMenu) {

  GtkWidget* menu_bar = gtk_menu_bar_new ();
  nativeMenu->Attach(GTK_MENU_SHELL(menu_bar));

  gtk_box_pack_start(GTK_BOX (gtk_bin_get_child(GTK_BIN(handle_))), menu_bar, FALSE, FALSE, 0);
  gtk_box_reorder_child(GTK_BOX (gtk_bin_get_child(GTK_BIN(handle_))),menu_bar,0);
  gtk_widget_show(menu_bar);

}

void NativeWindow::OpenFileDialog(uv_work_t* req) {

  GtkWidget*             dialog;
  GtkFileChooserAction  actions;
  AppjsDialogSettings* settings = (AppjsDialogSettings*)req->data;
  GtkWindow*             parent = (GtkWindow*)settings->me->handle_;
  std::string       acceptTypes = settings->reserveString1;
  bool              multiSelect = settings->reserveBool1;
  bool                dirSelect = settings->reserveBool2;

  if( settings->type == NW_DIALOGTYPE_FILE_OPEN ) {

    if( dirSelect ) {
      actions = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
    } else {
      actions = GTK_FILE_CHOOSER_ACTION_OPEN;
    }

    dialog = gtk_file_chooser_dialog_new (settings->title.c_str(),
                parent,
                actions,
                GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                NULL);

    if( multiSelect ) {
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), true);
    }

    if( dirSelect ) {
      gtk_file_chooser_set_create_folders(GTK_FILE_CHOOSER(dialog), true);
    }

    gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dialog), true);
    gtk_file_chooser_select_filename(GTK_FILE_CHOOSER(dialog),settings->initialValue.c_str());

  } else if ( settings->type == NW_DIALOGTYPE_FILE_SAVE ) {

    actions = GTK_FILE_CHOOSER_ACTION_SAVE;

    dialog = gtk_file_chooser_dialog_new (settings->title.c_str(),
                parent,
                actions,
                GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                NULL);

    gtk_file_chooser_set_create_folders(GTK_FILE_CHOOSER(dialog), true);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), true);

    gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dialog), true);
    gtk_file_chooser_select_filename(GTK_FILE_CHOOSER(dialog),settings->initialValue.c_str());

  }

  std::stringstream ss(acceptTypes);
  std::string filterStr;
  while(std::getline(ss, filterStr, ',')) {
    std::stringstream filterStream(filterStr);
    std::string filterPatterns;
    std::vector<std::string> temp;
    while(std::getline(filterStream,filterPatterns,':')) {
      temp.push_back(filterPatterns);
    }
    GtkFileFilter* filter = gtk_file_filter_new();

    if(temp.size() == 1) {
      // Filter Pattern has no label
      // Insert the pattern itself aas the label
      gtk_file_filter_set_name(filter,temp.back().c_str());
    } else {
      // Filter Pattern has label
      gtk_file_filter_set_name(filter,temp.front().c_str());
    }

    std::stringstream filterPatternStream(temp.back());
    std::string filterPattern;
    while(std::getline(filterPatternStream,filterPattern,';')) {
      gtk_file_filter_add_pattern(filter,filterPattern.c_str());
    }

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),filter);
  }

  g_object_ref (dialog);

  g_signal_connect( dialog,"response",G_CALLBACK (dialog_response_handler),req );
  g_signal_connect( dialog,"destroy",G_CALLBACK (dialog_destroy_handler),req );

  gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);

  gtk_widget_show (GTK_WIDGET (dialog));

}

void NativeWindow::ProcessFileDialog(uv_work_t* req) {

  AppjsDialogSettings* settings = (AppjsDialogSettings*)req->data;
  void*                  result = settings->result;
  Persistent<Function>       cb = settings->cb;

  if( result != NULL ) {
    GSList*   filenames = (GSList*)result;
    int          length = g_slist_length(filenames);
    Local<Array> newObj = Array::New(length);
    Handle<Value> error = Undefined();
    int           index = 0;

    for (GSList* iter = filenames; iter != NULL; iter = g_slist_next(iter)) {
      newObj->Set( index, String::New( (gchar*)iter->data ) );
      g_free( iter->data );
      index++;
    }

    g_slist_free( filenames );

    const int argc  = 2;
    Local<Value> argv[argc] = { Local<Value>::New(error), Local<Value>::New(newObj) };
    cb->Call( settings->me->GetV8Handle(), argc, argv );
  } else {
    const int argc  = 1;
    Local<Value> argv[argc] = { Local<Value>::New(String::New("canceled")) };
    cb->Call( settings->me->GetV8Handle(), argc, argv );
  }

  cb.Dispose();
  NativeWindow::DialogClosed();
}

void NativeWindow::OpenColorDialog(uv_work_t* req) {

}

void NativeWindow::ProcessColorDialog(uv_work_t* req) {

}

void NativeWindow::OpenFontDialog(uv_work_t* req) {
  GtkWidget*             dialog;
  GtkFileChooserAction  actions;
  AppjsDialogSettings* settings = (AppjsDialogSettings*)req->data;
  GtkWindow*             parent = (GtkWindow*)settings->me->handle_;
  const int                size = settings->reserveNumber1;
  const char*        sampleText = settings->reserveString1.c_str();
  Cef::Pause();
  gdk_threads_enter();

  dialog = gtk_font_selection_dialog_new( settings->title.c_str() );
  gtk_font_selection_dialog_set_font_name( GTK_FONT_SELECTION_DIALOG(dialog), settings->initialValue.c_str() );
  gtk_font_selection_dialog_set_preview_text( GTK_FONT_SELECTION_DIALOG(dialog), sampleText );

  if( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT ) {
    char* selection  = gtk_font_selection_dialog_get_font_name( GTK_FONT_SELECTION_DIALOG(dialog) );
    settings->result = selection;
  } else {
    settings->result = NULL;
  }

}

void NativeWindow::ProcessFontDialog(uv_work_t* req) {
  AppjsDialogSettings* settings = (AppjsDialogSettings*)req->data;
  void*                  result = settings->result;
  Persistent<Function>       cb = settings->cb;

  if( result != NULL ) {
    Handle<Value>   error = Undefined();
    Handle<Value> fontName = String::New((char*)result);
    const int argc  = 2;
    Local<Value> argv[argc] = { Local<Value>::New(error), Local<Value>::New(fontName) };
    cb->Call( settings->me->GetV8Handle(), argc, argv );
  } else {
    const int argc  = 1;
    Local<Value> argv[argc] = { Local<Value>::New(String::New("canceled")) };
    cb->Call( settings->me->GetV8Handle(), argc, argv );
  }

  cb.Dispose();
  NativeWindow::DialogClosed();
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

void NativeWindow::Focus() {
  gtk_window_present((GtkWindow*)handle_);
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

void NativeWindow::Move(int left, int top, int width, int height) {
  GtkWindow* window = (GtkWindow*)handle_;
  gtk_window_move(window,left,top);
  gtk_window_resize(window,width,height);
}

void NativeWindow::Move(int left, int top) {
  rect_.left = left;
  rect_.top = top;
  gtk_window_move((GtkWindow*)handle_,left,top);
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

void NativeWindow::SetTitle(const char* title) {
  gtk_window_set_title((GtkWindow*)handle_, title);
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
