#ifndef NODE_GUI_MENU_H
#define NODE_GUI_MENU_H

#include "node_gui_widget.h"

namespace appjs {
class Menu: public Widget {
DECLARE_NODE_OBJECT (Menu);

protected:
    DEFINE_CPP_METHOD (Popup);
};
}  /* appjs */

#endif /* end of NODE_GUI_MENU_H */
