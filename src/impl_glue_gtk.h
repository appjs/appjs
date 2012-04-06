#ifndef IMPL_VALUE_GTK_H
#define IMPL_VALUE_GTK_H

#include <node.h>
#include <glib-object.h>

namespace appjs {
// Generic v8::Value to GValue
GValue&& glue (v8::Handle<v8::Value> value);

// Generic GValue to v8::Value
v8::Handle<v8::Value> glue (const GValue* value);

// Get GTK+ object from javascript object
template<class T>
inline T* glue (v8::Handle<v8::Value> obj) {
    return static_cast<T*> (v8::Handle<v8::Object>::Cast (obj)->GetPointerFromInternalField (0));
}

// Convert GTK+ object to javascript object 
v8::Handle<v8::Object> glue (void *widget);
v8::Handle<v8::Object> glue (GtkTextBuffer *widget);

// From c type to v8::Value
inline v8::Handle<v8::Value> glue (int i) {
    return v8::Integer::New (i);
}

inline v8::Handle<v8::Value> glue (unsigned int i) {
    return v8::Integer::NewFromUnsigned (i);
}

inline v8::Handle<v8::Value> glue (long unsigned i) {
    return v8::Integer::NewFromUnsigned (i);
}

inline v8::Handle<v8::Value> glue (char *i) {
    g_free (i);
    return v8::String::New (i);
}

inline v8::Handle<v8::Value> glue (const char *i) {
    return v8::String::New (i);
}

inline v8::Handle<v8::Value> glue (bool i) {
    return v8::Boolean::New (i);
}

inline v8::Handle<v8::Value> glue (double i) {
    return v8::Number::New (i);
}

// Wrap around generic Gtk object
//template <class T>
//v8::Handle<v8::Value> glue (T *widget);

// Convert GValue to its raw state
template<class T>
T raw (const GValue* value) {
    return static_cast<T> (g_value_get_pointer (value));
}

template<class T*>
inline T* raw (const GValue* value) {
    return static_cast<T*> (g_value_get_pointer (value));
}

template<>
inline gpointer raw (const GValue* value) {
    return g_value_get_pointer (value);
}

template<>
inline unsigned raw (const GValue* value) {
    return g_value_get_int (value);
}

template<>
inline const gchar *raw (const GValue* value) {
    return g_value_get_string (value);
}

template<>
inline float raw (const GValue* value) {
    return static_cast<float> (g_value_get_double (value));
}

template<>
inline double raw (const GValue* value) {
    return g_value_get_double (value);
}

template<>
inline const GValue* raw (const GValue* value) {
    return value;
}

template<>
inline int raw (const GValue* value) {
    // We cannot distinguish between gboolean and int,
    // so we have to determin at runtime
    if (G_TYPE_FUNDAMENTAL(G_VALUE_TYPE(value)) == G_TYPE_BOOLEAN)
        return g_value_get_boolean (value);
    else
        return g_value_get_int (value);
}

// Copy GValue
inline GValue&& copy (const GValue* value) {
    GValue a = { 0 };
    g_value_init (&a, G_VALUE_TYPE (value));
    g_value_copy (value, &a);

    return std::move (a); 
}
} /* appjs */

#endif /* end of IMPL_VALUE_GTK_H */
