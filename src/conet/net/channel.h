#ifndef CONET_CHANNEL_H
#define CONET_CHANNEL_H

#include "conet/base/coroutine/coroutine.h"

#include <functional>
#include <unordered_map>

#include <sys/epoll.h>

namespace conet {
namespace net {

using ReadCallback = std::function<void()>;
using WriteCallback = std::function<void()>;
using CloseCallback = std::function<void()>;

enum {
    kNoneEvent = 0,
    kReadEvent = EPOLLIN | EPOLLPRI,
    kWriteEvent = EPOLLOUT,
};

class Channel {
public:
    using sptr = std::shared_ptr<Channel>;
    using ReadCallback = std::function<void()>;

    Channel(int fd);

    void handleEvent();

    void enableRead() { m_events |= kReadEvent; }
    void disableRead() { m_events &= ~kReadEvent; }
    void enableWrite() { m_events |= kWriteEvent; }
    void disableWrite() { m_events &= ~kWriteEvent; }
    void disableAll() { m_events = kNoneEvent; }
    bool isNoneEvent() const { return m_events == kNoneEvent; }

    int fd() const { return m_fd; }
    void setEvents(int events) { m_events = events; }
    int getEvents() const { return m_events; }
    void setRevents(int revents) { m_revents = revents; }
    int getRevents() const { return m_revents; }

    int index() const { return m_index; }
    void setIndex(int index) { m_index = index; }
    void setCoroutine(Coroutine::sptr co) { m_co = co; }

    void setReadCallback(ReadCallback cb) { m_read_cb = cb; }

private:
    int m_fd{-1};
    int m_revents{0};
    int m_events{0};
    int m_index{-1};
    
    Coroutine::sptr m_co;
    ReadCallback m_read_cb;
};

class ChannelManager {
public:
    static ChannelManager& getInstance();

    void addChannel(Channel* channel);
    Channel* getChannel(int fd);
private:
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap m_channels;
};

} // namespace net
} // namespace conet

#endif