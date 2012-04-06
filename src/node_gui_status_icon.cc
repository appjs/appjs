#include <gtk/gtk.h>
#include "node_gui_status_icon.h"

namespace appjs {
Persistent<FunctionTemplate> StatusIcon::constructor_template;

void StatusIcon::Init (Handle<v8::Object> target) {
    CREATE_CUSTOM_NODE_CONSTRUCTOR ("StatusIcon", StatusIcon, Object, status_icon);

	SETTER_METHOD (StatusIcon , "setFromFile"      , gtk_status_icon_set_from_file      , const gchar*) ;
	SETTER_METHOD (StatusIcon , "setFromIconName"  , gtk_status_icon_set_from_icon_name , const gchar*) ;
	SETTER_METHOD (StatusIcon , "setFromStock"     , gtk_status_icon_set_from_stock     , const gchar*) ;
	GETTER_METHOD (StatusIcon , "getStock"         , gtk_status_icon_get_stock          , const gchar*) ;
	GETTER_METHOD (StatusIcon , "getIconName"      , gtk_status_icon_get_icon_name      , const gchar*) ;
	GETTER_METHOD (StatusIcon , "getSize"          , gtk_status_icon_get_size           , int         ) ;
	SETTER_METHOD (StatusIcon , "setTooltipText"   , gtk_status_icon_set_tooltip_text   , const gchar*) ;
	GETTER_METHOD (StatusIcon , "getTooltipText"   , gtk_status_icon_get_tooltip_text   , gchar*      ) ;
	SETTER_METHOD (StatusIcon , "setTooltipMarkup" , gtk_status_icon_set_tooltip_markup , const gchar*) ;
	GETTER_METHOD (StatusIcon , "getTooltipMarkup" , gtk_status_icon_get_tooltip_markup , gchar*      ) ;
	SETTER_METHOD (StatusIcon , "setHasTooltip"    , gtk_status_icon_set_has_tooltip    , gboolean    ) ;
	GETTER_METHOD (StatusIcon , "getHasTooltip"    , gtk_status_icon_get_has_tooltip    , int         ) ;
	SETTER_METHOD (StatusIcon , "setTitle"         , gtk_status_icon_set_title          , const gchar*) ;
	GETTER_METHOD (StatusIcon , "getTitle"         , gtk_status_icon_get_title          , const gchar*) ;
	SETTER_METHOD (StatusIcon , "setName"          , gtk_status_icon_set_name           , const gchar*) ;
	SETTER_METHOD (StatusIcon , "setVisible"       , gtk_status_icon_set_visible        , gboolean    ) ;
	GETTER_METHOD (StatusIcon , "getVisible"       , gtk_status_icon_get_visible        , gboolean    ) ;
	GETTER_METHOD (StatusIcon , "isEmbedded"       , gtk_status_icon_is_embedded        , gboolean    ) ;

    END_CONSTRUCTOR ();
}
} /* appjs */
