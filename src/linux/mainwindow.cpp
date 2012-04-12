#include <node.h>
#include <gtk/gtk.h>
#include "include/cef_app.h"
#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "cef/client_handler.h"
#include "linux/mainwindow.h"

extern CefRefPtr<ClientHandler> g_handler;

void TerminationSignalHandler (int signatl) {
  appjs::MainWindow::Stop();
}


static void stop (uv_handle_t* handle){
  CefShutdown();
  uv_unref(uv_default_loop());
}

uv_timer_t timer;

namespace appjs {

MainWindow::MainWindow () {
    
  g_thread_init (NULL);
  gdk_threads_init ();
  gtk_init(NULL,NULL);
  
  CefSettings settings;
  settings.pack_loading_disabled = true;
  settings.multi_threaded_message_loop = false;

  CefRefPtr<CefApp> app;
  // Initialize CEF.
  CefInitialize(settings, app);

  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

//    g_signal_connect(window, "focus", G_CALLBACK(&HandleFocus), NULL);

  GtkWidget* vbox = gtk_vbox_new(FALSE, 0);

  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(gtk_widget_destroyed), &window);
  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(Stop), NULL);

  // Create the handler.
  g_handler = new ClientHandler();
//    g_handler->SetMainHwnd(vbox);
  // Create the browser view.
  CefWindowInfo window_info;
  CefBrowserSettings browserSettings;

  // Populate the settings based on command line arguments.
  //AppGetBrowserSettings(browserSettings);

  window_info.SetAsChild(vbox);

  CefBrowser::CreateBrowserSync(window_info,
                                static_cast<CefRefPtr<CefClient> >(g_handler),
                                "http://www.google.com", browserSettings);

  gtk_container_add(GTK_CONTAINER(window), vbox);
  gtk_widget_show_all(GTK_WIDGET(window));

  // Install an signal handler so we clean up after ourselves.
  signal(SIGINT, TerminationSignalHandler);
  signal(SIGTERM, TerminationSignalHandler);

/*  CefRunMessageLoop();

  CefShutdown();
  uv_unref(uv_default_loop());*/
};

void MainWindow::Show () {

  uv_timer_init(uv_default_loop(),&timer);
  uv_timer_start(&timer,Run,1,1);
//  CefRunMessageLoop();
};

void MainWindow::Run (uv_timer_t* handle, int status) {
  CefDoMessageLoopWork();
};

void MainWindow::Stop () {
  uv_close((uv_handle_t*)&timer, stop);
};

} /* appjs */

