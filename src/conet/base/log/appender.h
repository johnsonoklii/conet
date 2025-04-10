#ifndef CONET_APPENDER_H
#define CONET_APPENDER_H

#include <memory>

namespace conet {
namespace log {

class LogContext;
class LogOption;
class ConsoleAppender;
class FileAppender;

class LogAppender {
public:
    virtual ~LogAppender() = default;
    virtual void append(const LogContext& ctx) = 0;
    static std::shared_ptr<ConsoleAppender> consoleAppender();
    static std::shared_ptr<FileAppender> fileAppender(const LogOption& option);
};

using LogAppenderPtr = std::shared_ptr<LogAppender>;

} // namespace log
} // namespace conet

#endif