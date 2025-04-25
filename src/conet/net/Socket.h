#ifndef CONET_SOCKET_H
#define CONET_SOCKET_H

#include "conet/net/InetAddress.h"

namespace conet {
namespace net {

class Socket {
public:
    Socket(int fd);
    ~Socket();

    static Socket createNonBlockSocket();

    int fd() const { return m_fd; }
    void setFd(int fd) { m_fd = fd; }

    void bind(const InetAddress& addr);
    void listen();
    int accept(InetAddress* peer_addr);

    void close();

    void setNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);
    void setNonBlocking();

    bool valid() { return m_fd > 0; }

private:
    Socket();
    
private:
    int m_fd{-1};
};

} // namespace net
} // namespace conet

#endif