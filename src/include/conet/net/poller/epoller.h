#ifndef CONET_EPOLLER_H
#define CONET_EPOLLER_H

#include "conet/net/poller.h"

#include <vector>
#include <sys/epoll.h>

namespace conet {
namespace net {

class Epoller : public Poller {
public:
    Epoller();
    virtual ~Epoller();

    virtual void poll(int timeout_ms, ChannelList* active_channels);
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    bool valid() const { return m_efd > 0; }

private:
    void update(int operation, Channel* channel);

private:
    int m_efd{-1};
    std::vector<struct epoll_event> m_events;
};

} // namespace net
} // namespace conet

#endif