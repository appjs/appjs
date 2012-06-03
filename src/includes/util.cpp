#include "util.h"

namespace appjs {

using namespace v8;

char* V8StringToChar(Handle<String> str) {

  int len = str->Utf8Length();
  char* buf = new char[len + 1];
  str->WriteUtf8(buf, len + 1);
  return buf;
}

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

Settings::Settings(Local<Object> settings):settings_(settings){};

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

char* Settings::getString(const char* property, char* defaultValue = "") {
  Local<Value> tmp = get(property);
  return (tmp->IsString())? V8StringToChar(get(property)->ToString()) : defaultValue;
}

bool Settings::getBoolean(const char* property, bool defaultValue = false) {
  Local<Value> tmp = get(property);
  return (tmp->IsBoolean())? tmp->BooleanValue() : defaultValue;
}


Local<Object> Settings::getObject(const char* property, Local<Object> defaultValue) {
  Local<Value> tmp = get(property);
  return (tmp->IsObject())? tmp->ToObject() : defaultValue;
}

Local<Value> Settings::get(const char* property) {
  return settings_->Get(String::New(property));
}

}
