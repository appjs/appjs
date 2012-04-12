#ifndef APPJS_H
#define APPJS_H
#pragma once

#include <node.h>

// Easy throw exceptions
#define NODE_ERROR(str) \
  ThrowException(Exception::Error(String::New(str)))

#define THROW_BAD_ARGS \
  ThrowException(Exception::TypeError(String::New("Bad argument")))

// Better node prototype method
#define DEFINE_NODE_METHOD(Name, Method) \
    NODE_SET_PROTOTYPE_METHOD (constructor_template, Name, Method)

// Must have fields for object wrapper
#define DECLARE_NODE_OBJECT(Class) \
    public:\
        static void Init (Handle<v8::Object> target);\
        static Persistent<FunctionTemplate> constructor_template;\
\
    private:\
        Class (const Class&);\
        Class& operator= (const Class&);\
        static Handle<Value> New (const Arguments& args);

// Ease the 'Init' implementation
#define CREATE_NODE_CONSTRUCTOR(Name, Type) \
    HandleScope scope;\
    Local<String> symbol = String::NewSymbol(Name);\
    Local<FunctionTemplate> t = FunctionTemplate::New (New);\
    constructor_template = Persistent<FunctionTemplate>::New(t);\
    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);\
    constructor_template->SetClassName(symbol)

#define END_CONSTRUCTOR() \
    target->Set (symbol, t->GetFunction ())

namespace appjs {

void Init (v8::Handle<v8::Object> target);

} /* appjs */

#endif /* end of APPJS_H */
