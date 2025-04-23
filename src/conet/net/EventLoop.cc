#include "conet/net/EventLoop.h"

namespace conet {
namespace net {

constexpr int kPollTimeMs = 1000;

EventLoop::EventLoop()
: m_poller(Poller::createPoller()) {

}

EventLoop::~EventLoop() {
    m_looping.store(false);
    delete m_poller;
}

void EventLoop::stop() {
    m_looping.store(false);
}

void EventLoop::updateChannel(Channel* channel) {
    m_poller->updateChannel(channel);
}

void EventLoop::loop() {
    m_looping.store(true);

    ChannelList activeChannels;

    while (m_looping) {
        m_poller->poll(kPollTimeMs, &activeChannels);
        for (Channel* channel : activeChannels) {
            // TODO: channel回调函数
        }
    }
}

} // namespace net
} // namespace conet