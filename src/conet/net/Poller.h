#ifndef CONET_POLLER_H
#define CONET_POLLER_H

#include "conet/net/Channel.h"

#include <vector>
#include <unordered_map>

namespace conet {
namespace net {

using ChannelList = std::vector<Channel*>;

class Poller {
public:
    virtual ~Poller() = default;

    virtual void updateChannel(Channel* channel) = 0;
    virtual void poll(int timeout_ms, ChannelList* active_channels) = 0;

    static Poller* createPoller();

protected:
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap m_channels;
};



} // namespace net
} // namespace conet

#endif