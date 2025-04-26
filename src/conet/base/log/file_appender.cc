#include "conet/base/log/file_appender.h"
#include "conet/base/util/process.h"
#include "conet/base/util/util.h"

#include <cstring>
#include <cassert>

#include <stdexcept>

namespace conet {
namespace log {

const int kSumLength = 1024;
thread_local char t_filename[kSumLength];

FileAppender::FileAppender(const std::string& file_dir
                        , const std::string& file_name
                        , bool m_roll_by_day
                        , bool is_async
                        , int flush_interval
                        , bool is_safe
                        , size_t roll_size
                        , int check_everyn)
: m_file_dir(file_dir)
, m_file_name(file_name)
, m_roll_by_day(m_roll_by_day)
, m_is_async(is_async)
, m_flush_interval(flush_interval)
, m_check_everyn(check_everyn)
, m_roll_size(roll_size)
, m_file(nullptr)
, m_mutex(is_safe ? new std::mutex() : nullptr) {
        
}

FileAppender::~FileAppender() {
    // must first stop async logging，because it will fwrite and flush before close
    if (m_async_logging) {
        delete m_async_logging;
    }
    close();
}

void FileAppender::initAsyncLogging() {
    if (m_is_async) {
        m_async_logging = new AsyncLogging(std::bind(&FileAppender::fwrite, this, _1, _2)
                                            , m_flush_interval);
        m_async_logging->start();
    }
}

void FileAppender::append(const LogContext& ctx) {
    if (m_is_async) {
        m_async_logging->pushMsg(ctx);
    } else {
        std::string&& msg = ctx.format();
        fwrite(msg.c_str(), msg.size());
    } 
}

void FileAppender::fwrite(const char* msg, size_t len) {
    if (m_mutex) {
        std::lock_guard<std::mutex> lock(*m_mutex);
        fwriteUnlocked(msg, len);
    } else {
        fwriteUnlocked(msg, len);
    }
}

void FileAppender::fflush() {
    if (m_mutex) {
        std::lock_guard<std::mutex> lock(*m_mutex);
        fflushUnlocked();
    } else {
        fflushUnlocked();
    }
}

void FileAppender::fwriteUnlocked(const char* msg, size_t len) {
    if (!m_file) {
        rollFile();
    }
    
    time_t now = ::time(nullptr);
    if (m_roll_by_day) {
        rollFileByDay(now);
    }
    
    write(msg, len);
    rollFileBySize();
    check(now);
}

void FileAppender::fflushUnlocked(const time_t* cache_now) {
    if (m_file) {
        time_t now;
        if (cache_now != nullptr) { now = *cache_now; } 
        else { now = ::time(nullptr); }

        ::fflush(m_file);
        m_last_flush = now;
    }
}

void FileAppender::write(const char* msg, size_t len) {
    size_t written = 0;
    while (written < len) {
        size_t remain = len - written;
        size_t n = 0;
        n = ::fwrite_unlocked(msg + written, 1, remain, m_file);
        if (n != remain) {
            int err = ferror(m_file);
            if (err) {
                fprintf(stderr, "FileAppender::write() failed %s\n", Util::getErrInfo(err));
                break;
            }
            if (n == 0) { throw std::runtime_error("write failed, FILE* is null\n"); }
        }
        written += n;
    }

    m_writen_bytes += written;
}

void FileAppender::rollFile(const time_t* cache_now) {
    time_t now;
    if (cache_now != nullptr) { now = *cache_now; } 
    else { now = ::time(nullptr); }

    if (now >= m_last_roll) {    
        const char* filename = getLogFileName(now);
        mkNewFile(filename);

        long start = now / kRollPerSeconds * kRollPerSeconds;  // 更新天的数据
        m_last_roll   = now;
        m_last_flush  = now;
        m_last_period = start;
        m_roll_count++;
    }
}

void FileAppender::rollFileByDay(time_t& now) {
    time_t cur_period = now / kRollPerSeconds * kRollPerSeconds;
    if (cur_period != m_last_period) {
        rollFile(&now);
    } 
}

void FileAppender::rollFileBySize() {
    if (m_writen_bytes > m_roll_size) {
        rollFile();
        resetWritten();
    } 
}

void FileAppender::check(time_t& now) {
    ++m_count;
    if (m_count >= m_check_everyn) {
        m_count = 0;
        if (now - m_last_flush >= m_flush_interval) {
            fflushUnlocked(&now);
        }
    }
}

void FileAppender::mkNewFile(const char* file_name) {
    close();
    Util::mkDir(m_file_dir);
    std::string file_path = m_file_dir + "/" + std::string(file_name);
    m_file = fopen(file_path.c_str(), "a");
}

void FileAppender::close() {
    if (m_file) {
        if (::fflush(m_file) == 0) {
            fclose(m_file);
        } else {
            fprintf(stderr, "Error flushing file\n");
        }
    } 
}

const char* FileAppender::getLogFileName(time_t& now) {
    size_t base_sz = m_file_name.size();
    char* filename = t_filename;

    strcpy(t_filename, m_file_name.c_str());

    size_t tsz = strftime(t_filename + base_sz, sizeof(t_filename) - base_sz, ".%Y%m%d-%H%M%S", localtime(&now));
    snprintf(t_filename + base_sz + tsz, sizeof(t_filename) - base_sz - tsz, ".%s.%d.%d.log", ProcessInfo::getHostName(),
                              static_cast<int>(ProcessInfo::tid()), m_roll_count);

    return filename;
}

} // namespace log
} // namespace conet