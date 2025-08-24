#ifndef CONET_SOCKET_H
#define CONET_SOCKET_H

#include "conet/net/inet_address.h"

namespace conet {
namespace net {

class Socket {
public:
    Socket(int fd);
    ~Socket();

    static int createNonBlockSocket(sa_family_t family);

    int fd() const { return m_fd; }
    void setFd(int fd) { m_fd = fd; }

    void bind(const InetAddress& addr);
    void listen();
    int accept(InetAddress* peer_addr);

    size_t readv(const struct iovec *iovec, int count) const;
    size_t write(const char* buf, size_t len) const;

    void shutdownWrite();
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