#include "node_gui_container.h"

namespace appjs {
Persistent<FunctionTemplate> Container::constructor_template;

void Container::Init (Handle<v8::Object> target) {
    CREATE_CUSTOM_NODE_CONSTRUCTOR ("Container", Container, Widget, container);

	SETTER_METHOD (Container , "add"            , gtk_container_add               , GtkWidget* ) ;
	SETTER_METHOD (Container , "remove"         , gtk_container_remove            , GtkWidget* ) ;
	SIMPLE_METHOD (Container , "checkResize"    , gtk_container_check_resize                   ) ;
	GETTER_METHOD (Container , "getFocusChild"  , gtk_container_get_focus_child   , GtkWidget* ) ;
	SETTER_METHOD (Container , "setFocusChild"  , gtk_container_set_focus_child   , GtkWidget* ) ;
	SIMPLE_METHOD (Container , "resizeChildren" , gtk_container_resize_children                ) ;
	GETTER_METHOD (Container , "getBorderWidth" , gtk_container_get_border_width  , guint      ) ;
	SETTER_METHOD (Container , "setBorderWidth" , gtk_container_set_border_width  , guint      ) ;

    END_CONSTRUCTOR ();
}
}
