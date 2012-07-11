#include "include/cef_browser.h"
#include "includes/cef.h"
#include "includes/cef_handler.h"

using namespace v8;
using namespace appjs;

CefWindowHandle ClientHandler::GetMainHwnd(){
  return gtk_widget_get_ancestor(m_MainHwnd,GTK_TYPE_WINDOW);
}

NativeWindow* ClientHandler::GetWindow(CefWindowHandle handle){
  return (NativeWindow*)g_object_get_data(G_OBJECT(handle), "nativewindow");
}

NativeWindow* ClientHandler::GetWindow(CefRefPtr<CefBrowser> browser){
  GtkWidget* window = gtk_widget_get_ancestor(GTK_WIDGET(browser->GetWindowHandle()), GTK_TYPE_WINDOW);
  return ClientHandler::GetWindow(window);
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

bool ClientHandler::OnKeyEvent(CefRefPtr<CefBrowser> browser, KeyEventType type, int code,
                               int modifiers, bool isSystemKey, bool isAfterJavaScript) {

  if (!browser->IsPopup() && type == KEYEVENT_RAWKEYDOWN && code == 123) {
    browser->ShowDevTools();
    return true;
  }
  return false;
};

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
