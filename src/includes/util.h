#ifndef APPJS_UTIL
#define APPJS_UTIL

#include "include/cef_base.h"
#include "include/cef_v8.h"
#include <node.h>

#define ARRAY_SIZE(a) \
  ((sizeof(a) / sizeof(*(a))) / \
  static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

namespace appjs {

#if defined(__WIN__)
WCHAR* V8StringToWCHAR(v8::Handle<v8::String> str);
#endif

char* V8StringToChar(v8::Handle<v8::String> str);
char* V8StringToChar(v8::Local<v8::Value> val);
char* V8StringToFunctionChar(v8::Handle<v8::String> str);
v8::Local<v8::String> CefStringToV8(const CefString& str);
CefRefPtr<CefV8Value> V8StringToCef(v8::Handle<v8::Value> str);

class Settings {
public:
  Settings(v8::Persistent<v8::Object>);
  Settings(v8::Local<v8::Object>);
  ~Settings(){settings_.Dispose();}

  bool   isNull(const char*);
  bool   has(const char*);
  double getNumber(const char*, double);
  int    getInteger(const char*, int);
  bool   getBoolean(const char*,bool);
  char*  getString(const char*, char*);

  #ifdef __WIN__
  TCHAR* getString(const char*,TCHAR*);
  #endif

  v8::Local<v8::Object> getObject(const char*,v8::Local<v8::Object>);
  v8::Local<v8::Object> getObject(const char*);
  v8::Local<v8::Object> getObject(const int,v8::Local<v8::Object>);
  v8::Local<v8::Object> getObject(const int);
  v8::Persistent<v8::Object> getSettings();

private:
  v8::Persistent<v8::Object> settings_;
  v8::Local<v8::Value> get(const int);
  v8::Local<v8::Value> get(const char*);
};

}
#endif
