#ifndef NODE_GUI_WEBVIEW
#define NODE_GUI_WEBVIEW

#include "node_gui_container.h"

namespace appjs {
class WebView: public Container {
DECLARE_NODE_OBJECT (WebView);

protected:
    DEFINE_CPP_METHOD (New);

};
} /* appjs */

#endif /* end of NODE_GUI_TEXT_VIEW_H */
