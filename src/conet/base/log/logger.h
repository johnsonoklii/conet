#ifndef CONET_LOGGER_H
#define CONET_LOGGER_H

#include "conet/base/log/appender.h"
#include "conet/base/log/formatter.h"

#include <cstring>

#include <unordered_map>
#include <string>

namespace conet {
namespace log { 

enum LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
};

struct LogOption {
    LogLevel m_level{INFO};
    std::string m_name{"conet"};   
    bool m_is_console{true};

    std::string m_file_dir;                 // should use "log/conet", not "log/conet/"
    std::string m_file_name;                // should use "filename", not "filename.log"
    bool m_roll_by_day{true};   
    bool m_is_aync{true};
    int m_flush_interval{3};                // 3s
    bool m_is_safe{false};
    size_t m_roll_size{1024 * 1024 * 64};   // 64MB
    int m_check_everyn{10000}; 

    LogFormatterPtr m_formatter;

    void setLevel(LogLevel level) { m_level = level;}
    LogLevel getLevel() const { return m_level; }
    void setName(const std::string& name) { m_name = name; }
    const std::string& getName() const { return m_name; }
    void setConsole(bool is_console) { m_is_console = is_console; }
    bool isConsole() const { return m_is_console; }
    void setFileDir(const std::string& dir) { m_file_dir = dir; }
    const std::string& getFileDir() const { return m_file_dir; }
    void setFileName(const std::string& name) { m_file_name = name; }
    const std::string& getFileName() const { return m_file_name; }
    void setRollByDay(bool roll_by_day) { m_roll_by_day = roll_by_day; }
    bool isRollByDay() const { return m_roll_by_day; }
    void setAync(bool aync) { m_is_aync = aync; }
    bool isAync() const { return m_is_aync; }
    void setSafe(bool safe) { m_is_safe = safe; }
    bool isSafe() const { return m_is_safe; }
    void setFlushInterval(int interval) { m_flush_interval = interval; }
    int getFlushInterval() const { return m_flush_interval; }
    void setRollSize(size_t size) { m_roll_size = size; }
    size_t getRollSize() const { return m_roll_size; }
    void setCheckEveryn(int everyn) { m_check_everyn = everyn; }
    int getCheckEveryn() const { return m_check_everyn; }
    void setFormatter(LogFormatterPtr formatter) { m_formatter = formatter; }
    LogFormatterPtr getFormatter() const { return m_formatter; }
};

/*
    option for global logger,
    before using logger, you need to set the option
*/ 
LogOption& GLOBAL_OPTION();

#define GLOB_LOG_OPTION conet::log::GLOBAL_OPTION()

/*
    not thread safe, you need init logger int main(), and then not change the option
*/
class Logger {
public:
    Logger(LogOption& option);
    ~Logger();

    static Logger& getInstance();

    void setOption(const LogOption& option) { m_option = option; }
    LogOption getOption() const { return m_option; }

    void addAppender(const std::string& name, LogAppenderPtr appender);
    
    void log(LogLevel level, const char* file_name, int line, const char* msg, size_t len);
private:
    Logger();
    void init();

private:
    LogOption m_option;  
    LogFormatterPtr m_formatter;
    std::unordered_map<std::string, LogAppenderPtr> m_appenders;
};

} // namespace log
} // namesapce conet

namespace conet {

#ifdef CONET_DEBUG
#define LOG_DEBUG(fmt, ...) \
    do { \
        GLOB_LOG_OPTION.setLevel(LogLevel::DEBUG); \
        char bufxxx159[1024]; \
        snprintf(bufxxx159, sizeof(bufxxx159), fmt, ##__VA_ARGS__); \
        Logger::getInstance().log(LogLevel::DEBUG, __FILE__, __LINE__, bufxxx159, strlen(bufxxx159)); \
    } while (0)
#else
#define LOG_DEBUG(fmt, ...)
#endif
    
#define LOG_INFO(fmt, ...) \
    do { \
        char bufxxx159[1024]; \
        snprintf(bufxxx159, sizeof(bufxxx159), fmt, ##__VA_ARGS__); \
        Logger::getInstance().log(LogLevel::INFO, __FILE__, __LINE__, bufxxx159, strlen(bufxxx159)); \
    } while (0)

#define LOG_WARN(fmt, ...) \
    do { \
        char bufxxx159[1024]; \
        snprintf(bufxxx159, sizeof(bufxxx159), fmt, ##__VA_ARGS__); \
        Logger::getInstance().log(LogLevel::WARN, __FILE__, __LINE__, bufxxx159, strlen(bufxxx159)); \
    } while (0)

#define LOG_ERROR(fmt, ...) \
    do { \
        char bufxxx159[1024]; \
        snprintf(bufxxx159, sizeof(bufxxx159), fmt, ##__VA_ARGS__); \
        Logger::getInstance().log(LogLevel::ERROR, __FILE__, __LINE__, bufxxx159, strlen(bufxxx159)); \
    } while (0)

#define LOG_FATAL(fmt, ...) \
    do { \
        char bufxxx159[1024]; \
        snprintf(bufxxx159, sizeof(bufxxx159), fmt, ##__VA_ARGS__); \
        Logger::getInstance().log(LogLevel::ERROR, __FILE__, __LINE__, bufxxx159, strlen(bufxxx159)); \
        exit(1); \
    } while (0)


}

#endif