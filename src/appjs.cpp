#include "appjs.h"
#include "appjs_window.h"

namespace appjs {

using namespace v8;

void Init (Handle<v8::Object> target) {
  HandleScope scope;

  appjs::Window::Init (target);
//  appjs::Menu::Init (target);
//  appjs::StatusIcon::Init (target);
}

} /* appjs */

#ifdef WIN32
NODE_MODULE(appjs, appjs::Init)
#endif
