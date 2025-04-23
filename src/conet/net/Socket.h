#ifndef CONET_SOCKET_H
#define CONET_SOCKET_H

#include "conet/net/InetAddress.h"

namespace conet {
namespace net {

class Socket {
public:
    Socket(int fd);
    ~Socket();

    static Socket createSocket();

    int getFd() const { return m_fd; }
    void setFd(int fd) { m_fd = fd; }

    void bind(const InetAddress& addr);
    void listen();
    int accept();

    void close();

    void setNoDelay();
    void setReuseAddr();
    void setReusePort();
    void setKeepAlive();
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