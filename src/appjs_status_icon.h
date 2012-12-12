#ifndef APPJS_STATUS_ICON_H
#define APPJS_STATUS_ICON_H
#pragma once

#include "appjs.h"
#include "native_status_icon/native_status_icon.h"

namespace appjs {

using namespace v8;

class StatusIcon : public node::ObjectWrap {
  DEFINE_OBJECT_FACTORY(StatusIcon);
  DEFINE_PROTOTYPE_METHOD(Show);
  DEFINE_PROTOTYPE_METHOD(Hide);
};

} /* appjs */
#endif /* end of APPJS_STATUS_ICON_H */