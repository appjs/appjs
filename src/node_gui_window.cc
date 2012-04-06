#include "node_gui_window.h"

namespace appjs {
Persistent<FunctionTemplate> Window::constructor_template;

void Window::Init (Handle<v8::Object> target) {
    CREATE_CUSTOM_NODE_CONSTRUCTOR ("Window", Window, Bin, window);

    SETTER_METHOD (Window , "setTitle"             , gtk_window_set_title                                         , const gchar* ) ;
    GETTER_METHOD (Window , "getTitle"             , gtk_window_get_title                                         , const gchar* ) ;
    SETTER_METHOD (Window , "setRole"              , gtk_window_set_role                                          , const gchar* ) ;
    SETTER_METHOD (Window , "setStartupId"         , gtk_window_set_startup_id                                    , const gchar* ) ;
    GETTER_METHOD (Window , "getRole"              , gtk_window_get_role                                          , const gchar* ) ;
    GETTER_METHOD (Window , "activateFocus"        , gtk_window_activate_focus                                    , gboolean     ) ;
    GETTER_METHOD (Window , "activateDefault"      , gtk_window_activate_default                                  , gboolean     ) ;
    SETTER_METHOD (Window , "setFocus"             , gtk_window_set_focus                                         , GtkWidget*   ) ;
    GETTER_METHOD (Window , "getFocus"             , gtk_window_get_focus                                         , GtkWidget*   ) ;
    SETTER_METHOD (Window , "setDefault"           , gtk_window_set_default                                       , GtkWidget*   ) ;
    GETTER_METHOD (Window , "getDefault"           , gtk_window_get_default_widget                                , GtkWidget*   ) ;
    SETTER_METHOD (Window , "setOpacity"           , gtk_window_set_opacity                                       , double       ) ;
    GETTER_METHOD (Window , "getOpacity"           , gtk_window_get_opacity                                       , double       ) ;
    SETTER_METHOD (Window , "setSkipTaskbarHint"   , gtk_window_set_skip_taskbar_hint                             , gboolean     ) ;
    GETTER_METHOD (Window , "getSkipTaskbarHint"   , gtk_window_get_skip_taskbar_hint                             , gboolean     ) ;
    SETTER_METHOD (Window , "setSkipPagerHint"     , gtk_window_set_skip_pager_hint                               , gboolean     ) ;
    GETTER_METHOD (Window , "getSkipPagerHint"     , gtk_window_get_skip_pager_hint                               , gboolean     ) ;
    SETTER_METHOD (Window , "setUrgencyHint"       , gtk_window_set_urgency_hint                                  , gboolean     ) ;
    GETTER_METHOD (Window , "getUrgencyHint"       , gtk_window_get_urgency_hint                                  , gboolean     ) ;
    SETTER_METHOD (Window , "setAcceptFocus"       , gtk_window_set_accept_focus                                  , gboolean     ) ;
    GETTER_METHOD (Window , "getAcceptFocus"       , gtk_window_get_accept_focus                                  , gboolean     ) ;
    SETTER_METHOD (Window , "setFocusOnMap"        , gtk_window_set_focus_on_map                                  , gboolean     ) ;
    GETTER_METHOD (Window , "getFocusOnMap"        , gtk_window_get_focus_on_map                                  , gboolean     ) ;
    SETTER_METHOD (Window , "setDestroyWithParent" , gtk_window_set_destroy_with_parent                           , gboolean     ) ;
    GETTER_METHOD (Window , "getDestroyWithParent" , gtk_window_get_destroy_with_parent                           , gboolean     ) ;
    SETTER_METHOD (Window , "setMnemonicsVisible"  , gtk_window_set_mnemonics_visible                             , gboolean     ) ;
    GETTER_METHOD (Window , "getMnemonicsVisible"  , gtk_window_get_mnemonics_visible                             , gboolean     ) ;
    SETTER_METHOD (Window , "setResizable"         , gtk_window_set_resizable                                     , gboolean     ) ;
    GETTER_METHOD (Window , "getResizable"         , gtk_window_get_resizable                                     , gboolean     ) ;
    GETTER_METHOD (Window , "isActive"             , gtk_window_is_active                                         , gboolean     ) ;
    GETTER_METHOD (Window , "hasToplevelFocus"     , gtk_window_has_toplevel_focus                                , gboolean     ) ;
    SETTER_METHOD (Window , "setDecorated"         , gtk_window_set_decorated                                     , gboolean     ) ;
    GETTER_METHOD (Window , "getDecorated"         , gtk_window_get_decorated                                     , gboolean     ) ;
    SETTER_METHOD (Window , "setDeletable"         , gtk_window_set_deletable                                     , gboolean     ) ;
    GETTER_METHOD (Window , "getDeletable"         , gtk_window_get_deletable                                     , gboolean     ) ;
    SETTER_METHOD (Window , "setIconName"          , gtk_window_set_icon_name                                     , const gchar* ) ;
    GETTER_METHOD (Window , "getIconName"          , gtk_window_get_icon_name                                     , const gchar* ) ;
    SETTER_METHOD (Window , "setModal"             , gtk_window_set_modal                                         , gboolean     ) ;
    GETTER_METHOD (Window , "getModal"             , gtk_window_get_modal                                         , gboolean     ) ;
    SIMPLE_METHOD (Window , "present"              , gtk_window_present                                                          ) ;
    SETTER_METHOD (Window , "presentWithTime"      , gtk_window_present_with_time                                 , guint32      ) ;
    SIMPLE_METHOD (Window , "iconify"              , gtk_window_iconify                                                          ) ;
    SIMPLE_METHOD (Window , "deiconify"            , gtk_window_deiconify                                                        ) ;
    SIMPLE_METHOD (Window , "stick"                , gtk_window_stick                                                            ) ;
    SIMPLE_METHOD (Window , "unstick"              , gtk_window_unstick                                                          ) ;
    SIMPLE_METHOD (Window , "maximize"             , gtk_window_maximize                                                         ) ;
    SIMPLE_METHOD (Window , "unmaximize"           , gtk_window_unmaximize                                                       ) ;
    SIMPLE_METHOD (Window , "fullscreen"           , gtk_window_fullscreen                                                       ) ;
    SIMPLE_METHOD (Window , "unfullscreen"         , gtk_window_unfullscreen                                                     ) ;
    SETTER_METHOD (Window , "setKeepAbove"         , gtk_window_set_keep_above                                    , gboolean     ) ;
    SETTER_METHOD (Window , "setKeepBelow"         , gtk_window_set_keep_below                                    , gboolean     ) ;

    END_CONSTRUCTOR ();
}
} /* appjs */
