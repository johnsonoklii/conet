#include "conet/net/Channel.h"

namespace conet {
namespace net {

Channel::Channel(int fd): m_fd(fd) {
}

void Channel::handleEvent() {

}

} // namespace net
} // namespace conet