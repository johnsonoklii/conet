#ifndef CONET_ACCEPTOR_H
#define CONET_ACCEPTOR_H

#include "conet/net/Socket.h"
#include "conet/net/Channel.h"

namespace conet {
namespace net {

class EventLoop;
class InetAddress;

class Acceptor {
public:
    Acceptor(EventLoop* loop, const InetAddress& listenAddr);

    void listen();
    int accept();

    void stop() { m_listening = false; }

private:
    EventLoop* m_loop;
    InetAddress m_listenAddr;
    Socket m_acceptSocket;
    Channel m_acceptChannel;
    bool m_listening;
};

} // namespace net
} // namespace conet

#endif