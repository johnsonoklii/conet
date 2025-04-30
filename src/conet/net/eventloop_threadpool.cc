#include "conet/net/eventloop_threadpool.h"
#include "conet/net/eventLoop.h"

#include <cassert>

namespace conet {
namespace net {

EventLoopThreadPool::EventLoopThreadPool(EventLoop* main_loop, const std::string& name)
: m_main_loop(main_loop), m_name(name) {
}

EventLoopThreadPool::~EventLoopThreadPool() {}

void EventLoopThreadPool::start(const EventLoopThread::ThreadInitCallback& cb) {
    assert(!m_started);
    m_main_loop->assertInLoopThread();

    m_started = true;

    if (m_thread_num <= 0) {
        if (cb) {
            cb(m_main_loop);
        }
        return;
    }

    for (int i = 0; i < m_thread_num; ++i) {
        char buf[m_name.size() + 32];
        snprintf(buf, sizeof(buf), "%s-%d", m_name.c_str(), i);
        std::string name(buf);

        EventLoopThread* t = new EventLoopThread(name, cb);
        m_threads.push_back(std::unique_ptr<EventLoopThread>(t));
        m_loops.push_back(t->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    assert(m_started);
    assert(m_main_loop);
    m_main_loop->assertInLoopThread();

    EventLoop* loop = m_main_loop;
    if (!m_loops.empty()) {
        loop = m_loops[count];
        count = (count + 1) % m_loops.size();
    }
    return loop;
}
EventLoop* EventLoopThreadPool::getLoopForHash(size_t hash_code) {
    assert(m_started);
    assert(m_main_loop);
    m_main_loop->assertInLoopThread();

    EventLoop* loop = m_main_loop;
    if (!m_loops.empty()) {
        loop = m_loops[hash_code % m_loops.size()];
    }
    return loop;
}

} // namespace net
} // namespace conet