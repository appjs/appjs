#ifndef IMPL_CHANNEL_H
#define IMPL_CHANNEL_H

#include <memory>
#include <list>
#include <tuple>
#include <functional>
#include "impl_signal_wrap.h"

namespace appjs {
template<class MUTEX, class LOCK>
class Channel {
public:
    typedef std::function<void ()> Callback;
    typedef std::function<void ()> Task;
    typedef std::tuple<Task /* task */, Callback /* callback */> Job;
    typedef std::list<Job> JobsList;

    enum JOB_TYPE {
        NODE, GUI
    };

    Channel () {
    }

    // Move jobs list out
    template<JOB_TYPE type>
    JobsList move_jobs () {
        JobsList empty_list;
        {
        LOCK lock (get_mutex<type> ());
        get_jobs<type> ().swap (empty_list);
        }

        return std::move (empty_list);
    }

    template<JOB_TYPE type>
    std::unique_ptr<SignalBase>& get_signal () {
        return signal_[type];
    }

    template<JOB_TYPE type>
    JobsList& get_jobs () {
        return jobs_[type];
    }

    template<JOB_TYPE type>
    MUTEX& get_mutex () {
        return mutex_[type];
    }

    // Init channel
    template<JOB_TYPE type>
    void init (SignalBase *signal) {
        get_signal<type> ().reset (signal);
        emit<type> ();
    }

    // Push new job to list
    template<JOB_TYPE type>
    void push_job (Task&& task, Callback&& callback = Callback ()) {
        LOCK lock (get_mutex<type> ());
        get_jobs<type> ().push_back (std::move (
                    std::make_tuple (task, callback)));
    }

    // Notifies new jobs
    template<JOB_TYPE type>
    void emit () {
        // Check whether jobs list is empty
        bool empty;
        {
        LOCK lock (get_mutex<type> ());
        empty = get_jobs<type> ().empty ();
        }

        if (!empty && get_signal<type> ())
            get_signal<type> ()->emit ();
    }

private:
    std::unique_ptr<SignalBase> signal_[2];
    MUTEX mutex_[2];
    JobsList jobs_[2];

/* Not to be implemented */
private:
    Channel (const Channel&);
    Channel& operator= (const Channel&);

};
} /* appjs */

#endif /* end of IMPL_CHANNEL_H */
