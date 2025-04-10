#include "conet/base/log/appender.h"
#include "conet/base/log/console_appender.h"
#include "conet/base/log/file_appender.h"
#include "conet/base/log/logger.h"

namespace conet {
namespace log { 

std::shared_ptr<ConsoleAppender> LogAppender::consoleAppender() {
    return std::make_shared<ConsoleAppender>();
}

std::shared_ptr<FileAppender> LogAppender::fileAppender(const LogOption& option) {
    return std::make_shared<FileAppender>(option.getFileDir(), option.getFileName()
                                                                    , option.isRollByDay(), option.isAync()
                                                                    , option.getFlushInterval(), option.isSafe()
                                                                    , option.getRollSize(), option.getCheckEveryn());

}

} // namespace log
} // namespace conet