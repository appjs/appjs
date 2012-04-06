#include <gtk/gtk.h>
#include "impl_async_wrap.h"
#include "impl_mainloop_gtk.h"
#include "impl_dispatcher_gtk.hpp"

namespace appjs {
MainLoop* MainLoop::self = NULL;

MainLoop* MainLoop::get () {
    if (self)
        return self;
    else
        return (self = new MainLoop ());
}

MainLoop::MainLoop ()
{
    // Init channel after thread_init
    channel.reset (new MyChannel ());

    // Init signal in node thread
    channel->init<MyChannel::NODE> (new AsyncDispatcher (
                std::bind (&MainLoop::do_jobs<MyChannel::NODE>, this)));

    g_thread_create (main, this, false, NULL);
}

void* MainLoop::main (void *data) {
    // Init signal in gui thread
    Dispatcher *sig = new Dispatcher (
                std::bind (&MainLoop::do_jobs<MyChannel::GUI>, self));
    self->channel->init<MyChannel::GUI> (sig);

    gdk_threads_enter ();
    gtk_main ();
    gdk_threads_leave ();

    uv_unref (uv_default_loop ());

    return NULL;
}

template<MainLoop::MyChannel::JOB_TYPE type>
void MainLoop::do_jobs () {
    // Fetch and do jobs
    MyChannel::JobsList list = channel->move_jobs<type> ();
    for (auto it = list.begin (); it != list.end (); ++it) {
        std::get<0> (*it) ();
    }
}
} /* appjs */
