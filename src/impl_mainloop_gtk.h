#ifndef IMPL_MAIN_GTK_H
#define IMPL_MAIN_GTK_H

#include "impl_mutex.hpp"
#include "impl_channel_gtk.hpp"

namespace appjs {
    class MainLoop {
    public:
        // Only the node thread can access it!!
        static MainLoop *get ();

        // Used by node and gui thread to transfer jobs
        typedef Channel<Mutex, Mutex::Lock> MyChannel;
        std::unique_ptr<MyChannel> channel;

        // Helper for Channel::push_job
        static void push_job_gui (MyChannel::Task&& t) {
            get ()->channel->push_job<MyChannel::GUI> (
                    std::forward<MyChannel::Task> (t));
            get ()->channel->emit<MyChannel::GUI> ();
        }
        static void push_job_node (MyChannel::Task&& t) {
            get ()->channel->push_job<MyChannel::NODE> (
                    std::forward<MyChannel::Task> (t));
            get ()->channel->emit<MyChannel::NODE> ();
        }

    private:
        MainLoop ();

        static MainLoop* self;
        static void* main (void *data);

        template<MyChannel::JOB_TYPE>
        void do_jobs ();

    /* Not to be implemented */
    private:
        MainLoop (const MainLoop&);
        MainLoop& operator= (const MainLoop&);

    };
} /* appjs */

#endif /* end of IMPL_MAIN_GTK_H */
