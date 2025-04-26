#include "conet/base/log/console_appender.h"
#include "conet/base/log/log_context.h"
#include "conet/base/util/util.h"

#include <cstdio>
#include <cstring>

namespace conet {
namespace log {

void ConsoleAppender::append(const LogContext& ctx)  {
    std::string&& msg = ctx.m_formatter->format(ctx);
    ::fwrite(msg.c_str(), sizeof(char), msg.size(), stdout);                     
}

} // namespace log
} // namespace conet