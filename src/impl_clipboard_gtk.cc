#include "impl_clipboard_gtk.h"
#include "impl_mainloop_gtk.h"

namespace appjs {
ClipboardImpl::ClipboardImpl (std::function<void (std::string)> paste) :
    paste_ (std::move (paste)),
    i_changed_board_ (false)
{
    MainLoop::push_job_gui (
            std::bind (&ClipboardImpl::create_clipboard, this));
}

void ClipboardImpl::set_data (std::string data) {
    // Set flag
    i_changed_board_ = true;

    // Tell the GUI thread to read the paste
    MainLoop::push_job_gui ([=] {
        gtk_clipboard_set_text (
            gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
            data.c_str (),
            data.length ());
    });
}

void ClipboardImpl::create_clipboard () {
    // Monitor clipboard
#ifdef WIN32
    monitor_.reset (new ClipboardImplW32 (
                std::bind (&ClipboardImpl::on_changed, this)));
#else
    g_signal_connect (
        gtk_clipboard_get (GDK_SELECTION_CLIPBOARD),
        "owner-change",
        G_CALLBACK (on_changed),
        this);
#endif
}

void ClipboardImpl::on_changed (GtkClipboard *clipboard,
                                GdkEvent *event,
                                gpointer data)
{
    ClipboardImpl *self = static_cast<ClipboardImpl*> (data);

    // Guard from own changes
    if (self->i_changed_board_) {
        self->i_changed_board_ = false;
        return;
    }

    gtk_clipboard_request_text (clipboard, on_received, self);
}

void ClipboardImpl::on_received (GtkClipboard *clipboard,
                                 const gchar *text,
                                 gpointer data)
{
    if (text == NULL) 
        return;

    ClipboardImpl *self = static_cast<ClipboardImpl*> (data);

    // Notice node
    MainLoop::push_job_node (std::bind (self->paste_, std::string (text)));
}
}
