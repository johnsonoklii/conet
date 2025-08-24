#ifndef CONET_LOG_FORMATER_H
#define CONET_LOG_FORMATER_H

#include <string>
#include <memory>

namespace conet {
namespace log {

class LogContext;

class LogFormatter {
public:
    using sptr = std::shared_ptr<LogFormatter>;
    LogFormatter() = default;
    virtual ~LogFormatter() = default;

    virtual std::string format(const LogContext& ctx) = 0;

    static std::shared_ptr<LogFormatter> textFormatter();
};



} // namespace log
} // namespace conet

#endif