#ifndef IMPL_ASYNC_WRAP_H
#define IMPL_ASYNC_WRAP_H

#include <node.h>
#include <functional>
#include "impl_signal_wrap.h"

// Wrap uv_async_t as a signal-style class

namespace appjs {
class AsyncDispatcher: public SignalBase {
public:
    typedef std::function<void ()> Callback;

    AsyncDispatcher (Callback callback);
    virtual void emit ();

private:
    uv_async_t handle_;
    Callback callback_;
    static void on_async (uv_async_t *handle, int status);
};
} /* appjs */

#endif /* end of IMPL_ASYNC_WRAP_H */
