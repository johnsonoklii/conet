#include "conet/net/poller.h"
#include "conet/net/poller/epoller.h"

namespace conet {
namespace net {
Poller* Poller::createPoller() {
    return new Epoller();
}

} // namespace net
} // namespace conet