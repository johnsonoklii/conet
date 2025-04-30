#include "conet/net/eventloop_thread.h"
#include "conet/net/eventLoop.h"

#include <cassert>

namespace conet {
namespace net {
    
EventLoopThread::EventLoopThread(std::string& name, const ThreadInitCallback& thread_init_cb)
: m_thread(new Thread(name, std::bind(&EventLoopThread::threadFunc, this)))
, m_wg(1)
, m_thread_init_cb(thread_init_cb) {
}

EventLoopThread::~EventLoopThread() {
    if (m_thread->started()) {
        m_loop->stop();
        m_thread->join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!m_thread->started());

    m_thread->start();
    m_wg.wait();
    assert(m_loop);

    return m_loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;

    if (m_thread_init_cb) {
        m_thread_init_cb(&loop);
    }

    m_loop = &loop;
    m_wg.done();
    loop.loop();
}

} // namespace net
} // namespace conet