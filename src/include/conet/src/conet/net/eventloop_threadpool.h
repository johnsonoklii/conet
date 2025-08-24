#ifndef CONET_NET_EVENTLOOP_THREADPOOL_H
#define CONET_NET_EVENTLOOP_THREADPOOL_H 

#include "conet/net/eventloop_thread.h"

#include <string>
#include <vector>
#include <functional>

namespace conet {
namespace net {

class EventLoopThread;
class EventLoop;

class EventLoopThreadPool {
public:
    using uptr = std::unique_ptr<EventLoopThreadPool>;
    EventLoopThreadPool(EventLoop* main_loop, const std::string& name);
    ~EventLoopThreadPool();

    void setThreadNum(int num) { m_thread_num = num; }
    void start(const EventLoopThread::ThreadInitCallback& cb);

    EventLoop* getNextLoop();
    EventLoop* getLoopForHash(size_t hash_code);

    std::vector<EventLoop*> getAllLoops() const { return m_loops; }

private:
    EventLoop* m_main_loop;
    int m_thread_num{0};
    std::string m_name;
    int count{0};
    bool m_started{false};
    std::vector<EventLoop*> m_loops;
    std::vector<EventLoopThread::uptr> m_threads;
};

} // namespace net
} // namespace conet

#endif