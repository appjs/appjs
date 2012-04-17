#include "appjs_util.h"

namespace appjs {

char* StringToChar(v8::Handle<v8::String> str) {

  int len = str->Utf8Length();
  char* buf = new char[len + 1];
  str->WriteUtf8(buf, len + 1);
  return buf;
}

}
