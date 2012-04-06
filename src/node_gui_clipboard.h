#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <memory>
#include <string>
#include <node.h>
using namespace node;
using namespace v8;

namespace appjs {
class ClipboardImpl;

class Clipboard: ObjectWrap {
public:
    Clipboard ();

    static void Init (Handle<v8::Object> target);

protected:
    static Handle<Value> New (const Arguments& args);
    static Handle<Value> Paste (const Arguments& args);

private:
    static Persistent<FunctionTemplate> constructor_template;

    std::unique_ptr<ClipboardImpl> impl_;
    void on_paste (std::string data);

/* Not to be implemented */
private:
    Clipboard (const Clipboard&);
    Clipboard& operator= (const Clipboard&);

};
}

#endif /* end of CLIPBOARD_H */
