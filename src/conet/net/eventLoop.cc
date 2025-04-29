#include "conet/net/eventLoop.h"
#include "conet/net/channel.h"
#include "conet/base/log/logger.h"
#include "conet/base/util/process.h"

#include <sys/eventfd.h>
#include <unistd.h>

namespace conet {
namespace net {

static constexpr int kPollTimeMs = 1000 * 60; // 1min

static thread_local EventLoop* t_event_loop = nullptr;

int createEventfd() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    LOG_FATAL("%s.", strerror(errno));
  }
  return evtfd;
}

EventLoop* EventLoop::getEventLoop() {
    if (t_event_loop == nullptr) {
        t_event_loop = new EventLoop();
    }
    return t_event_loop;
}

EventLoop::EventLoop() {
    if (t_event_loop) {
        LOG_FATAL("EventLoop::EventLoop(): this thread has already create a eventloop.");
        return;
    }

    m_tid = ProcessInfo::tid();

    m_poller = Poller::createPoller();
    
    m_wakeup_fd = createEventfd();
    m_wakeup_channel = std::make_shared<Channel>(this, m_wakeup_fd);
    m_wakeup_channel->enableRead();

    t_event_loop = this;
    m_timer_set.reset(new TimerSet(this));
}

EventLoop::~EventLoop() {
    m_looping.store(false);
    delete m_poller;
}

void EventLoop::stop() {
    m_looping.store(false);
}

void EventLoop::updateChannel(Channel* channel) {
    assertInLoopThread();
    m_poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
    assertInLoopThread();
    m_poller->removeChannel(channel);
}

void EventLoop::runCoroutine(const Coroutine::sptr& co) {
    if (isInLoopThread() && Coroutine::isMainCoroutine()) {
        // LOG_DEBUG("EventLoop::runCoroutineInLoop(): running in main coroutine");
        Coroutine::resume(co);
        // LOG_DEBUG("EventLoop::runCoroutineInLoop(): end..");
    } else {
        queueInLoop(co);
    }
}

void EventLoop::queueInLoop(const Coroutine::sptr& co) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_co_list.push_back(co);
    }
    
    if (!isInLoopThread() || m_calling_co.load()) {
        wakeup();
    }
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(m_wakeup_fd, &one, sizeof one);
    if (n != sizeof(one)) {
        LOG_ERROR("EventLoop::wakeup(): writes %lu bytes instead of 8", n);
    }
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = ::read(m_wakeup_fd, &one, sizeof one);
    if (n != sizeof one) {
        LOG_DEBUG("EventLoop::handleRead(): read %lu bytes instead of 8", n);
    }
}

bool EventLoop::isInLoopThread() {
    return m_tid == ProcessInfo::tid();
}

void EventLoop::assertInLoopThread() {
    if (!isInLoopThread()) {
        abortNotInLoopThread();
    }
}

void EventLoop::abortNotInLoopThread() {
    LOG_FATAL("EventLoop::abortNotInLoopThread(): EventLoop is created in thread %d, current thread is %d", m_tid, ProcessInfo::tid());
}

void EventLoop::loop() {
    m_looping.store(true);

    ChannelList active_channels;

    while (m_looping) {
        active_channels.clear();
        m_poller->poll(kPollTimeMs, &active_channels);

        m_calling_co.store(true);
        for (Channel* channel : active_channels) {
            if (channel->fd() == m_wakeup_fd) {
                handleRead();
            } else {
                channel->handleEvent();
            }
        }
        
        std::vector<Coroutine::sptr> tmp_co_list;
        
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            tmp_co_list.swap(m_co_list);
        }

        // COMMENT: co如果在运行过程中yield前, 需要先将co加入loop或注册事件，否则就无法再恢复
        for (auto& co : tmp_co_list) {
            Coroutine::resume(co);
        }
        m_calling_co.store(false);
    }
}

TimerId EventLoop::runAfter(int delay, const std::function<void()>& cb) {
    return m_timer_set->addTimer(std::make_shared<Timer>(delay, false, cb));
}

TimerId EventLoop::runEvery(int interval, const std::function<void()>& cb) {
    return m_timer_set->addTimer(std::make_shared<Timer>(interval, true, cb));
}

TimerId EventLoop::runAt(const Timestamp& time, const std::function<void()>& cb) {
    double interval = time.milliSecondsSinceEpoch() - Timestamp::now().milliSecondsSinceEpoch();
    return runAfter(interval, cb);
}

} // namespace net
} // namespace conet