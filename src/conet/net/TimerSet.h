#ifndef CONET_NET_TIMERSET_H
#define CONET_NET_TIMERSET_H

#include "conet/net/Timer.h"
#include "conet/net/Channel.h"

#include <set>
#include <vector>
#include <unordered_map>
#include <memory>

namespace conet {
namespace net {

class EventLoop;

class TimerSet {
public:
    using uptr = std::unique_ptr<TimerSet>;
    TimerSet() = default;
    TimerSet(EventLoop* loop);
    ~TimerSet();

    TimerId addTimer(const Timer::sptr& timer);
    void removeTimer(const TimerId& timer_id);

private:
    std::vector<Timer::sptr> getExpiredTimers();
    void handleRead();
    void runTimer(const Timer::sptr timer);
    void addTimerInLoop(const Timer::sptr& timer);
    void removeTimerInLoop(const TimerId& timer_id);

private:
    struct TimerComparator {
        bool operator()(const Timer::sptr& lhs, const Timer::sptr& rhs) const {
            return Timer::compare(lhs, rhs);
        }
    };

    using Entry = std::pair<TimerId, Timer::sptr>;
    using TimerMinSet = std::set<Timer::sptr, TimerComparator>;
    using TimerMap = std::unordered_map<TimerId, Timer::sptr>;

    EventLoop* m_loop{nullptr};
    TimerMinSet m_min_set;
    TimerMap m_timer_map;
    int m_timer_fd{-1};
    Channel m_timer_channel;
};

} // namespace net
} // namespace conet

#endif