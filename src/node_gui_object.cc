#include <string>

#include <gdk/gdk.h>

#include "node_gui_object.h"
#include "impl_mainloop_gtk.h"
#include "impl_closure_gtk.hpp"

namespace appjs {
Persistent<FunctionTemplate> Object::constructor_template;

void Object::Init (Handle<v8::Object> target) {
    CREATE_NODE_CONSTRUCTOR ("Object", Object);

    DEFINE_NODE_METHOD ("on", On);
    DEFINE_NODE_METHOD ("getProperty", GetProperty);

    NODE_SET_PROTOTYPE_METHOD (constructor_template, "unref", (SetterMethod<void, g_object_unref>));
    NODE_SET_PROTOTYPE_METHOD (constructor_template, "setProperty", (SetterMethod<const gchar *, const GValue *, GObject, g_object_set_property>));

    END_CONSTRUCTOR ();
}

Handle<Value> Object::GetProperty (const Arguments& args) {
    HandleScope scope;

    if (args.Length () != 1)
        return THROW_BAD_ARGS;

    GObject *obj = glue<GObject> (args.This ());

    GValue key = glue (args[0]);

    // Work out property's type
    GValue value = { 0 };
    GParamSpec *spec = g_object_class_find_property (
            G_OBJECT_GET_CLASS (obj), g_value_get_string (&key));

    // Guard against invalid property
    if (spec == NULL) {
        g_value_unset (&key);
        return NODE_ERROR ("Invalid property name"); 
    }

    // Init type from property's type
    g_value_init (&value, G_PARAM_SPEC_VALUE_TYPE (spec));

    // Get it
    g_object_get_property (obj, g_value_get_string (&key), &value);
    Handle<Value> result = glue (&value);

    // And remember to release it
    g_value_unset (&key);
    g_value_unset (&value);

    return result;
}

Handle<Value> Object::On (const Arguments& args) {
    HandleScope scope;

    if (!args.Length () == 2 ||
        !args[0]->IsString () ||
        !args[1]->IsFunction ())
    {
        return THROW_BAD_ARGS;
    }

    GObject *obj = glue<GObject> (args.This ());

    // Signal name
    GValue signal = glue (args[0]);

    // Create closure
    GClosure *closure = NodeClosure::create (args[1]);

    MainLoop::push_job_gui ([=] {
        // Connect
        g_closure_set_marshal (closure, signal_marshal);
        g_signal_connect_closure (obj, g_value_get_string (&signal),
                                  closure, true);
    });

    return Undefined ();
}

void Object::signal_marshal (GClosure *closure,
                             GValue *return_value,
                             guint n_param_values,
                             const GValue *param_values,
                             gpointer invocation_hint,
                             gpointer marshal_data)
{
    // Copy params
    std::vector<GValue> params;
    for (size_t i = 1; i < n_param_values; i++) {
        GValue a = copy (param_values + i);
        params.push_back (a);
    }

    // Call it
    MainLoop::push_job_node ([=] () mutable {
        HandleScope scope;

        std::vector < Handle<Value> > args;

        // Convert arguments
        for (size_t i = 0; i < params.size (); i++) {
            args.push_back (glue (&params[i]));
        }

        (*((NodeClosure*) closure)->iter)->Call (
            Context::GetCurrent ()->Global (), args.size (), args.data ());

        // Free params
        for (size_t i = 0; i < params.size (); i++) {
            g_value_unset (&params[i]);
        }
    });
}
} /* appjs */
