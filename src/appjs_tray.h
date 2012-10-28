#ifndef APPJS_TRAY_H
#define APPJS_TRAY_H
#pragma once

#include "appjs.h"
#include "native_tray/native_tray.h"

namespace appjs {

using namespace v8;

class Tray : public node::ObjectWrap {
  DEFINE_OBJECT_FACTORY(Tray);
};

} /* appjs */
#endif /* end of APPJS_TRAY_H */