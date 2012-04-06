#include "impl_async_wrap.h"

namespace appjs {
AsyncDispatcher::AsyncDispatcher (Callback callback) :
    callback_ (callback)
{
    handle_.data = this;

    // Init libuv stuff
    uv_async_init (uv_default_loop (), &handle_, on_async);
    uv_unref (uv_default_loop ());
}

void AsyncDispatcher::on_async (uv_async_t *handle, int status) {
    AsyncDispatcher *self = static_cast<AsyncDispatcher*> (handle->data);
    self->callback_ ();
}

void AsyncDispatcher::emit () {
    uv_async_send (&handle_);
}
} /* appjs */
