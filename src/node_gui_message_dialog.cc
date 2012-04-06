#include "node_gui_message_dialog.h"

namespace appjs {
Persistent<FunctionTemplate> MessageDialog::constructor_template;

void MessageDialog::Init (Handle<v8::Object> target) {
    CREATE_CUSTOM_NODE_CONSTRUCTOR ("MessageDialog", MessageDialog, Dialog, message_dialog);

    SETTER_METHOD (MessageDialog , "setMarkup"      , gtk_message_dialog_set_markup       , const gchar* ) ;
    GETTER_METHOD (MessageDialog , "getImage"       , gtk_message_dialog_get_image        , GtkWidget*   ) ;
    SETTER_METHOD (MessageDialog , "setImage"       , gtk_message_dialog_set_image        , GtkWidget*   ) ;
    GETTER_METHOD (MessageDialog , "getMessageArea" , gtk_message_dialog_get_message_area , GtkWidget*   ) ;

    END_CONSTRUCTOR ();
}
} /* appjs */
