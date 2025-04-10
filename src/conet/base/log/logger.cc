#include "conet/base/log/logger.h"
#include "conet/base/log/console_appender.h"
#include "conet/base/log/file_appender.h"
#include "conet/base/log/log_context.h"
#include "conet/base/log/formatter.h"
#include "conet/base/util/timestamp.h"
#include "conet/base/util/util.h"
#include "conet/base/util/process.h"

#include <cstring>
#include <string_view>

namespace conet {
namespace log {

LogOption& GLOBAL_OPTION() {
    static LogOption option;
    return option;
}

const char* getLevelStr(LogLevel level) {
    switch (level) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARN:
            return "WARN";
        case ERROR:
            return "ERROR";
        case FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

Logger::Logger(): m_option(GLOB_LOG_OPTION) {
    init();
}

Logger::Logger(LogOption& option): m_option(option) {
    init();
}

// FIXME: 内存泄漏，FileAppender析构函数没有被调用
Logger::~Logger() {
}

void Logger::init() {
    if (m_option.isConsole()) {
        std::shared_ptr<ConsoleAppender> console_appender = std::make_shared<ConsoleAppender>();
        m_appenders["console"] = console_appender;
    }
    
    std::string file_dir = m_option.getFileDir();
    if (!file_dir.empty()) {
        std::shared_ptr<FileAppender> file_appender = LogAppender::fileAppender(m_option);
        file_appender->initAsyncLogging();   
        m_appenders["file"] = file_appender;                               
    }

    if (m_option.getFormatter() != nullptr) {
        m_formatter = m_option.getFormatter();
    } else {
        m_formatter = LogFormatter::textFormatter();
    }
}

Logger& Logger::getInstance() {
    static Logger logger;
    return logger;
}

void Logger::log(LogLevel level, const char* file_name, int line, const char* msg, size_t len) {
    if (level < m_option.getLevel()) return;

    LogContext ctx {
        m_option.getName().c_str(),
        getLevelStr(level),
        file_name,
        line,
        ProcessInfo::tid(),
        std::string(msg, len),
        m_formatter
    };

    for (auto& appender : m_appenders) {
        appender.second->append(ctx);
    }
}

void Logger::addAppender(const std::string& name, LogAppenderPtr appender) {
    m_appenders.emplace(name, appender);
}

} // namesapce log
} // namesapce conet