#ifndef CONET_THREAD_H
#define CONET_THREAD_H

#include "conet/base/util/nocopyable.h"
#include "conet/base/util/waitgroup.h"

#include <pthread.h>

#include <functional>
#include <thread>
#include <string>

namespace conet {

class Thread : public nocopyable {
public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(const std::string& name, ThreadFunc&& func);
    ~Thread();

    void start();
    void join();

    bool started() const { return m_started; }
    bool joined() const { return m_joined; }
    pid_t tid() const { return m_tid; }

    std::string getName() const { return m_name; }

private:
    void setDefaultName();
    void runInThread();

private:
    bool m_started{false};
    bool m_joined{false};
    std::string m_name;
    pid_t m_tid;
    ThreadFunc m_func;
    std::thread m_thread;
    WaitGroup m_wait_group;
    static std::atomic_int m_num_created;
};

}

#endif