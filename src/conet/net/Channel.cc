#include "conet/net/Channel.h"

namespace conet {
namespace net {

Channel::Channel(int fd): m_fd(fd) {
}

void Channel::handleEvent() {
    if (m_revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if (m_co) {
           Coroutine::resume(m_co);
        }
    }
}

ChannelManager& ChannelManager::getInstance() {
    static thread_local ChannelManager channel_manager;
    return channel_manager;
}

void ChannelManager::addChannel(Channel* channel) {
    m_channels[channel->fd()] = channel;
}

Channel* ChannelManager::getChannel(int fd) {
    if (m_channels.find(fd) != m_channels.end()) {
        return m_channels[fd];
    } else {
        return nullptr;
    }
}

} // namespace net
} // namespace conet