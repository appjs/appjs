#include <node.h>
#include <gtk/gtk.h>

#include "appjs.h"
#include "native_menu/native_menu.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/cef_handler.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

typedef struct _appjs_action_callback {
  Persistent<Object> action;
  Persistent<Object> item;
} appjs_action_callback;

void MenuClicked(GtkWidget* menuItem,appjs_action_callback* actionCallback){
  Persistent<Object> action = actionCallback->action;
  const int argc = 1;
  Handle<Value> argv[argc] = {actionCallback->item};
  action->CallAsFunction(action,argc,argv);
}

void NativeMenu::Init(Settings* settings) {

  GtkWidget* menu = gtk_menu_new();

  AddSubMenu(menu,settings);

  GList* menuItems = gtk_container_get_children(GTK_CONTAINER(menu));

  for(GList *list = menuItems; list; list = list->next) {
    gtk_container_remove(GTK_CONTAINER(menu),(GtkWidget*)list->data);
    menuItems_.push_back((GtkWidget*)list->data);
  }

}

int NativeMenu::AddSubMenu(GtkWidget* menu,Settings* settings){

  int length = settings->getInteger("length",0);

  Settings* item;
  GtkWidget* menuItem;
  char* label;
  char* icon;
  appjs_action_callback* actionCb = new appjs_action_callback();

  for( int i = 0; i < length; i++ ) {

    item = new Settings( settings->getObject( i ) );
    label  = item->getString("label","");
    icon   = item->getString("icon","");
    actionCb->action = Persistent<Object>::New( item->getObject("action") );
    actionCb->item = Persistent<Object>::New( settings->getObject( i ) );

    if( strlen( label ) == 0 ) {
      menuItem = gtk_separator_menu_item_new();
    } else if( strlen( icon ) == 0 ) {
      menuItem = gtk_menu_item_new_with_mnemonic( label );
    } else {
      menuItem = gtk_image_menu_item_new_with_mnemonic( label );
      GtkWidget* image = gtk_image_new_from_file(icon);
      gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuItem),image);
      gtk_image_menu_item_set_always_show_image(GTK_IMAGE_MENU_ITEM(menuItem),TRUE);
    }

    Settings* subsettings = new Settings(item->getObject("submenu"));
    GtkWidget* submenu = gtk_menu_new();

    if(AddSubMenu(submenu,subsettings)){
      gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem),submenu);
    } else {
      g_signal_connect(G_OBJECT(menuItem), "activate",G_CALLBACK(MenuClicked),actionCb);
    }

    gtk_menu_shell_append( GTK_MENU_SHELL( menu ), menuItem );
    gtk_widget_show( menuItem );
  }

  return length;

}

} /* appjs */
