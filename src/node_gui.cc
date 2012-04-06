#include "node_gui.h"
#include "node_gui_global.h"
#include "node_gui_clipboard.h"
#include "node_gui_object.h"
#include "node_gui_widget.h"
#include "node_gui_container.h"
#include "node_gui_bin.h"
#include "node_gui_window.h"
#include "node_gui_scrolled_window.h"
#include "node_gui_dialog.h"
#include "node_gui_message_dialog.h"
#include "node_gui_about_dialog.h"
#include "node_gui_menu.h"
#include "node_gui_status_icon.h"
#include "node_gui_webview.h"

namespace appjs {
void Init (Handle<v8::Object> target) {
    HandleScope scope;

    uv_ref (uv_default_loop ());

    // Init GTK+ in node thread
    g_thread_init (NULL);
    gdk_threads_init ();
    gtk_init (NULL, NULL);

    appjs::Global::Init (target);
    appjs::Object::Init (target);
    appjs::Clipboard::Init (target);
    appjs::Widget::Init (target);
    appjs::Container::Init (target);
    appjs::Bin::Init (target);
    appjs::Window::Init (target);
    appjs::ScrolledWindow::Init (target);
    appjs::Dialog::Init (target);
    appjs::MessageDialog::Init (target);
    appjs::AboutDialog::Init (target);
    appjs::Menu::Init (target);
    appjs::StatusIcon::Init (target);
    appjs::WebView::Init(target);
}
} /* appjs */

#ifdef WIN32
NODE_MODULE(node_appjs, appjs::Init)
#endif
