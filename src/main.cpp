#include "appjs.h"

extern "C" void
init (v8::Handle<v8::Object> target)
{
    appjs::Init (target);
}
