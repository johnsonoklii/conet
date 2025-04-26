#ifndef CONET_LOG_CONTEXT_H
#define CONET_LOG_CONTEXT_H

#include "conet/base/log/formatter.h"

#include <sys/types.h>

#include <string>
#include <memory>

namespace conet {
namespace log {

struct LogContext {
    const char* m_name{nullptr};
    const char* m_level{nullptr};
    const char* m_file_name{nullptr};
    std::string m_short_file_name;
    int m_line{0};
    pid_t m_tid{-1};
    std::string m_msg;
    std::shared_ptr<LogFormatter> m_formatter{nullptr};

    LogContext() {
    }

    LogContext( const char* name, const char* level, const char* file_name, int line, pid_t tid, std::string&& msg, std::shared_ptr<LogFormatter> formatter)
    : m_name(name), m_level(level), m_file_name(file_name), m_line(line), m_tid(tid), m_msg(std::move(msg)), m_formatter(formatter) {
    }

    LogContext(const LogContext& other) {
        if (this != &other) {
            m_name = other.m_name;
            m_level = other.m_level;
            m_file_name = other.m_file_name;
            m_short_file_name = other.m_short_file_name;
            m_line = other.m_line;
            m_tid = other.m_tid;
            m_msg = other.m_msg;
            m_formatter = other.m_formatter;
        }
    }

    LogContext(LogContext&& other) {
        if (this != &other) {
            m_name = other.m_name;
            m_level = other.m_level;
            m_file_name = std::move(other.m_file_name);
            m_short_file_name = std::move(other.m_short_file_name);
            m_line = other.m_line;
            m_tid = other.m_tid;
            m_msg = std::move(other.m_msg);
            m_formatter = other.m_formatter;
        }
    }

    LogContext& operator=(LogContext&& other) {
        if (this != &other) {
            m_name = other.m_name;
            m_level = other.m_level;
            m_file_name = std::move(other.m_file_name);
            m_short_file_name = std::move(other.m_short_file_name);
            m_line = other.m_line;
            m_tid = other.m_tid;
            m_msg = std::move(other.m_msg);
            m_formatter = other.m_formatter;
        }
       return *this;
    }

    std::string format() const {
        return m_formatter->format(*this);
    }

    std::string getShortFileName() const {
        std::string file_name = m_file_name;
        size_t last_slash_pos = file_name.find_last_of("/\\");
        if (last_slash_pos == std::string::npos) {
            return file_name;
        }
        return file_name.substr(last_slash_pos + 1);
    }
};

} // namespace log
} // namespace conet

#endif