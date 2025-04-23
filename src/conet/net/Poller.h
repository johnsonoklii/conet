#ifndef CONET_POLLER_H
#define CONET_POLLER_H

#include <vector>

namespace conet {
namespace net {

class Channel;

class Poller {
public:
    virtual ~Poller();

    virtual void updateChannel(Channel* channel) = 0;
    virtual void poll(int timeoutMs, ChannelList* activeChannels) = 0;

    static Poller* createPoller();
};

using ChannelList = std::vector<Channel*>;

} // namespace net
} // namespace conet

#endif