#ifndef NODE_GUI_OBJECT_H
#define NODE_GUI_OBJECT_H

#include <vector>
#include <string>

#include <gtk/gtk.h>

#include "node_gui.h"
#include "impl_glue_gtk.h"
#include "impl_mainloop_gtk.h"

namespace appjs {
using namespace node;
using namespace v8;

class Object {
DECLARE_NODE_OBJECT (Object);

protected:
    DEFINE_CPP_METHOD (On);
    DEFINE_CPP_METHOD (GetProperty);

    // Create a slice of memory and attach it to the object,
    // use by GtkTextIter and GtkTreeIter, etc.
    template<class Type, class GtkType>
    static Handle<Value> New (const Arguments& args) {
        HandleScope scope;

        WRAP_EXSISTING_OBJECT (Type);

        // Wrap it
        args.This ()->SetPointerInInternalField (0, g_slice_new (GtkType));
        args.This ()->SetPointerInInternalField (1, nullptr);

        return args.This ();
    }

    // Define abstract type's constructor
    template<class Type>
    static Handle<Value> Constructor (const Arguments& args) {
        HandleScope scope;

        WRAP_EXSISTING_OBJECT (Type);

        return NODE_ERROR ("Cannot create instance of abstract type.");
    }

    // Define generic constructor
    template<class Type, GType get_type (void)>
    static Handle<Value> Constructor (const Arguments& args) {
        HandleScope scope;

        WRAP_EXSISTING_OBJECT (Type);

        if (!((args.Length () == 0) ||
              (args.Length () == 1 && args[0]->IsObject ())))
            return THROW_BAD_ARGS;

        void *widget;

        if (args.Length () == 0) {
            // Just null new
            widget = g_object_new (get_type (), NULL);
        } else {
            // Get the parameters object
            Local<v8::Object> params = Local<v8::Object>::Cast (args[0]);

            // Receive arguments from object
            Local<Array> names = params->GetPropertyNames ();

            // Keep strings in this list, so when we create parameters the
            // raw string stays valid.
            std::vector<std::string> tmp_store;
            tmp_store.reserve (names->Length ());
            std::vector<GParameter> parameters (names->Length ());

            // Push parameters
            for (size_t i = 0; i < names->Length (); ++i) {
                tmp_store.push_back (*String::Utf8Value (names->Get (i)));

                parameters[i].name = tmp_store.back ().c_str ();
                parameters[i].value = glue (params->Get (names->Get (i)));
            }

            // Create object
            widget = g_object_newv (get_type (), parameters.size (), parameters.data ());

            // Clean parameters
            for (auto it = parameters.begin (); it != parameters.end (); ++it) {
                g_value_unset (&it->value);
            }
        }

        if (widget == NULL)
            return NODE_ERROR ("Cannot create object");

        // Wrap it
        args.This ()->SetPointerInInternalField (0, widget);
        args.This ()->SetPointerInInternalField (1, nullptr);

        return args.This ();
    }

    // Define methods without any arguments.
    // Example: from 'gtk_widget_show_all' to 'show'
    template<class GtkType, class Obj, void function (Obj)>
    static Handle<Value> SetterMethod (const Arguments& args) {
        HandleScope scope;

        GtkType *obj = glue<GtkType> (args.This ());

        MainLoop::push_job_gui ([=] {
            function (obj);
        });

        return Undefined ();
    }

    // A Story: Why those fucking templates?
    // void function (const GtkType*) is different with
    // void function (GtkType*), if we want to use one template to fit
    // these two types of functions, we must define two template functions.

    template<class GtkType, void function (const GtkType*)>
    static inline Handle<Value> SetterMethod (const Arguments& args) {
        return SetterMethod<GtkType, const GtkType*, function> (args);
    }

    template<class GtkType, void function (GtkType*)>
    static inline Handle<Value> SetterMethod (const Arguments& args) {
        return SetterMethod<GtkType, GtkType*, function> (args);
    }

    // Define setter methods
    // Convert from 'gtk_status_icon_set_name (name)' to 'set_name (name)'
    template<class ARG0,
             class GtkType, class Obj, void function (Obj, ARG0)>
    static Handle<Value> SetterMethod (const Arguments& args) {
        HandleScope scope;

        if (args.Length () != 1)
            return THROW_BAD_ARGS;

        GtkType *obj = glue<GtkType> (args.This ());

        GValue value = glue (args[0]);

        MainLoop::push_job_gui ([=] () mutable {
            function (obj, raw<ARG0> (&value));
            g_value_unset (&value);
        });

        return Undefined ();
    }

    template<class ARG0, class GtkType, void function (const GtkType*, ARG0)>
    static inline Handle<Value> SetterMethod (const Arguments& args) {
        return SetterMethod<ARG0, GtkType, const GtkType*, function> (args);
    }

    template<class ARG0, class GtkType, void function (GtkType*, ARG0)>
    static inline Handle<Value> SetterMethod (const Arguments& args) {
        return SetterMethod<ARG0, GtkType, GtkType*, function> (args);
    }

    // Define setter methods
    template<class ARG0, class ARG1,
             class GtkType, class Obj, void function (Obj, ARG0, ARG1)>
    static Handle<Value> SetterMethod (const Arguments& args) {
        HandleScope scope;

        if (args.Length () != 2)
            return THROW_BAD_ARGS;

        GtkType *obj = glue<GtkType> (args.This ());

        GValue arg0 = glue (args[0]);
        GValue arg1 = glue (args[1]);

        MainLoop::push_job_gui ([=] () mutable {
            function (obj, raw<ARG0> (&arg0), raw<ARG1> (&arg1));
            g_value_unset (&arg0);
            g_value_unset (&arg1);
        });

        return Undefined ();
    }

    template<class ARG0, class ARG1, class GtkType, void function (const GtkType*, ARG0, ARG1)>
    static inline Handle<Value> SetterMethod (const Arguments& args) {
        return SetterMethod<ARG0, ARG1, GtkType, const GtkType*, function> (args);
    }

    template<class ARG0, class ARG1, class GtkType, void function (GtkType*, ARG0, ARG1)>
    static inline Handle<Value> SetterMethod (const Arguments& args) {
        return SetterMethod<ARG0, ARG1, GtkType, GtkType*, function> (args);
    }

    // Define getter methods
    // Convert from 'gtk_status_icon_get_stock ()' to 'get_name ()'
    template<class ReturnType,
             class GtkType, class Obj, ReturnType function (Obj)>
    static Handle<Value> GetterMethod (const Arguments& args) {
        HandleScope scope;

        GtkType *obj = glue<GtkType> (args.This ());

        gdk_threads_enter ();
        ReturnType result = function (obj);
        gdk_threads_leave ();

        return scope.Close (glue (static_cast<ReturnType> (result)));
    }

    template<class ReturnType, class GtkType, ReturnType function (const GtkType*)>
    static inline Handle<Value> GetterMethod (const Arguments& args) {
        return GetterMethod<ReturnType, GtkType, const GtkType*, (ReturnType (*) (const GtkType *)) function> (args);
    }

    template<class ReturnType, class GtkType, ReturnType function (GtkType*)>
    static inline Handle<Value> GetterMethod (const Arguments& args) {
        return GetterMethod<ReturnType, GtkType, GtkType*, (ReturnType (*) (GtkType *)) function> (args);
    }

    template<class ReturnType, class ARG0,
             class GtkType, class Obj, ReturnType function (Obj, ARG0)>
    static Handle<Value> GetterMethod (const Arguments& args) {
        HandleScope scope;

        if (args.Length () != 1)
            return THROW_BAD_ARGS;

        GtkType *obj = glue<GtkType> (args.This ());

        GValue arg0 = glue (args[0]);

        gdk_threads_enter ();
        ReturnType result = function (obj, raw<ARG0> (&arg0));
        gdk_threads_leave ();

        g_value_unset (&arg0);

        return scope.Close (glue (result));
    }

    template<class ReturnType, class ARG0, class GtkType, ReturnType function (const GtkType*, ARG0)>
    static inline Handle<Value> GetterMethod (const Arguments& args) {
        return GetterMethod<ReturnType, ARG0, GtkType, const GtkType*, (ReturnType (*) (const GtkType *, ARG0)) function> (args);
    }

    template<class ReturnType, class ARG0, class GtkType, ReturnType function (GtkType*, ARG0)>
    static inline Handle<Value> GetterMethod (const Arguments& args) {
        return GetterMethod<ReturnType, ARG0, GtkType, GtkType*, (ReturnType (*) (GtkType *, ARG0)) function> (args);
    }

    template<class ReturnType, class ARG0, class ARG1,
             class GtkType, class Obj, ReturnType function (Obj, ARG0, ARG1)>
    static Handle<Value> GetterMethod (const Arguments& args) {
        HandleScope scope;

        if (args.Length () != 2)
            return THROW_BAD_ARGS;

        GtkType *obj = glue<GtkType> (args.This ());

        GValue arg0 = glue (args[0]);
        GValue arg1 = glue (args[1]);

        gdk_threads_enter ();
        ReturnType result = function (obj, raw<ARG0> (&arg0), raw<ARG1> (&arg1));
        gdk_threads_leave ();

        g_value_unset (&arg0);
        g_value_unset (&arg1);

        return scope.Close (glue (result));
    }

    template<class ReturnType, class ARG0, class ARG1, class GtkType, ReturnType function (const GtkType*, ARG0, ARG1)>
    static inline Handle<Value> GetterMethod (const Arguments& args) {
        return GetterMethod<ReturnType, ARG0, ARG1, GtkType, const GtkType*, (ReturnType (*) (const GtkType *, ARG0, ARG1)) function> (args);
    }

    template<class ReturnType, class ARG0, class ARG1, class GtkType, ReturnType function (GtkType*, ARG0, ARG1)>
    static inline Handle<Value> GetterMethod (const Arguments& args) {
        return GetterMethod<ReturnType, ARG0, ARG1, GtkType, GtkType*, (ReturnType (*) (GtkType *, ARG0, ARG1)) function> (args);
    }

    // Force running in the same thread with node,
    // mainly used with functions with [out] parameters
    template<class GtkType, class Obj, void function (Obj)>
    static Handle<Value> OutterMethod (const Arguments& args) {
        HandleScope scope;

        GtkType *obj = glue<GtkType> (args.This ());

        gdk_threads_enter ();
        function (obj);
        gdk_threads_leave ();

        return Undefined ();
    }

    template<class GtkType, void function (const GtkType*)>
    static inline Handle<Value> OutterMethod (const Arguments& args) {
        return OutterMethod<GtkType, const GtkType*, function> (args);
    }

    template<class GtkType, void function (GtkType*)>
    static inline Handle<Value> OutterMethod (const Arguments& args) {
        return OutterMethod<GtkType, GtkType*, function> (args);
    }

    template<class ARG0,
             class GtkType, class Obj, void function (Obj, ARG0)>
    static Handle<Value> OutterMethod (const Arguments& args) {
        HandleScope scope;

        if (args.Length () != 1)
            return THROW_BAD_ARGS;

        GtkType *obj = glue<GtkType> (args.This ());

        GValue value = glue (args[0]);

        gdk_threads_enter ();
        function (obj, raw<ARG0> (&value));
        gdk_threads_leave ();

        g_value_unset (&value);

        return Undefined ();
    }

    template<class ARG0, class GtkType, void function (const GtkType*, ARG0)>
    static inline Handle<Value> OutterMethod (const Arguments& args) {
        return OutterMethod<ARG0, GtkType, const GtkType*, function> (args);
    }

    template<class ARG0, class GtkType, void function (GtkType*, ARG0)>
    static inline Handle<Value> OutterMethod (const Arguments& args) {
        return OutterMethod<ARG0, GtkType, GtkType*, function> (args);
    }

    template<class ARG0, class ARG1,
             class GtkType, class Obj, void function (Obj, ARG0, ARG1)>
    static Handle<Value> OutterMethod (const Arguments& args) {
        HandleScope scope;

        if (args.Length () != 2)
            return THROW_BAD_ARGS;

        GtkType *obj = glue<GtkType> (args.This ());

        GValue arg0 = glue (args[0]);
        GValue arg1 = glue (args[1]);

        gdk_threads_enter ();
        function (obj, raw<ARG0> (&arg0), raw<ARG1> (&arg1));
        gdk_threads_leave ();

        g_value_unset (&arg0);
        g_value_unset (&arg1);

        return Undefined ();
    }

    template<class ARG0, class ARG1, class GtkType, void function (const GtkType*, ARG0, ARG1)>
    static inline Handle<Value> OutterMethod (const Arguments& args) {
        return OutterMethod<ARG0, ARG1, GtkType, const GtkType*, function> (args);
    }

    template<class ARG0, class ARG1, class GtkType, void function (GtkType*, ARG0, ARG1)>
    static inline Handle<Value> OutterMethod (const Arguments& args) {
        return OutterMethod<ARG0, ARG1, GtkType, GtkType*, function> (args);
    }

protected:
    static void signal_marshal (GClosure *closure,
                                GValue *return_value,
                                guint n_param_values,
                                const GValue *param_values,
                                gpointer invocation_hint,
                                gpointer marshal_data);
};
} /* appjs */

#endif /* end of NODE_GUI_OBJECT_H */
