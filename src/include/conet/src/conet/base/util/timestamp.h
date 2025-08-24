#ifndef LEMON_TIMESTAMP_H
#define LEMON_TIMESTAMP_H

#include <ctime>

#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>

namespace conet {

class Timestamp {
public:
    static const int kMicroSecondsPerSecond = 1000 * 1000;

    Timestamp();
    Timestamp(int64_t microseconds);
    static Timestamp now();
    static Timestamp addTimeSecond(const Timestamp& timestamp, double microseconds);
    static Timestamp addTimeMilliSecond(const Timestamp& timestamp, double microseconds);
    static Timestamp addTimeMicroSecond(const Timestamp& timestamp, double microseconds);
    static Timestamp invalid();
    static struct timespec howMuchTimeFromNow(const Timestamp& when) {
        int64_t microseconds = when.microSecondsSinceEpoch()
                                - Timestamp::now().microSecondsSinceEpoch();
        if (microseconds < 100) {
            microseconds = 100;
        }
        struct timespec ts;
        ts.tv_sec = static_cast<time_t>(
            microseconds / Timestamp::kMicroSecondsPerSecond);
        ts.tv_nsec = static_cast<long>(
            (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
        return ts;
    }
    

    std::string toString() const;
    bool isValid() const {  return m_system_timePoint.time_since_epoch().count() > 0; }   

    int64_t microSecondsSinceEpoch() const;
    int64_t milliSecondsSinceEpoch() const;
    int64_t secondsSinceEpoch() const;

    bool operator<(const Timestamp& rhs) const {
        return m_system_timePoint < rhs.m_system_timePoint;
    }

    bool operator==(const Timestamp& rhs) const {
        return m_system_timePoint == rhs.m_system_timePoint;
    }

    // util
    static const char* getCurDateTime(bool isTime, time_t* now = nullptr);

private:
    Timestamp(const std::chrono::system_clock::time_point& timePoint);

    std::chrono::system_clock::time_point m_system_timePoint;
};

} // namespace lemon

#endif