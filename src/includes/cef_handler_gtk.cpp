#include "include/cef_browser.h"
#include "includes/cef.h"
#include "includes/cef_handler.h"

using namespace v8;
using namespace appjs;

CefWindowHandle ClientHandler::GetMainHwnd(){
  return gtk_widget_get_ancestor(m_MainHwnd,GTK_TYPE_WINDOW);
}


Handle<Object> ClientHandler::CreatedBrowser(CefRefPtr<CefBrowser> browser) {
  GtkWidget* window =
      gtk_widget_get_ancestor(GTK_WIDGET(browser->GetWindowHandle()),
                              GTK_TYPE_WINDOW);

  NativeWindow* nativewindow = (NativeWindow*)g_object_get_data(G_OBJECT(window),"nativewindow");
  nativewindow->SetBrowser(browser);
  return nativewindow->GetV8Handle();
}


Handle<Object> ClientHandler::GetV8WindowHandle(CefRefPtr<CefBrowser> browser) {
  GtkWidget* window =
      gtk_widget_get_ancestor(GTK_WIDGET(browser->GetWindowHandle()),
                              GTK_TYPE_WINDOW);

  NativeWindow* nativewindow = (NativeWindow*)g_object_get_data(G_OBJECT(window),"nativewindow");

  return nativewindow->GetV8Handle();
}

NativeWindow* ClientHandler::WindowFromHandle(CefWindowHandle handle){
  NativeWindow* nativewindow = (NativeWindow*)g_object_get_data(G_OBJECT(handle), "nativewindow");
  return nativewindow;
}


void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title)
{
  REQUIRE_UI_THREAD();

  GtkWidget* window =
      gtk_widget_get_ancestor(GTK_WIDGET(browser->GetWindowHandle()),
                              GTK_TYPE_WINDOW);
  std::string titleStr(title);
  gtk_window_set_title(GTK_WINDOW(window), titleStr.c_str());
}

void ClientHandler::OnContentsSizeChange(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    int width,
                                    int height)
{
  REQUIRE_UI_THREAD();

  if(this->m_AutoResize) {
    GtkWidget* window =
        gtk_widget_get_ancestor(GTK_WIDGET(browser->GetWindowHandle()),
                                GTK_TYPE_WINDOW);

    gtk_window_resize(GTK_WINDOW(window),width,height);
  }
}
void ClientHandler::CloseMainWindow() {

  REQUIRE_UI_THREAD();
  appjs::Cef::Shutdown();
}

CefRefPtr<ClientHandler> g_handler;
