#include "node_gui.h"

extern "C" void
init (v8::Handle<v8::Object> target)
{
    appjs::Init (target);
}
