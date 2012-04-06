#include "node_gui_about_dialog.h"

namespace appjs {
Persistent<FunctionTemplate> AboutDialog::constructor_template;

void AboutDialog::Init (Handle<v8::Object> target) {
    CREATE_CUSTOM_NODE_CONSTRUCTOR ("AboutDialog", AboutDialog, Dialog, about_dialog);

	GETTER_METHOD (AboutDialog , "getProgramName"       , gtk_about_dialog_get_program_name       , const gchar* ) ;
	SETTER_METHOD (AboutDialog , "setProgramName"       , gtk_about_dialog_set_program_name       , const gchar* ) ;
	GETTER_METHOD (AboutDialog , "getVersion"           , gtk_about_dialog_get_version            , const gchar* ) ;
	SETTER_METHOD (AboutDialog , "setVersion"           , gtk_about_dialog_set_version            , const gchar* ) ;
	GETTER_METHOD (AboutDialog , "getCopyright"         , gtk_about_dialog_get_copyright          , const gchar* ) ;
	SETTER_METHOD (AboutDialog , "setCopyright"         , gtk_about_dialog_set_copyright          , const gchar* ) ;
	GETTER_METHOD (AboutDialog , "getComments"          , gtk_about_dialog_get_comments           , const gchar* ) ;
	SETTER_METHOD (AboutDialog , "setComments"          , gtk_about_dialog_set_comments           , const gchar* ) ;
	GETTER_METHOD (AboutDialog , "getLicense"           , gtk_about_dialog_get_license            , const gchar* ) ;
	SETTER_METHOD (AboutDialog , "setLicense"           , gtk_about_dialog_set_license            , const gchar* ) ;
	GETTER_METHOD (AboutDialog , "getWrapLicense"       , gtk_about_dialog_get_wrap_license       , gboolean     ) ;
	SETTER_METHOD (AboutDialog , "setWrapLicense"       , gtk_about_dialog_set_wrap_license       , gboolean     ) ;
	GETTER_METHOD (AboutDialog , "getWebsite"           , gtk_about_dialog_get_website            , const gchar* ) ;
	SETTER_METHOD (AboutDialog , "setWebsite"           , gtk_about_dialog_set_website            , const gchar* ) ;
	GETTER_METHOD (AboutDialog , "getWebsiteLabel"      , gtk_about_dialog_get_website_label      , const gchar* ) ;
	SETTER_METHOD (AboutDialog , "setWebsiteLabel"      , gtk_about_dialog_set_website_label      , const gchar* ) ;
	GETTER_METHOD (AboutDialog , "getTranslatorCredits" , gtk_about_dialog_get_translator_credits , const gchar* ) ;
	SETTER_METHOD (AboutDialog , "setTranslatorCredits" , gtk_about_dialog_set_translator_credits , const gchar* ) ;
	GETTER_METHOD (AboutDialog , "getLogoIconName"      , gtk_about_dialog_get_logo_icon_name     , const gchar* ) ;
	SETTER_METHOD (AboutDialog , "setLogoIconName"      , gtk_about_dialog_set_logo_icon_name     , const gchar* ) ;

    END_CONSTRUCTOR ();
}
} /* appjs */
