#include "conet/net/Acceptor.h"
#include "conet/base/log/logger.h"

using namespace conet::log;

namespace conet {
namespace net {

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr)
: m_loop(loop)
, m_listenAddr(listenAddr)
, m_acceptSocket(Socket::createSocket())
, m_acceptChannel(m_acceptSocket.getFd()) {
    
}

void Acceptor::listen() {
    m_acceptSocket.bind(m_listenAddr);
    m_acceptSocket.listen();
    accept();
}

void Acceptor::accept() {
    int cfd = m_acceptSocket.accept();
    LOG_DEBUG("get new connection fd: %d", cfd);
}

} // namespace net
} // namespace conet