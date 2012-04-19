#ifndef APPJS_UTIL
#define APPJS_UTIL

#include <node.h>

namespace appjs {

char* V8StringToChar(v8::Handle<v8::String> str);

}
#endif
