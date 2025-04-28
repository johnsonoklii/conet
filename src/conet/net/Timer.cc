#include "conet/net/Timer.h"

#include <atomic>

namespace conet {
namespace net {

static thread_local int64_t t_sequence{0};

Timer::Timer(int interval)
: m_interval(interval)
, m_expire_time(Timestamp::addTimeMilliSecond(Timestamp::now(), interval)) {

}

Timer::Timer(int interval, bool recursive, std::function<void()> callback)
: m_interval(interval)
, m_recursive(recursive)
, m_expire_time(Timestamp::addTimeMilliSecond(Timestamp::now(), interval))
, m_timer_id(this, ++t_sequence)
, m_callback(std::move(callback)) {

}

void Timer::run() {
    m_callback();
}

void Timer::restart(const Timestamp& now) {
    if (m_recursive) {
        m_expire_time = Timestamp::addTimeMilliSecond(now, m_interval);
    } else {
        m_expire_time = Timestamp::invalid();
    }
}
    
} // namespace net
} // namespace conet