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
    using uptr = std::unique_ptr<Acceptor>;
    Acceptor(EventLoop* loop, const InetAddress& listen_addr, bool reuse_port);

    int accept(InetAddress* peer_addr);
    InetAddress getListenAddr() const { return m_listen_addr; }

private:
    EventLoop* m_loop;
    InetAddress m_listen_addr;
    Socket m_accept_socket;
    Channel m_accept_channel;
    int m_idle_fd{-1};
};

} // namespace net
} // namespace conet

#endif