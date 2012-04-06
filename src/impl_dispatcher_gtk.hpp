#ifndef IMPL_DISPATCHER_GTK_H
#define IMPL_DISPATCHER_GTK_H

#include <glib.h>
#include <functional>
#include "impl_signal_wrap.h"

// Emulate Glib::Dispatcher, a asynchorous signal

namespace appjs {
class Dispatcher: public SignalBase {
public:
    typedef std::function<void ()> Callback;

    Dispatcher (Callback callback) :
        callback_ (callback)
    { }

    virtual void emit () {
        g_idle_add (on_idle, this);
    }

private:
    Callback callback_;
    static gboolean on_idle (gpointer data) {
        static_cast<Dispatcher*> (data)->callback_ ();

        return FALSE;
    }
};
} /* appjs */

#endif /* end of IMPL_DISPATCHER_GTK_H */
