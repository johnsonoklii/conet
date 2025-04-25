#ifndef CONET_NET_TCP_SERVER_H
#define CONET_NET_TCP_SERVER_H

#include "conet/net/EventLoop.h"
#include "conet/net/Acceptor.h"

namespace conet {
namespace net {

class TcpServer {
public:
    TcpServer(const InetAddress& listen_addr);
    TcpServer(const std::string& ip, uint16_t port);
    ~TcpServer();

    void start();
    void newConnection(int cfd);

private:
    void accept();

private:
    EventLoop::uptr m_main_loop;
    Acceptor::uptr m_acceptor;
};

} // namespace net
} // namespace conet

#endif