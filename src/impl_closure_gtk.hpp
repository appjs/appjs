#ifndef IMPL_CLOSURE_H
#define IMPL_CLOSURE_H

#include <list>

#include <node.h>
#include <glib-object.h>
#include "impl_mainloop_gtk.h"

namespace appjs {
using namespace node;
using namespace v8;

// Store all the callbacks passed in closure
typedef std::list< Persistent<Function> > cb_store;
static cb_store callbacks;

struct NodeClosure {
    GClosure closure;
    cb_store::iterator iter; // Don't store function here, just store iterator

    static GClosure* create (Local<Value> callback) {
        HandleScope scope;

        // Save function to GClosure
        GClosure *closure = g_closure_new_simple (sizeof (NodeClosure), 0);
        ((NodeClosure*) closure)->iter = callbacks.insert (callbacks.end (),
            Persistent<Function>::New (Local<Function>::Cast (callback)));

        g_closure_add_invalidate_notifier (closure, NULL, closure_invalidate);

        return closure;
    }

    static void closure_invalidate (gpointer data, GClosure *closure) {
        cb_store::iterator iter = ((NodeClosure*) closure)->iter;

        // Delete function in node thread
        MainLoop::push_job_node ([=] () mutable {
            iter->Dispose ();
            callbacks.erase (iter);
        });
    }
};
} /* appjs */

#endif /* end of IMPL_CLOSURE_H */
