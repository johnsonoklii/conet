#include "conet/base/log/text_formatter.h" 
#include "conet/base/log/log_context.h"
#include "conet/base/util/timestamp.h"

#include <cstring>

namespace conet {
namespace log {
std::string TextFormatter::format(const LogContext& ctx) {
    char buf[ctx.m_msg.size() + 512];
    snprintf(buf, sizeof(buf), "[%s][%s][%s:%d][tid:%d] %s\n"
                                        , Timestamp::getCurDateTime(true)
                                        , ctx.m_level
                                        , ctx.getShortFileName().c_str()
                                        , ctx.m_line
                                        , ctx.m_tid
                                        , ctx.m_msg.c_str());
    return std::string(buf, strlen(buf));                                  
}

} // namespace log
} // namespace conet