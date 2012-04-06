#include "node_gui_bin.h"

namespace appjs {
Persistent<FunctionTemplate> Bin::constructor_template;

void Bin::Init (Handle<v8::Object> target) {
    CREATE_CUSTOM_NODE_CONSTRUCTOR ("Bin", Bin, Container, bin);

    GETTER_METHOD (Bin , "getChild" , gtk_bin_get_child , GtkWidget*);

    END_CONSTRUCTOR ();
}
} /* appjs */
