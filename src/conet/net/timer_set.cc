#include "conet/net/timer_set.h"
#include "conet/net/eventLoop.h"  
#include "conet/base/log/logger.h"  

#include <sys/timerfd.h>
#include <unistd.h>
#include <cassert>

namespace conet {
namespace net {

int createTimerfd() {
    int timer_fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timer_fd < 0) {
        LOG_FATAL("createTimerfd(): %s.", strerror(errno));
        return -1;
    }
    return timer_fd;
}

void resetTimerfd(int timer_fd, const Timestamp& when) {
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof(newValue));
    bzero(&oldValue, sizeof(oldValue));
    newValue.it_value = Timestamp::howMuchTimeFromNow(when);
    // newValue.it_interval
    
    if (0 != ::timerfd_settime(timer_fd, 0, &newValue, &oldValue)) {
        LOG_ERROR("resetTimerfd(): %s.", strerror(errno));
    }
}

void readTimerfd(int timer_fd) {
    uint64_t how_many;
    ssize_t n = ::read(timer_fd, &how_many, sizeof(how_many)); // FIXME: read hook
    if (n != sizeof(how_many)) {
        LOG_ERROR("readTimerfd(): %s.", strerror(errno));
    }
}

TimerSet::TimerSet(EventLoop* loop)
: m_loop(loop)
, m_timer_fd(createTimerfd())
, m_timer_channel(loop, m_timer_fd) {
    m_timer_channel.setReadCallback(std::bind(&TimerSet::handleRead, this));
    m_timer_channel.enableRead(nullptr);
}

TimerSet::~TimerSet() {
    m_loop->removeChannel(&m_timer_channel);
    ::close(m_timer_fd);
    m_timer_fd = -1;
    m_min_set.clear();
    m_timer_map.clear();
}

TimerId TimerSet::addTimer(const Timer::sptr& timer) {
    Coroutine::sptr co = std::make_shared<Coroutine>(kDefaultStackSize, std::bind(&TimerSet::addTimerInLoop, this, timer));
    m_loop->runCoroutine(co);
    return timer->timerId();
}

void TimerSet::addTimerInLoop(const Timer::sptr& timer) {
    m_loop->assertInLoopThread();
    bool is_update = false;
    if (m_min_set.empty()|| timer <  *m_min_set.begin()) {
        is_update = true;
    }

    m_min_set.insert(timer);
    m_timer_map[timer->timerId()] = timer;

    if (is_update) {
        resetTimerfd(m_timer_fd, timer->expireTime());
    }
    
    assert(m_min_set.size() == m_timer_map.size());
}

void TimerSet::removeTimer(const TimerId& timer_id) {
    Coroutine::sptr co = std::make_shared<Coroutine>(kDefaultStackSize, std::bind(&TimerSet::removeTimerInLoop, this, timer_id));
    m_loop->runCoroutine(co);
}

void TimerSet::removeTimerInLoop(const TimerId& timer_id) {
    m_loop->assertInLoopThread();

    auto map_it = m_timer_map.find(timer_id);
    if (map_it == m_timer_map.end()) {
        return;
    }

    auto set_it = m_min_set.find(map_it->second);
    if (set_it != m_min_set.end()) {
        m_min_set.erase(set_it);
        m_timer_map.erase(map_it);
    }

    assert(m_min_set.size() == m_timer_map.size());
}

std::vector<Timer::sptr> TimerSet::getExpiredTimers() {
    std::vector<Timer::sptr> expired_timers;
    if (m_min_set.empty()) {
        return expired_timers;
    }

    Timer::sptr now = Timer::sptr(Timer::getNowTimer());

    auto end = m_min_set.lower_bound(now);
    expired_timers.reserve(std::distance(m_min_set.begin(), end));
    expired_timers.assign(m_min_set.begin(), end);
    m_min_set.erase(m_min_set.begin(), end);
    
    for (auto& timer : expired_timers) {
        m_timer_map.erase(timer->timerId());
    }

    return expired_timers;
}

void TimerSet::runTimer(const Timer::sptr timer) {
    m_loop->assertInLoopThread();
    timer->run();
    if (timer->isRecursive()) {
        timer->restart(Timestamp::now());
        addTimer(timer);
    }
}

void TimerSet::handleRead() {
    m_loop->assertInLoopThread();
    readTimerfd(m_timer_fd);

    std::vector<Timer::sptr> expired_timers = getExpiredTimers();

    for (auto& timer : expired_timers) {
        // COMMENT: 需要加入loop的协程队列，防止timer中存在阻塞，饿死其他timer
        Coroutine::sptr co = std::make_shared<Coroutine>(kDefaultStackSize, std::bind(&TimerSet::runTimer, this, timer));
        m_loop->runCoroutine(co);
    }

    if (!m_min_set.empty()) {
        auto it = m_min_set.begin();
        resetTimerfd(m_timer_fd, (*it)->expireTime());
    }
}

} // namespace net
} // namespace conet