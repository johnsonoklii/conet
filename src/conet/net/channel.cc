#include "conet/net/channel.h"
#include "conet/net/eventLoop.h"
#include "conet/base/log/logger.h"

namespace conet {
namespace net {

Channel::Channel(EventLoop* loop, int fd)
: m_loop(loop)
, m_fd(fd) {
}

void Channel::handleEvent() {
    // FIXME: error, write事件
    if (m_revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if (m_read_co) {
            Coroutine::resume(m_read_co);
            if (m_read_co->getState() == Coroutine::kFinished) {
                m_read_co.reset();
            }
        }
        if (m_read_cb) {
            m_read_cb();
        }
    } else if (m_revents & EPOLLOUT) {
        if (m_write_co) {
            Coroutine::resume(m_write_co);
            if (m_write_co->getState() == Coroutine::kFinished) {
                m_write_co.reset();
            }
        }
    }
}

void Channel::update() {
    m_loop->updateChannel(this);
}

ChannelManager& ChannelManager::getInstance() {
    static thread_local ChannelManager channel_manager;
    return channel_manager;
}

void ChannelManager::addChannel(Channel* channel) {
    m_channels[channel->fd()] = channel;
}

void ChannelManager::removeChannel(Channel* channel) {
    m_channels.erase(channel->fd());
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