#include "conet/net/poller/Epoller.h"
#include "conet/net/Channel.h"
#include "conet/base/log/logger.h"

#include <unistd.h>
#include <cassert>

namespace conet {
namespace net {

static constexpr int kInitEventListSize = 16;

enum {
    kNew = -1,
    kAdded = 1,
    kDeleted = 2
};

Epoller::Epoller()
: m_events(kInitEventListSize) {
    m_efd = ::epoll_create1(EPOLL_CLOEXEC);
    if (m_efd == -1) {
        LOG_FATAL("Epoller::Epoller(): %s.", strerror(errno));
        return;
    }
}

Epoller::~Epoller() {
    if (valid()) {
        ::close(m_efd);
    }
}

void Epoller::removeChannel(Channel* channel) {
    int fd = channel->fd();
    if (m_channels.find(fd) == m_channels.end()) {
        return;
    }

    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    assert(m_channels[fd] == channel);

    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
    
    size_t n = m_channels.erase(fd);
    assert(n == 1);
}

void Epoller::updateChannel(Channel* channel) {
    if (!valid()) {
        LOG_FATAL("Epoller::updateChannel(): efd is invalid.");
        return;
    }
    if (!channel) {
        return;
    }

    int fd = channel->fd();
    const int index = channel->index();
    if (index == kNew || index == kDeleted) {
        if (index == kNew) {
            assert(m_channels.find(fd) == m_channels.end());
            m_channels[fd] = channel;
        } else {
            assert(m_channels.find(fd) != m_channels.end());
            assert(m_channels[fd] == channel);
        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        assert(m_channels.find(fd) != m_channels.end());
        assert(m_channels[fd] == channel);
        assert(index == kAdded);
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void Epoller::update(int operation, Channel*channel) {
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->getEvents();
    event.data.ptr = channel;
    int fd = channel->fd();
    if (::epoll_ctl(m_efd, operation, fd, &event) < 0) {
        LOG_FATAL("Epoller::update(): %s.", strerror(errno));
    }
}

void Epoller::poll(int timeout_ms, ChannelList* active_channels) {
    int n = ::epoll_wait(m_efd, m_events.data(), static_cast<int>(m_events.size()), timeout_ms);
    int saved_errno = errno;
    if (n < 0) {
        if (saved_errno != EINTR) {
            errno = saved_errno;
            LOG_FATAL("Epoller::poll(): %s.", strerror(saved_errno));
            return;
        }
    } else if (n == 0) {
        // nothing todo
    } else {
        for (int i = 0; i < n; ++i) {
            Channel* channel = static_cast<Channel*>(m_events[i].data.ptr);
            channel->setRevents(m_events[i].events);
            active_channels->push_back(channel);
        }

        if (n == (int)m_events.size()) {
            m_events.reserve(m_events.size() * 2);
        }
    }
}

} // namespace net
} // namespace conet