#include "util.h"

namespace appjs {

using namespace v8;

char* V8StringToChar(Handle<String> str) {
  int len = str->Utf8Length();
  char* buf = new char[len + 1];
  str->WriteUtf8(buf, len + 1);
  return buf;
}
char* V8StringToChar(Local<Value> val) {
  return V8StringToChar(val->ToString());
}

#if defined(__WIN__)
WCHAR* V8StringToWCHAR(Handle<String> str) {
  int len = str->Utf8Length();
  char* buf = new char[len + 1];
  str->WriteUtf8(buf, len + 1);

  WCHAR *wBuf;
  int nChars = MultiByteToWideChar(CP_UTF8, 0, buf, -1, NULL, 0);
  wBuf = new WCHAR[nChars];
  MultiByteToWideChar(CP_UTF8, 0, buf, -1, (LPWSTR)wBuf, nChars);

  return wBuf;
}
#endif

char* V8StringToFunctionChar(Handle<String> str) {

  Local<String> prefix = String::New("(");
  Local<String> postfix = String::New(")();");

  Local<String> result = String::Concat(prefix,str);
  result = String::Concat(result,postfix);

  int len = result->Utf8Length();
  char* buf = new char[len + 1];
  result->WriteUtf8(buf, len);
  buf[len] = '\0';
  return buf;
}


Local<String> CefStringToV8(const CefString& str) {
  return String::New(reinterpret_cast<uint16_t*>(const_cast<CefString::char_type*>(str.c_str())), str.length());
}

CefRefPtr<CefV8Value> V8StringToCef(Handle<Value> str){
  return CefV8Value::CreateString(V8StringToChar(str->ToString()));
}

Settings::Settings(Persistent<Object> settings):settings_(settings){};
Settings::Settings(Local<Object> settings):settings_(Persistent<Object>::New(settings)){};

bool Settings::has(const char* property) {
  return settings_->Has(String::New(property));
}

bool Settings::isNull(const char* property) {
  return get(property)->IsNull();
}

double Settings::getNumber(const char* property, double defaultValue = 0) {
  Local<Value> tmp = get(property);
  return (tmp->IsNumber())? tmp->NumberValue() : defaultValue;
}


int Settings::getInteger(const char* property, int defaultValue = 0) {
  Local<Value> tmp = get(property);
  return (tmp->IsNumber())? tmp->Int32Value() : defaultValue;
}

char* Settings::getString(const char* property, char* defaultValue = "") {
  Local<Value> tmp = get(property);
  return (tmp->IsString())? V8StringToChar(get(property)->ToString()) : defaultValue;
}

#ifdef __WIN__
TCHAR* Settings::getString(const char* property, TCHAR* defaultValue = L"") {
  Local<Value> tmp = get(property);
  return (tmp->IsString())? V8StringToWCHAR(get(property)->ToString()) : defaultValue;
}
#endif

bool Settings::getBoolean(const char* property, bool defaultValue = false) {
  Local<Value> tmp = get(property);
  return (tmp->IsBoolean())? tmp->BooleanValue() : defaultValue;
}


Local<Object> Settings::getObject(const char* property, Local<Object> defaultValue) {
  Local<Value> tmp = get(property);
  return (tmp->IsObject())? tmp->ToObject() : defaultValue;
}

Local<Object> Settings::getObject(const char* property) {
  Local<Value> tmp = get(property);
  return tmp->IsObject() ? tmp->ToObject() : Object::New();
}

Persistent<Object> Settings::getSettings() {
  return settings_;
}

Local<Value> Settings::get(const char* property) {
  return settings_->Get(String::New(property));
}

Local<Object> Settings::getObject(const int index,Local<Object> defaultValue) {
  Local<Value> tmp = get(index);
  return tmp->IsObject() ? tmp->ToObject() : defaultValue;
}

Local<Object> Settings::getObject(const int index) {
  Local<Value> tmp = get(index);
  return tmp->IsObject() ? tmp->ToObject() : Object::New();
}

Local<Value> Settings::get(const int index) {
  return settings_->Get(index);
}

}
