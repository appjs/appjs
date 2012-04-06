#ifndef NODE_GUI_H
#define NODE_GUI_H

#include <node.h>

// Easy throw exceptions
#define NODE_ERROR(str) \
    ThrowException(Exception::Error(String::New(str)))

#define THROW_BAD_ARGS \
    ThrowException(Exception::TypeError(String::New("Bad argument")))

// Check is internal object
#define IS_INTERNAL(obj) \
    (obj->IsObject () && (v8::Handle<v8::Object>::Cast (obj)->InternalFieldCount () == 2))

// Wrap around existing object, don't manage its life
#define WRAP_EXSISTING_OBJECT(T) \
    if (args.Length () == 1 && args[0]->IsExternal ()) {\
        void *widget = External::Unwrap (args[0]);\
        args.This ()->SetPointerInInternalField (0, widget);\
        args.This ()->SetPointerInInternalField (1, nullptr);\
\
        return args.This ();\
    } else if (args.Length () == 1 && IS_INTERNAL (args[0])) {\
        Local<v8::Object> obj = Local<v8::Object>::Cast (args[0]);\
        args.This ()->SetPointerInInternalField (0, obj->GetPointerFromInternalField (0));\
        args.This ()->SetPointerInInternalField (1, obj->GetPointerFromInternalField (1));\
\
        return args.This ();\
    }

// Easy for define methods
#define DEFINE_CPP_METHOD(Method) \
    static v8::Handle<v8::Value> Method (const v8::Arguments& args)

#define DEFINE_NODE_METHOD(Name, Method) \
    NODE_SET_PROTOTYPE_METHOD (constructor_template, Name, Method)

#define SIMPLE_METHOD(Class, Name, Method) \
    NODE_SET_PROTOTYPE_METHOD (constructor_template, Name, \
            (SetterMethod<Gtk##Class, Method>));

#define SETTER_METHOD(Class, Name, Method, Type) \
    NODE_SET_PROTOTYPE_METHOD (constructor_template, Name, \
            (SetterMethod<Type, Gtk##Class, Method>));

#define OUTTER_METHOD(Class, Name, Method, Type) \
    NODE_SET_PROTOTYPE_METHOD (constructor_template, Name, \
            (OutterMethod<Type, Gtk##Class, Method>));

#define GETTER_METHOD(Class, Name, Method, Type) \
    NODE_SET_PROTOTYPE_METHOD (constructor_template, Name, \
            (GetterMethod<Type, Gtk##Class, Method>));

// Must have fields for object wrapper
#define DECLARE_NODE_OBJECT(Class) \
    public:\
        static void Init (Handle<v8::Object> target);\
        static Persistent<FunctionTemplate> constructor_template;\
\
    private:\
        Class (const Class&);\
        Class& operator= (const Class&)

// Ease the 'Init' implementation
#define ATTACH_CONSTRUCTOR(Name, Type, Constructor) \
    HandleScope scope;\
    Local<String> symbol = String::NewSymbol(Name);\
    Local<FunctionTemplate> t = FunctionTemplate::New (Constructor);\
    constructor_template = Persistent<FunctionTemplate>::New(t);\
    constructor_template->InstanceTemplate()->SetInternalFieldCount(2);\
    constructor_template->SetClassName(symbol)

#define ATTACH_INHERITANCE(Super) \
    constructor_template->Inherit (Super::constructor_template)

#define CREATE_NODE_CONSTRUCTOR(Name, Type) \
    ATTACH_CONSTRUCTOR(Name, Type, Constructor<Type>)

#define CREATE_NODE_CONSTRUCTOR_INHERIT(Name, Type, Super) \
    CREATE_NODE_CONSTRUCTOR(Name, Type);\
    ATTACH_INHERITANCE(Super)

#define CREATE_CUSTOM_NODE_CONSTRUCTOR(Name, Type, Super, Method) \
    ATTACH_CONSTRUCTOR(Name, Type, (Constructor<Type, gtk_##Method##_get_type>));\
    ATTACH_INHERITANCE(Super)

#define END_CONSTRUCTOR() \
    target->Set (symbol, t->GetFunction ())

// Generate javascript type from GTK+ object's type
#define DEFINE_GLUE(Type) \
    v8::Handle<v8::Object> glue (Gtk##Type *widget) {\
        v8::HandleScope scope;\
\
        v8::Local<v8::Value> external = v8::External::New (widget);\
        v8::Handle<v8::Object> obj = Type::constructor_template->GetFunction ()->NewInstance (1, &external);\
\
        return scope.Close (obj);\
    }

namespace appjs {
using namespace v8;

void Init (v8::Handle<v8::Object> target);
} /* appjs */

#endif /* end of NODE_GUI_H */
