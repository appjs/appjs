#include "node_gui_dialog.h"

namespace appjs {
Persistent<FunctionTemplate> Dialog::constructor_template;

void Dialog::Init (Handle<v8::Object> target) {
    CREATE_CUSTOM_NODE_CONSTRUCTOR ("Dialog", Dialog, Window, dialog);

    GETTER_METHOD (Dialog , "run"                , gtk_dialog_run                  , gint       ) ;
    SETTER_METHOD (Dialog , "response"           , gtk_dialog_response             , gint       ) ;
    SETTER_METHOD (Dialog , "setDefaultResponse" , gtk_dialog_set_default_response , gint       ) ;
    GETTER_METHOD (Dialog , "getActionArea"      , gtk_dialog_get_action_area      , GtkWidget* ) ;
    GETTER_METHOD (Dialog , "getContentArea"     , gtk_dialog_get_content_area     , GtkWidget* ) ;

    NODE_SET_PROTOTYPE_METHOD (constructor_template, "addButton", (GetterMethod<GtkWidget*, const gchar*, gint, GtkDialog, gtk_dialog_add_button>));
    NODE_SET_PROTOTYPE_METHOD (constructor_template, "addActionWidget", (SetterMethod<GtkWidget*, gint, GtkDialog, gtk_dialog_add_action_widget>));
    NODE_SET_PROTOTYPE_METHOD (constructor_template, "setResponseSensitive", (SetterMethod<gint, gboolean, GtkDialog, gtk_dialog_set_response_sensitive>));
    NODE_SET_PROTOTYPE_METHOD (constructor_template, "setResponseForWidget", (GetterMethod<gint, GtkWidget*, GtkDialog, gtk_dialog_get_response_for_widget>));
    NODE_SET_PROTOTYPE_METHOD (constructor_template, "getWidgetForResponse", (GetterMethod<GtkWidget*, gint, GtkDialog, gtk_dialog_get_widget_for_response>));

    END_CONSTRUCTOR ();
}
}
