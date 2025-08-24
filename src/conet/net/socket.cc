#include "conet/net/socket.h"
#include "conet/base/log/logger.h"
#include "conet/base/coroutine/hook.h"

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <fcntl.h>

namespace conet {
namespace net {

Socket::Socket(int fd) {
    m_fd = fd;
}

Socket::~Socket() {
    close();
}

int Socket::createNonBlockSocket(sa_family_t family) {
    int fd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (fd < 0) {
        LOG_FATAL("Socket::createNonBlockSocket(): %s.", strerror(errno));
        return -1;
    }
    return fd;
}

void Socket::bind(const InetAddress& addr) {
    if (!valid()) {
        LOG_FATAL("Socket::bind(): fd is invalid.");
        return;
    }

    if (::bind(m_fd, addr.getSockAddr(), addr.getAddrLen()) == -1) {
        LOG_FATAL("Socket::bind(): %s.", strerror(errno));
        return;
    }
}

void Socket::listen() {
    if (!valid()) {
        LOG_FATAL("Socket::listen(), fd is invalid.");
        return;
    }

    if (::listen(m_fd, SOMAXCONN) == -1) {
        LOG_FATAL("Socket::listen(): %s.", strerror(errno));
        return;
    }
}

int Socket::accept(InetAddress* peer_addr) {
    sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    socklen_t len = sizeof(cli_addr);
    int ret = conet::accept(m_fd, (sockaddr*)&cli_addr, &len);
    if (ret > 0) {
        peer_addr->setSockAddr(cli_addr);
    }
    return ret;
}

size_t Socket::readv(const struct iovec *iovec, int count) const {
    return conet::readv(m_fd, iovec, count);
}

size_t Socket::write(const char* buf, size_t len) const {
    return conet::write(m_fd, buf, len);
}

void Socket::close() {
    if (valid()) {
        ::close(m_fd);
        m_fd = -1;
    }
}

void Socket::shutdownWrite() {
    if (::shutdown(m_fd, SHUT_WR) == -1) {
        LOG_ERROR("Socket::shutdownWrite(): %s.", strerror(errno));
    }
}

void Socket::setNoDelay(bool on) {
    int optval = on ? 1 : 0;
    if(::setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY,
                    &optval, static_cast<socklen_t>(sizeof(optval))) == -1) {
        LOG_FATAL("Socket::setNoDelay(): %s.", strerror(errno));
    }
}

void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    if(::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR,
                    &optval, static_cast<socklen_t>(sizeof(optval))) == -1) {
        LOG_FATAL("Socket::setReuseAddr(): %s.", strerror(errno));
    }
}

void Socket::setReusePort(bool on) {
    int optval = on ? 1 : 0;
    if(::setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT,
                    &optval, static_cast<socklen_t>(sizeof(optval))) == -1) {
        LOG_FATAL("Socket::setReusePort(): %s.", strerror(errno));
    }
}

void Socket::setKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    if(::setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE,
                    &optval, static_cast<socklen_t>(sizeof(optval))) == -1) {
        LOG_FATAL("Socket::setKeepAlive(): %s.", strerror(errno));
    }
}

void Socket::setNonBlocking() {
    if (::fcntl(m_fd, F_SETFL, O_NONBLOCK) == -1) {
        LOG_FATAL("Socket::setNonBlocking(): %s.", strerror(errno));
    }
}

} // namespace net
} // namespace conet