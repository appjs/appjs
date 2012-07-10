#ifndef APPJS_H
#define APPJS_H
#pragma once

#include <node.h>
#include <node_version.h>

// Easy throw exceptions
#define NODE_ERROR(str) \
  ThrowException(Exception::Error(String::New(str)))

#define THROW_BAD_ARGS \
  ThrowException(Exception::TypeError(String::New("Bad argument")))

// Easy method definition
#define DEFINE_CPP_METHOD(Method) \
  static v8::Handle<v8::Value> Method (const v8::Arguments& args)

#define CREATE_CPP_ACCESSOR(Name, Method) \
  tpl->InstanceTemplate()->SetAccessor(String::New(Name), Get##Method, Set##Method)

#define DEFINE_CPP_ACCESSOR(Method) \
  static v8::Handle<v8::Value> Get##Method(v8::Local<v8::String> property, const v8::AccessorInfo &info); \
  static void Set##Method(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);

// Better node prototype method
#define DEFINE_PROTOTYPE_METHOD(Name, Method) \
    NODE_SET_PROTOTYPE_METHOD (tpl, Name, Method)

// Must have fields for object wrapper
#define DECLARE_NODE_OBJECT_FACTORY(Class) \
  public:\
    static void Init ();\
    static Handle<Value> NewInstance(const Arguments& args);\
\
  public:\
    Class ();\
    ~Class ();\
\
    static Handle<Value> New (const Arguments& args);\
    static Persistent<Function> constructor

// Ease the 'Init' implementation
#define CREATE_NODE_CONSTRUCTOR(Name) \
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);\
  tpl->SetClassName(String::NewSymbol(Name));\
  tpl->InstanceTemplate()->SetInternalFieldCount(1)

#define END_CONSTRUCTOR() \
    constructor = Persistent<Function>::New(tpl->GetFunction())



namespace appjs {

void Init (v8::Handle<v8::Object> target);

} /* appjs */

#endif /* end of APPJS_H */
