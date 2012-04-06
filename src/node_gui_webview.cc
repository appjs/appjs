#include <webkit/webkit.h>
#include "node_gui_webview.h"

namespace appjs {
Persistent<FunctionTemplate> WebView::constructor_template;

void WebView::Init (Handle<v8::Object> target) {
//    CREATE_CUSTOM_NODE_CONSTRUCTOR ("WebView", WebView, Container, text_view);
    //ATTACH_CONSTRUCTOR("WebView", WebKitWebView, (Constructor<WebKitWebView, webkit_web_view_get_type>));
    ATTACH_CONSTRUCTOR("WebView", WebView, New);\
    ATTACH_INHERITANCE(Container);

//    GETTER_METHOD (TextView , "getBuffer" , gtk_text_view_get_buffer, GtkTextBuffer *) ;
//    SETTER_METHOD (TextView , "setBuffer" , gtk_text_view_set_buffer, GtkTextBuffer *) ;

    END_CONSTRUCTOR ();
}

// var webview = new WebView ('http://example.com', callback);
Handle<Value> WebView::New (const Arguments& args) {
    HandleScope scope;

    // Check parameters
    if (!(args.Length () == 2 && args[0]->IsString () && args[1]->IsFunction ())) {
        return THROW_BAD_ARGS;
    }

    std::string url = *String::Utf8Value (args[0]);
    auto callback = Persistent<Function>::New (Local<Function>::Cast (args[1]));
    auto self = Persistent<v8::Object>::New (args.This ());

    // In GTK
    MainLoop::push_job_gui ([=] () mutable {
        GtkWidget *obj = webkit_web_view_new ();

        webkit_web_view_load_uri (WEBKIT_WEB_VIEW(obj), url.c_str ());
        webkit_web_view_set_transparent (WEBKIT_WEB_VIEW(obj), TRUE);
        // Notify the creation
        MainLoop::push_job_node ([=] () mutable {
            // Store the builder
            self->SetPointerInInternalField (0, obj);
            self->SetPointerInInternalField (1, nullptr);

            // Callback
            Handle<Value> args[] = { self };
            callback->Call (self, 1, args);

            // Free them
            callback.Dispose ();
            self.Dispose ();
        });
    });

    args.This ()->SetPointerInInternalField (0, nullptr);
    args.This ()->SetPointerInInternalField (1, nullptr);
    return args.This ();
}
} /* appjs */
