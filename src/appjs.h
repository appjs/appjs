#ifndef APPJS_H
#define APPJS_H
#pragma once

#include <node.h>
#include <node_version.h>
#include <string>

#ifndef __WIN__
typedef char TCHAR;
#define TEXT(t) t
#endif

typedef std::basic_string<TCHAR> tstring;

#define APPJS_PSYMBOL(s) v8::Persistent<v8::String>::New(v8::String::NewSymbol(s))
#define MAKE_BOOLEAN(v) (v)->BooleanValue()
#define MAKE_INT32(v) (v)->Int32Value()
#define MAKE_DOUBLE(v) (v)->NumberValue()
#define STRING_EQ(v, str) (v)->Equals(String::New(str))
// Easy throw exceptions
#define NODE_ERROR(str) \
  ThrowException(Exception::Error(String::New(str)))

#define THROW_BAD_ARGS \
  ThrowException(Exception::TypeError(String::New("Bad argument")))

// Easy method definition
#define DEFINE_PROTOTYPE_METHOD(Method) \
  static v8::Handle<v8::Value> Method (const v8::Arguments& args)

// Easy method definition
#define DEFINE_CLASS_FUNCTION(Method) \
  static v8::Handle<v8::Value> Method (const v8::Arguments& args)

#define DEFINE_INSTANCE_ACCESSOR(Method) \
  static v8::Handle<v8::Value> Get##Method(v8::Local<v8::String> property, const v8::AccessorInfo &info); \
  static void Set##Method(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);

// Must have fields for object wrapper
#define DEFINE_OBJECT_FACTORY(Class) \
  public:\
    static void Init ();\
    static v8::Handle<v8::Value> NewInstance(const v8::Arguments& args);\
\
  public:\
    Class ();\
    ~Class ();\
\
    static v8::Handle<v8::Value> New (const v8::Arguments& args);\
    static v8::Persistent<v8::Function> constructor




#define DECLARE_INSTANCE_ACCESSOR(Name, Method) \
  tpl->InstanceTemplate()->SetAccessor(v8::String::New(Name), Get##Method, Set##Method)

// Better node prototype method
#define DECLARE_PROTOTYPE_METHOD(Name, Method) \
    NODE_SET_PROTOTYPE_METHOD (tpl, Name, Method)

#define DECLARE_CLASS_FUNCTION(Name, Func) \
  v8::Local<v8::String> name_##Name = v8::String::NewSymbol(#Name); \
  v8::Local<v8::Function> fn_##Name = v8::FunctionTemplate::New(Func)->GetFunction(); \
  fn_##Name->SetName(name_##Name); \
  tpl->Set(name_##Name, fn_##Name)

// Ease the 'Init' implementation
#define DECLARE_CONSTRUCTOR(Name) \
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);\
  tpl->SetClassName(String::NewSymbol(Name));\
  tpl->InstanceTemplate()->SetInternalFieldCount(1)

#define END_CONSTRUCTOR() \
  constructor = Persistent<Function>::New(tpl->GetFunction())


#define CREATE_INSTANCE_ACCESSOR(Type, PropertyName, GetterType, SetterType) \
  void Type::Set##PropertyName(Local<String> property, Local<Value> value, const AccessorInfo& info) { \
    Native##Type *obj = ObjectWrap::Unwrap<Native##Type>(info.Holder()); \
    obj->Set##PropertyName(SetterType(value)); \
  } \
  \
  v8::Handle<Value> Type::Get##PropertyName(Local<String> property, const AccessorInfo &info) { \
    HandleScope scope; \
    Native##Type *obj = ObjectWrap::Unwrap<Native##Type>(info.Holder()); \
    return scope.Close(v8::GetterType::New(obj->Get##PropertyName())); \
  }


#define CREATE_STRING_INSTANCE_ACCESSOR(Type, PropertyName, GetterType, SetterType) \
  void Type::Set##PropertyName(Local<String> property, Local<Value> value, const AccessorInfo& info) { \
    Native##Type *obj = ObjectWrap::Unwrap<Native##Type>(info.Holder()); \
    obj->Set##PropertyName(SetterType(value->ToString())); \
  } \
  \
  v8::Handle<Value> Type::Get##PropertyName(Local<String> property, const AccessorInfo &info) { \
    HandleScope scope; \
    Native##Type *obj = ObjectWrap::Unwrap<Native##Type>(info.Holder()); \
    return scope.Close(GetterType::New((uint16_t*)obj->Get##PropertyName())); \
  }


#define CREATE_PROTOTYPE_INVOKER(Type, Method) \
  v8::Handle<Value> Type::Method(const Arguments& args) { \
    HandleScope scope; \
    Native##Type *obj = ObjectWrap::Unwrap<Native##Type>(args.This()); \
    obj->Method(); \
    return scope.Close(args.This()); \
  }

#define STRING_TO_ENUM(str, value) \
  if (val->Equals(String::New(str))) { \
    enumVal = value; \
  }

#define ENUM_TO_STRING(value, str) \
  case value: \
    val = String::New(str); \
    break;



namespace appjs {

void Init (v8::Handle<v8::Object> target);

} /* appjs */

#endif /* end of APPJS_H */
