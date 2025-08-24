#ifndef CONET_CONSOLE_APPENDER_H
#define CONET_CONSOLE_APPENDER_H

#include "conet/base/log/appender.h"

#include <string>

namespace conet {
namespace log {
    
class ConsoleAppender : public LogAppender {
public:
    ConsoleAppender() = default;
    virtual ~ConsoleAppender() = default;
    
    virtual void append(const LogContext& ctx) override;
};

} // namespace log
} // namespace conet

#endif