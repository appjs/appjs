#include <stdio.h>
#include "node_gui_object.h"
#include "impl_glue_gtk.h"

namespace appjs {
GValue&& glue (v8::Handle<v8::Value> value) {
    GValue a = { 0 };

    if (value->IsUndefined () || value->IsNull ()) {
        g_value_init (&a, G_TYPE_POINTER);
        g_value_set_pointer (&a, NULL);
    } else if (value->IsString ()) {
        g_value_init (&a, G_TYPE_STRING);
        g_value_set_string (&a, *String::Utf8Value (value));
    } else if (value->IsExternal ()) {
        g_value_init (&a, G_TYPE_POINTER);
        g_value_set_pointer (&a, v8::External::Unwrap (value));
    } else if (value->IsInt32 ()) {
        g_value_init (&a, G_TYPE_INT);
        g_value_set_int (&a, value->Int32Value ());
    } else if (value->IsBoolean ()) {
        g_value_init (&a, G_TYPE_BOOLEAN);
        g_value_set_boolean (&a, value->BooleanValue ());
    } else if (value->IsNumber ()) {
        g_value_init (&a, G_TYPE_DOUBLE);
        g_value_set_double (&a, value->NumberValue ());
    } else if (IS_INTERNAL (value)) {
        // Have passed a ObjectWrap'ed GtkObject
        GObject *obj = glue<GObject> (value);

        // Pass it
        g_value_init (&a, G_TYPE_POINTER);
        g_value_set_pointer (&a, obj);
    } else {
        g_value_init (&a, G_TYPE_INVALID);
    }

    return std::move (a);
}

v8::Handle<Value> glue (const GValue* value) {
    if (value == NULL)
        return Undefined ();

    switch (G_TYPE_FUNDAMENTAL (G_VALUE_TYPE (value))) {
        case G_TYPE_INVALID:
        case G_TYPE_NONE:
            return Undefined ();

        case G_TYPE_BOOLEAN:
            return Boolean::New (g_value_get_boolean (value));

        case G_TYPE_INT:
            return Integer::New (g_value_get_int (value));

        case G_TYPE_LONG:
            return Number::New (g_value_get_long (value));

        case G_TYPE_UINT:
            return Integer::NewFromUnsigned (g_value_get_uint (value));

        case G_TYPE_ULONG:
            return Integer::NewFromUnsigned (g_value_get_ulong (value));

        case G_TYPE_FLOAT:
            return Number::New (g_value_get_float (value));

        case G_TYPE_ENUM:
            return Integer::New (g_value_get_enum (value));

        case G_TYPE_FLAGS:
            return Integer::New (g_value_get_flags (value));

        case G_TYPE_STRING:
            return String::New (g_value_get_string (value));

        case G_TYPE_POINTER:
            return glue (g_value_get_pointer (value));

        case G_TYPE_OBJECT:
            return glue (g_value_get_object (value));

        case G_TYPE_BOXED:
            return glue (g_value_get_boxed (value));

        case G_TYPE_INTERFACE:
            return ThrowException(Exception::TypeError(
                        String::New("Don't support interface type now")));

        case G_TYPE_PARAM:
            return ThrowException(Exception::TypeError(
                        String::New("Don't support param type now")));

        default:
            fprintf (stderr, "Unknow type: %s\n", G_VALUE_TYPE_NAME (value));
            return ThrowException(Exception::TypeError(
                        String::New("Cannot find equivanent type")));
    }
}

v8::Handle<v8::Object> glue (void *widget) {
    v8::HandleScope scope;

    v8::Local<v8::Value> external = v8::External::New (widget);
    v8::Handle<v8::Object> obj = Object::constructor_template->GetFunction ()->NewInstance (1, &external);

    return scope.Close (obj);
}
} /* appjs */
