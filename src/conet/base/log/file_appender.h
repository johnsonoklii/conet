#ifndef CONET_FILE_APPENDER_H
#define CONET_FILE_APPENDER_H

#include "conet/base/log/appender.h"
#include "conet/base/log/async_logging.h"

#include <string>
#include <memory>

namespace conet {
namespace log {

class FileAppender : public LogAppender {
public:
    FileAppender(const std::string& file_dir
                , const std::string& file_name
                , bool m_roll_by_day
                , bool is_async
                , int flush_interval
                , bool is_safe
                , size_t roll_size
                , int check_everyn);
    virtual ~FileAppender();

    virtual void append(const LogContext& ctx) override;

    void initAsyncLogging();

    void fwrite(const char* msg, size_t len);
    void fflush();

private:
    enum { kRollPerSeconds = 60 * 60 * 24 };

    void fwriteUnlocked(const char* msg, size_t len);
    void fflushUnlocked(const time_t* cache_now = nullptr);
    void write(const char* msg, size_t len);

    void rollFile(const time_t* cache_now = nullptr);
    void rollFileByDay(time_t& now);
    void rollFileBySize();
    void check(time_t& now);

    void resetWritten() { m_writen_bytes = 0; }

    void mkNewFile(const char* file_name);

    void close();

    const char* getLogFileName(time_t& now);
    
private:
    std::string m_file_dir;
    std::string m_file_name;
    bool m_roll_by_day{false};
    bool m_is_async{false};
    int m_flush_interval{0};
    size_t m_writen_bytes{0};
    int m_count{0};                // 统计log次数
    int m_roll_count{0};           // 统计roll次数
    int m_check_everyn{0};
    size_t m_roll_size{0};
    time_t m_last_period{0};
    time_t m_last_roll{0};
    time_t m_last_flush{0};
    
    FILE* m_file{nullptr};
    AsyncLogging* m_async_logging{nullptr};
    std::unique_ptr<std::mutex> m_mutex{nullptr};
};

} // namespace log
} // namespace conet

#endif