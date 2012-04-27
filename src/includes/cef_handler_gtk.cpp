#include "include/cef_browser.h"
#include "includes/cef.h"
#include "includes/cef_handler.h"

CefWindowHandle ClientHandler::GetMainHwnd(){
  return gtk_widget_get_ancestor(m_MainHwnd,GTK_TYPE_WINDOW);
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


  GtkWidget* window =
      gtk_widget_get_ancestor(GTK_WIDGET(browser->GetWindowHandle()),
                              GTK_TYPE_WINDOW);

  gtk_window_resize(GTK_WINDOW(window),width,height);

}
void ClientHandler::CloseMainWindow() {
  
  REQUIRE_UI_THREAD();
  appjs::Cef::Shutdown();
}

CefRefPtr<ClientHandler> g_handler;
