#include "conet/net/poller/Epoller.h"
#include "conet/net/Channel.h"
#include "conet/base/log/logger.h"

#include <unistd.h>

using namespace conet::log;

namespace conet {
namespace net {

Epoller::Epoller() {
    m_efd = ::epoll_create1(EPOLL_CLOEXEC);
    if (m_efd == -1) {
        LOG_FATAL("Epoller::Epoller() fatal: %s.", strerror(errno));
        return;
    }
    m_events.reserve(16);
}

Epoller::~Epoller() {
    if (valid()) {
        ::close(m_efd);
    }
}

void Epoller::updateChannel(Channel* channel) {
    if (!valid()) {
        LOG_FATAL("Epoller::updateChannel() fatal: efd is invalid.");
    }
    if (!channel) {
        return;
    }
    // TODO: 注册channel上的事件
}

void Epoller::poll(int timeoutMs, ChannelList* activeChannels) {
    if (::epoll_wait(m_efd, m_events.data(), m_events.size(), timeoutMs) != -1 ) {
        for (int i = 0; i < m_events.size(); ++i) {
            Channel* channel = static_cast<Channel*>(m_events[i].data.ptr);
            channel->setRevents(m_events[i].events);
            activeChannels->push_back(channel);
        }

        if (m_events.size() == m_events.capacity()) {
            m_events.reserve(m_events.size() * 2);
        }
    }
}

} // namespace net
} // namespace conet