#include "conet/net/Socket.h"
#include "conet/base/log/logger.h"

#include <sys/socket.h>
#include <unistd.h>

using namespace conet::log;

namespace conet {
namespace net {

Socket::Socket(int fd) {
    m_fd = fd;
}

Socket::~Socket() {
    close();
}

Socket Socket::createSocket() {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    Socket sock(fd);
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

int Socket::accept() {
    int connfd = ::accept(m_fd, nullptr, nullptr);
    if (connfd == -1) {
        LOG_FATAL("Socket::accept fatal: %s.", strerror(errno));
        return -1;
    }

    return connfd;
}

void Socket::close() {
    if (valid()) {
        ::close(m_fd);
        m_fd = -1;
    }
}

// TODO
void Socket::setNoDelay() {

}

void Socket::setReuseAddr() {

}

void Socket::setReusePort() {

}

void Socket::setKeepAlive() {

}

void Socket::setNonBlocking() {

}

} // namespace net
} // namespace conet