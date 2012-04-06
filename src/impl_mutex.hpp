#ifndef IMPL_MUTEX_H
#define IMPL_MUTEX_H

#include <glib.h>

namespace appjs {
class Mutex {
public:
    Mutex () :
        handle_ (g_mutex_new ())
    { }

    ~Mutex () {
        g_mutex_free (handle_);
    }

    class Lock {
    public:
        Lock (Mutex &mutex) :
            mutex_ (mutex)
        {
            g_mutex_lock (mutex_.handle_);
        }

        ~Lock () {
            g_mutex_unlock (mutex_.handle_);
        }

    private:
        Mutex &mutex_;

    /* Not to be implemented */
    private:
        Lock (const Lock&);
        Lock& operator= (const Lock&);
    };

private:
    GMutex *handle_;

/* Not to be implemented */
private:
    Mutex (const Mutex&);
    Mutex& operator= (const Mutex&);
};

} /* appjs */

#endif /* end of IMPL_MUTEX_H */
