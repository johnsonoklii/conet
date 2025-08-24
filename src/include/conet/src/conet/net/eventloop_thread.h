#ifndef CONET_NET_EVENTLOOP_THREAD_H
#define CONET_NET_EVENTLOOP_THREAD_H

#include "conet/base/util/thread.h"
#include "conet/base/util/waitgroup.h"

namespace conet {
namespace net {

class EventLoop;

class EventLoopThread {
public:
    using uptr = std::unique_ptr<EventLoopThread>;
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    EventLoopThread(std::string& name, const ThreadInitCallback& thread_init_cb);
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void threadFunc();

private:
    EventLoop* m_loop{nullptr};
    Thread::uptr m_thread{nullptr};
    WaitGroup m_wg;

    ThreadInitCallback m_thread_init_cb;
};

} // namespace net
} // namespace conet

#endif