#ifndef CONET_LOG_FORMATER_H
#define CONET_LOG_FORMATER_H

#include <string>
#include <memory>

namespace conet {
namespace log {

class LogContext;

class LogFormatter {
public:
    LogFormatter() = default;
    virtual ~LogFormatter() = default;

    virtual std::string format(const LogContext& ctx) = 0;

    static std::shared_ptr<LogFormatter> textFormatter();
};

using LogFormatterPtr = std::shared_ptr<LogFormatter>;

} // namespace log
} // namespace conet

#endif