#include "conet/base/util/timestamp.h"
#include <cassert>

namespace conet {

thread_local char           t_time[64];
thread_local int            t_timezone = -1;
thread_local struct std::tm t_tm;
thread_local struct std::tm t_gmtm;
thread_local time_t         t_lastSecond;

Timestamp::Timestamp() 
: m_system_timePoint() {
}

Timestamp::Timestamp(int64_t microseconds) {
    m_system_timePoint = std::chrono::system_clock::time_point(std::chrono::microseconds(microseconds));
}

Timestamp::Timestamp(const std::chrono::system_clock::time_point& timePoint) 
: m_system_timePoint(timePoint) {
}

Timestamp Timestamp::now() {
    return Timestamp(std::chrono::system_clock::now());
}

Timestamp Timestamp::addTimeSecond(const Timestamp& timestamp, double second) {
    int64_t microseconds = second * kMicroSecondsPerSecond;
    return Timestamp(timestamp.m_system_timePoint + std::chrono::microseconds(static_cast<long long>(microseconds)));
}

Timestamp Timestamp::addTimeMilliSecond(const Timestamp& timestamp, double millisecond) {
    int64_t microseconds = millisecond * 1000;
    return Timestamp(timestamp.m_system_timePoint + std::chrono::microseconds(static_cast<long long>(microseconds)));
}

Timestamp Timestamp::addTimeMicroSecond(const Timestamp& timestamp, double microsecond) {
    return Timestamp(timestamp.m_system_timePoint + std::chrono::microseconds(static_cast<long long>(microsecond)));
}

Timestamp Timestamp::invalid() {
    return Timestamp();
}

std::string Timestamp::toString() const {
    std::time_t time = std::chrono::system_clock::to_time_t(m_system_timePoint);
    std::tm bt = *std::localtime(&time);

    char buffer[32] = {0};
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &bt);
    return buffer;
}

int64_t Timestamp::microSecondsSinceEpoch() const {
    return std::chrono::duration_cast<std::chrono::microseconds>(m_system_timePoint.time_since_epoch()).count();
}

int64_t Timestamp::milliSecondsSinceEpoch() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_system_timePoint.time_since_epoch()).count();
}

int64_t Timestamp::secondsSinceEpoch() const {
    return std::chrono::duration_cast<std::chrono::seconds>(m_system_timePoint.time_since_epoch()).count();
}

// util
const char* Timestamp::getCurDateTime(bool is_time, time_t* now) {
   time_t timer = time(nullptr);
   if (now != nullptr) {   // to reduce system call
        *now = timer;
   }

   if (t_lastSecond != timer) {
        t_lastSecond = timer;
        ::localtime_r(&timer,&t_tm);
   }
   // to subtract gmtime and localtime for obtain timezone
   if (t_timezone == -1) {
        t_timezone = t_tm.tm_gmtoff / 3600;;
   }
   int len;
   if (is_time) {
        len = std::snprintf(t_time, sizeof(t_time),
                            "%4d-%02d-%02d %02d:%02d:%02d +%02d",
                            t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday,
                            t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec, t_timezone);
        assert(len == 23);
   }
   else {
        len = std::snprintf(t_time, sizeof(t_time), "%4d-%02d-%02d",
                            t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday);
        assert(len == 10);
   }
   (void)len;
   return t_time;
}

} // namespace conet