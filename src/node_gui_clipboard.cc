#include "node_gui_clipboard.h"
#include "impl_clipboard_gtk.h"

namespace appjs {
Persistent<FunctionTemplate> Clipboard::constructor_template;

Clipboard::Clipboard () :
    impl_ (new ClipboardImpl (std::bind (&Clipboard::on_paste,
                                         this,
                                         std::placeholders::_1)))
{
}

void Clipboard::Init (Handle<v8::Object> target) {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New (New);
    constructor_template = Persistent<FunctionTemplate>::New(t);
    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("Clipboard"));

    NODE_SET_PROTOTYPE_METHOD (constructor_template, "paste", Paste);
    target->Set (String::NewSymbol ("Clipboard"), t->GetFunction ());
}

Handle<Value> Clipboard::New (const Arguments& args) {
    HandleScope scope;

    Clipboard *clip = new Clipboard ();

    clip->Wrap (args.This ());
    clip->Ref ();
    return args.This ();
}

Handle<Value> Clipboard::Paste (const Arguments& args) {
    HandleScope scope;

    if (args.Length () == 1 && args[0]->IsString ()) {
        Clipboard *self = ObjectWrap::Unwrap<Clipboard> (args.This());

        self->impl_->set_data (*String::Utf8Value (args[0]));
    }

    return v8::Undefined ();
}

void Clipboard::on_paste (std::string str) {
    HandleScope scope;

    // Read data from clipboard
    Local<String> data = String::New (str.c_str (), str.size ());

    // Then send it
    Local<Value> argv[] = { String::New ("copy"), data };

    Local<Value> emit_v = handle_->Get (String::New ("emit"));
    Local<Function> emit = Local<Function>::Cast(emit_v);

    emit->Call (handle_, 2, argv);
}
}
