#ifndef CONET_EVENTLOOP_H
#define CONET_EVENTLOOP_H

#include "conet/net/poller.h"
#include "conet/net/timer_set.h"
#include "conet/base/coroutine/coroutine.h"

#include <atomic>
#include <vector>
#include <memory>
#include <mutex>

namespace conet {
namespace net {

class EventLoop {
public:
    using uptr = std::unique_ptr<EventLoop>;
    EventLoop();
    ~EventLoop();

    static EventLoop* getCurrentEventLoop();

    void loop();
    void stop();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void runCoroutine(const Coroutine::sptr& co);
    void queueInLoop(const Coroutine::sptr& co);
    void assertInLoopThread();

    // TODO: 确定时间？s，ms，us？目前是毫秒
    TimerId runAfter(int delay, const std::function<void()>& cb);
    TimerId runEvery(int interval, const std::function<void()>& cb);
    TimerId runAt(const Timestamp& time, const std::function<void()>& cb);

public:
    static EventLoop* getEventLoop();

private:
    void wakeup();
    void handleRead();

    bool isInLoopThread();
    void abortNotInLoopThread();

private:
    std::atomic_bool m_looping{false};
    Poller* m_poller{nullptr};
    int m_wakeup_fd{0};
    Channel::sptr m_wakeup_channel{nullptr};
    pid_t m_tid{-1};

    TimerSet::uptr m_timer_set{nullptr};

    std::mutex m_mutex;
    std::atomic_bool m_calling_co{false};
    std::vector<Coroutine::sptr> m_co_list;
};

} // namespace net
} // namespace conet

#endif