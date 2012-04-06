#include "node_gui_scrolled_window.h"

namespace appjs {
Persistent<FunctionTemplate> ScrolledWindow::constructor_template;

void ScrolledWindow::Init (Handle<v8::Object> target) {
    CREATE_CUSTOM_NODE_CONSTRUCTOR ("ScrolledWindow", ScrolledWindow, Bin, scrolled_window);

    END_CONSTRUCTOR ();
}
} /* appjs */
