#ifndef CONET_CHANNEL_H
#define CONET_CHANNEL_H

#include <functional>
#include <sys/epoll.h>

namespace conet {
namespace net {

using ReadCallback = std::function<void()>;
using WriteCallback = std::function<void()>;
using CloseCallback = std::function<void()>;

class Channel {
public:
    Channel(int fd);

    void handleEvent();

    void enableRead() { m_events |= EPOLLIN; }
    void disableRead() { m_events &= ~EPOLLIN; }
    void enableWrite() { m_events |= EPOLLOUT; }
    void disableWrite() { m_events &= ~EPOLLOUT; }
    void disableAll() { m_events = 0; }

    void setEvents(int events) { m_events = events; }
    int getEvents() const { return m_events; }
    void setRevents(int revents) { m_revents = revents; }
    int getRevents() const { return m_revents; }
    
private:
    int m_fd{-1};
    int m_revents{0};
    int m_events{0};
};

} // namespace net
} // namespace conet

#endif