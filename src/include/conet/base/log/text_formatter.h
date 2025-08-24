#ifndef CONET_LOG_TEXT_FORMATTER_H
#define CONET_LOG_TEXT_FORMATTER_H

#include "conet/base/log/formatter.h"

namespace conet {
namespace log {

class TextFormatter: public LogFormatter {
public:
    virtual ~TextFormatter() = default;
    virtual std::string format(const LogContext& ctx) override;
};

} // namespace log
} // namespace conet

#endif