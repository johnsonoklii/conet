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

class EventLoop;

class Channel {
public:
    using sptr = std::shared_ptr<Channel>;
    using ReadCallback = std::function<void()>;

    Channel(EventLoop* loop, int fd);

    void handleEvent();

    void enableRead(const Coroutine::sptr& co) { m_events |= kReadEvent; setReadCoroutine(co); update();}
    void disableRead() { m_events &= ~kReadEvent; update();}
    void enableWrite(const Coroutine::sptr& co) { m_events |= kWriteEvent; setWriteCoroutine(co); update();}
    void disableWrite() { m_events &= ~kWriteEvent; update();}
    void disableAll() { m_events = kNoneEvent; update();}
    bool isNoneEvent() const { return m_events == kNoneEvent; }

    int fd() const { return m_fd; }
    void setEvents(int events) { m_events = events; }
    int getEvents() const { return m_events; }
    void setRevents(int revents) { m_revents = revents; }
    int getRevents() const { return m_revents; }

    int index() const { return m_index; }
    void setIndex(int index) { m_index = index; }
    void setReadCoroutine(const Coroutine::sptr& co) { m_read_co = co; }
    void setWriteCoroutine(const Coroutine::sptr& co) { m_write_co = co; }

    void setReadCallback(ReadCallback cb) { m_read_cb = cb; }

    bool isWriting() const { return m_events & kWriteEvent; }

private:
    void update();

private:
    EventLoop* m_loop;
    int m_fd{-1};
    int m_revents{0};
    int m_events{0};
    int m_index{-1};
    
    // COMMENT: 如果注册的函数中存在IO操作，注册协程；否则可以注册回调函数
    Coroutine::sptr m_read_co;
    Coroutine::sptr m_write_co;
    ReadCallback m_read_cb;
};

class ChannelManager {
public:
    static ChannelManager& getInstance();

    void addChannel(Channel* channel);
    void removeChannel(Channel* channel);
    Channel* getChannel(int fd);
private:
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap m_channels;
};

} // namespace net
} // namespace conet

#endif