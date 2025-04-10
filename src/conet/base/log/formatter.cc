#include "conet/base/log/formatter.h"
#include "conet/base/log/text_formatter.h"

#include <memory>

namespace conet {
namespace log {
std::shared_ptr<LogFormatter> LogFormatter::textFormatter() {
    return std::make_shared<TextFormatter>();
}

} // namespace log
} // namespace conet