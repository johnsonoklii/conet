#include "conet/net/Socket.h"
#include "conet/base/log/logger.h"
#include "conet/base/coroutine/hook.h"

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <fcntl.h>

using namespace conet::log;

namespace conet {
namespace net {

Socket::Socket(int fd) {
    m_fd = fd;
}

Socket::~Socket() {
    close();
}

Socket Socket::createNonBlockSocket() {
    int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    Socket sock(fd);
    sock.setNonBlocking();
    return sock;
}

void Socket::bind(const InetAddress& addr) {
    if (!valid()) {
        LOG_FATAL("Socket::bind fatal: fd is invalid.");
        return;
    }

    if (::bind(m_fd, addr.getSockAddr(), addr.getAddrLen()) == -1) {
        LOG_FATAL("Socket::bind fatal: %s.", strerror(errno));
        return;
    }
}

void Socket::listen() {
    if (!valid()) {
        LOG_FATAL("Socket::listen fatal, fd is invalid.");
        return;
    }

    if (::listen(m_fd, SOMAXCONN) == -1) {
        LOG_FATAL("Socket::listen fatal: %s.", strerror(errno));
        return;
    }
}

int Socket::accept(InetAddress* peer_addr) {
    sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    socklen_t len = sizeof(cli_addr);
    int ret = accept_hook(m_fd, (sockaddr*)&cli_addr, &len);
    if (ret > 0) {
        peer_addr->setSockAddr(cli_addr);
    }
    return ret;
}

void Socket::close() {
    if (valid()) {
        ::close(m_fd);
        m_fd = -1;
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