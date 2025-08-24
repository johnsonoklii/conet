#ifndef CONET_NET_TIMER_H
#define CONET_NET_TIMER_H

#include "conet/base/util/timestamp.h"
#include "conet/base/log/logger.h"

#include <functional>
#include <memory>

namespace conet {
namespace net {

class Timer;

class TimerId {
public:
    TimerId() = default;
    TimerId(Timer* timer, int sequence)
        : m_timer(timer), m_sequence(sequence) {}

    Timer* timer() const { return m_timer; }
    int sequence() const { return m_sequence; }

    bool operator==(const TimerId& rhs) const {
        return m_timer == rhs.m_timer && m_sequence == rhs.m_sequence;
    }

private:
    Timer* m_timer{nullptr};
    int m_sequence{0};
};

class Timer {
public:
    using sptr = std::shared_ptr<Timer>;
    Timer() = default;
    Timer(int interval, bool recursive, std::function<void()> callback);
    ~Timer() {
        // LOG_DEBUG("~Timer(): %d", m_timer_id.sequence());
    }

    static Timer* getNowTimer() {
        return new Timer(0);
    }

    void run();

    Timestamp expireTime() const { return m_expire_time; }
    bool isRecursive() const { return m_recursive; }

    void restart(const Timestamp& now);
    TimerId timerId() const { return m_timer_id; }

    bool operator<(const Timer& rhs) const {
        return m_expire_time < rhs.m_expire_time;
    }

    bool operator==(const Timer& rhs) const {
        return m_timer_id == rhs.m_timer_id;
    }

    static bool compare(const Timer::sptr& lhs, const Timer::sptr& rhs) {
        return lhs->m_expire_time < rhs->m_expire_time;
    }

private:
    Timer(int interval);

private:
    double m_interval{0.0};
    bool m_recursive{false};
    Timestamp m_expire_time;
    TimerId m_timer_id;

    std::function<void()> m_callback;
};

} // namespace net
} // namespace conet

namespace std {
template<>
struct hash<conet::net::TimerId> {
    size_t operator()(const conet::net::TimerId& id) const {
        return std::hash<conet::net::Timer*>()(id.timer()) ^ 
                std::hash<int>()(id.sequence());
    }
};
}

#endif