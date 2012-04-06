#ifndef NODE_GUI_GLOBAL_H
#define NODE_GUI_GLOBAL_H

#include <node.h>

namespace appjs {
// Initialize some global functions for gui
class Global {
public:
    static void Init (v8::Handle<v8::Object> target);

protected:
    DEFINE_CPP_METHOD (Ready);
    DEFINE_CPP_METHOD (Quit);

/* Not to be implemented */
private:
    Global (const Global&);
    Global& operator= (const Global&);
};
} /* appjs */

#endif /* end of NODE_GUI_GLOBAL_H */
