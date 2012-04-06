#ifndef IMPL_GTK_H
#define IMPL_GTK_H

#include <string>
#include <memory>
#include <functional>

#include <node.h>
#include <gtk/gtk.h>

#ifdef WIN32
#include "impl_clipboard_win.h"
#endif

namespace appjs {

class ClipboardImpl {
public:
    ClipboardImpl (std::function<void (std::string)> paste);

    // Set clipboard's data
    void set_data (std::string data);

private:
#ifdef WIN32
    std::unique_ptr<ClipboardImplW32> monitor_;
#endif
    std::function<void (std::string)> paste_;

    bool i_changed_board_;

    void create_clipboard ();
    static void on_changed (GtkClipboard*, GdkEvent*, gpointer);
    static void on_received (GtkClipboard*, const gchar*, gpointer);

/* Not to be implemented */
private:
    ClipboardImpl (const ClipboardImpl&);
    ClipboardImpl& operator= (const ClipboardImpl&);

};
}

#endif /* end of IMPL_LINUX_H */
