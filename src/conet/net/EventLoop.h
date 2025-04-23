#ifndef CONET_EVENTLOOP_H
#define CONET_EVENTLOOP_H

#include "conet/net/Poller.h"

#include <atomic>

namespace conet {
namespace net {

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void stop();
    void updateChannel(Channel* channel);
private:
    std::atomic_bool m_looping{false};
    Poller* m_poller;
};

} // namespace net
} // namespace conet

#endif