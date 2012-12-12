#include <node.h>
#include <gtk/gtk.h>
#include <vector>

#include "appjs.h"
#include "native_status_icon/native_status_icon.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/cef_handler.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

void status_icon_show_menu(GtkWidget* statusIcon,guint button, guint32 activate_time,GtkMenu* menu) {
  gtk_menu_popup(menu, NULL, NULL, gtk_status_icon_position_menu, statusIcon, button, activate_time);
}

void status_icon_click(GtkWidget* statusIcon,NativeStatusIcon* me) {
  me->Emit("click");
}

void status_icon_popup(GtkWidget* statusIcon,guint button, guint32 activate_time,NativeStatusIcon* me) {
  me->Emit("rightclick");
}

void NativeStatusIcon::Init(Settings* settings) {
  char* iconFile = settings->getString("icon","");
  char* tooltip  = settings->getString("tooltip","");
  bool  visible  = settings->getBoolean("visible",true);

  // TODO local or persistent?
  Local<Object> menu = settings->getObject("menu");

  NativeMenu* nativeMenu;
  GtkStatusIcon *statusIcon;

  if( settings->has("icon") ) {
    statusIcon = gtk_status_icon_new_from_file(iconFile);
  } else {
    statusIcon = gtk_status_icon_new_from_stock(GTK_STOCK_MISSING_IMAGE);
  }

  statusIconHandle_ = statusIcon;

  if( settings->has("tooltip") ) {
    gtk_status_icon_set_tooltip_text(statusIcon, tooltip);
    gtk_status_icon_set_title(statusIcon,tooltip);
  }

  if( settings->has("menu") ) {

    GtkWidget* gtkMenu = gtk_menu_new();

    nativeMenu = (NativeMenu*)menu->GetPointerFromInternalField(0);
    nativeMenu->Attach(GTK_MENU_SHELL(gtkMenu));

    g_signal_connect(statusIcon, "popup-menu", GTK_SIGNAL_FUNC (status_icon_show_menu), gtkMenu);

  }

  g_signal_connect(statusIcon, "activate", G_CALLBACK(status_icon_click), this);
  g_signal_connect(statusIcon, "popup-menu", G_CALLBACK(status_icon_popup), this);

  gtk_status_icon_set_visible(statusIcon, visible);
}

void NativeStatusIcon::Show(){
  gtk_status_icon_set_visible(statusIconHandle_, TRUE);
  this->Emit("show");
}

void NativeStatusIcon::Hide(){
  gtk_status_icon_set_visible(statusIconHandle_, FALSE);
  this->Emit("hide");
}

} /* appjs */
