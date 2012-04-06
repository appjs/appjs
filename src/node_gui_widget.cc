#include "node_gui_widget.h"

namespace appjs {
Persistent<FunctionTemplate> Widget::constructor_template;

void Widget::Init (Handle<v8::Object> target) {
    CREATE_NODE_CONSTRUCTOR_INHERIT ("Widget", Widget, Object);

    SIMPLE_METHOD (Widget , "destroy"      , gtk_widget_destroy);
    SIMPLE_METHOD (Widget , "unparent"     , gtk_widget_unparent);
    SIMPLE_METHOD (Widget , "show"         , gtk_widget_show);
    SIMPLE_METHOD (Widget , "showNow"      , gtk_widget_show_now);
    SIMPLE_METHOD (Widget , "hide"         , gtk_widget_hide);
    SIMPLE_METHOD (Widget , "showAll"      , gtk_widget_show_all);
    SIMPLE_METHOD (Widget , "map"          , gtk_widget_map);
    SIMPLE_METHOD (Widget , "unmap"        , gtk_widget_unmap);
    SIMPLE_METHOD (Widget , "realize"      , gtk_widget_realize);
    SIMPLE_METHOD (Widget , "unrealize"    , gtk_widget_unrealize);
    SIMPLE_METHOD (Widget , "queueDraw"    , gtk_widget_queue_draw);
    SIMPLE_METHOD (Widget , "queueResize"  , gtk_widget_queue_resize);
    GETTER_METHOD (Widget , "activate"     , gtk_widget_activate       , gboolean       );
    GETTER_METHOD (Widget , "isFocus"      , gtk_widget_is_focus       , gboolean       );
    SIMPLE_METHOD (Widget , "grabFocus"    , gtk_widget_grab_focus);
    SIMPLE_METHOD (Widget , "grabDefault"  , gtk_widget_grab_default);
    SETTER_METHOD (Widget , "setName"      , gtk_widget_set_name       , const gchar *) ;
    GETTER_METHOD (Widget , "getName"      , gtk_widget_get_name       , const gchar *) ;
    SETTER_METHOD (Widget , "setSensitive" , gtk_widget_set_sensitive  , gboolean     ) ;

    END_CONSTRUCTOR ();
}
} /* appjs */
