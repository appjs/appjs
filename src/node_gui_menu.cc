#include <gtk/gtk.h>
#include "node_gui_menu.h"

namespace appjs {
Persistent<FunctionTemplate> Menu::constructor_template;
 
void Menu::Init (Handle<v8::Object> target) {
    CREATE_CUSTOM_NODE_CONSTRUCTOR ("Menu", Menu, Widget, menu);

    SETTER_METHOD (Menu , "setTitle"        , gtk_menu_set_title             , const gchar* ) ;
    GETTER_METHOD (Menu , "getTitle"        , gtk_menu_get_title             , const gchar* ) ;
    SETTER_METHOD (Menu , "setMonitor"      , gtk_menu_set_monitor           , gint         ) ;
    GETTER_METHOD (Menu , "getMonitor"      , gtk_menu_get_monitor           , gint         ) ;
    GETTER_METHOD (Menu , "getTearoffState" , gtk_menu_get_tearoff_state     , gboolean     ) ;
    SETTER_METHOD (Menu , "setTearoffState" , gtk_menu_set_tearoff_state     , gboolean     ) ;
    SIMPLE_METHOD (Menu , "popdown"         , gtk_menu_popdown                              ) ;
    SIMPLE_METHOD (Menu , "reposition"      , gtk_menu_reposition                           ) ;
    SIMPLE_METHOD (Menu , "detach"          , gtk_menu_detach                               ) ;

    NODE_SET_PROTOTYPE_METHOD (constructor_template, "popup", Popup); 

    END_CONSTRUCTOR ();
}
   
Handle<Value> Menu::Popup (const Arguments& args) {
    HandleScope scope;

    if (args.Length () != 2 && args.Length () !=3)
        return THROW_BAD_ARGS;

    GtkMenu *obj = glue<GtkMenu> (args.This ());

    if (args.Length () == 2) {
        // popup (button, activate_time)
        GValue arg0 = glue (args[0]);
        GValue arg1 = glue (args[1]);

        MainLoop::push_job_gui ([=] () mutable {
            gtk_menu_popup (obj, NULL, NULL, NULL, NULL, raw<int> (&arg0), raw<int> (&arg1));

            g_value_unset (&arg0);
            g_value_unset (&arg1);
        });
    } else if (args.Length () == 3) {
        // popup (widget, button, activate_time)
        GValue arg0 = glue (args[0]);
        GValue arg1 = glue (args[1]);
        GValue arg2 = glue (args[2]);

        MainLoop::push_job_gui ([=] () mutable {
            gtk_menu_popup (obj, NULL, NULL, gtk_status_icon_position_menu,
                raw<gpointer> (&arg0), raw<int> (&arg1), raw<int> (&arg2));

            g_value_unset (&arg0);
            g_value_unset (&arg1);
            g_value_unset (&arg2);
        });
    }

    return Undefined ();
}
}  /* appjs */
