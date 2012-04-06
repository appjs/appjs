#include <gtk/gtk.h>

#include "node_gui.h"
#include "node_gui_global.h"
#include "impl_mainloop_gtk.h"

namespace appjs {
using namespace v8;

void Global::Init (v8::Handle<v8::Object> target) {
    NODE_SET_METHOD (target, "ready", Ready);
    NODE_SET_METHOD (target, "quit", Quit);
}

Handle<Value> Global::Ready (const Arguments& args) {
    if (args.Length () != 1 || !args[0]->IsFunction ())
        return THROW_BAD_ARGS;

    // Save callback
    Persistent<Function> callback = Persistent<Function>::New (
            Local<Function>::Cast (args[0]));

    // In GTK
    MainLoop::push_job_gui ([=] () mutable {
        // Notify gtk is ready
        MainLoop::push_job_node ([=] () mutable {
            callback->Call (Context::GetCurrent ()->Global (), 0, NULL);
            callback.Dispose ();
        });
    });

    return Undefined ();
}

Handle<Value> Global::Quit (const Arguments& args) {
    gdk_threads_enter ();
    gtk_main_quit ();
    gdk_threads_leave ();

    return Undefined ();
}
}
