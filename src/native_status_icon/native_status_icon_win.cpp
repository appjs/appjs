#include <node.h>
#include <windows.h>
#include <algorithm>
#define min(left,right) std::min(left,right)
#define max(left,right) std::max(left,right)
#include <gdiplus.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "appjs.h"
#include "includes/cef.h"
#include "includes/util.h"
#include "includes/util_win.h"
#include "includes/cef_handler.h"
#include "native_status_icon/native_status_icon.h"

extern CefRefPtr<ClientHandler> g_handler;

namespace appjs {

using namespace v8;

void NativeStatusIcon::Init(Settings* settings) {

}

} /* appjs */
