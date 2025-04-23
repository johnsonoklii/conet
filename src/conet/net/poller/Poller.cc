#include "conet/net/Poller.h"
#include "conet/net/poller/Epoller.h"

namespace conet {
namespace net {
static Poller* createPoller() {
    return new Epoller();
}

} // namespace net
} // namespace conet