#include "conet/net/Acceptor.h"
#include "conet/net/EventLoop.h"
#include "conet/base/log/logger.h"

using namespace conet::log;

namespace conet {
namespace net {

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr)
: m_loop(loop)
, m_listenAddr(listenAddr)
, m_acceptSocket(Socket::createSocket())
, m_acceptChannel(m_acceptSocket.getFd()) {
    // TODO: 注册连接成功的回调
}

void Acceptor::listen() {
    m_acceptSocket.bind(m_listenAddr);
    m_acceptSocket.listen();
}

int Acceptor::accept() {
    return m_acceptSocket.accept();
    // m_listening = true;
    // while (m_listening) {
    //     int cfd = m_acceptSocket.accept();
    //     if (cfd < 0) {
    //         if (errno == EAGAIN || errno == EWOULDBLOCK) {
    //             LOG_DEBUG("accept error: %s", strerror(errno));
                
    //             // TODO 
    //             // 1. 注册定时器，超过x秒事件没触发，就调用定时器
    //             // 2. 注册可读事件
    //             m_acceptChannel.enableRead();
    //             m_loop->updateChannel(&m_acceptChannel);
    //             // 3. 退出当前协程yield()
                
    //         } else {
    //             LOG_ERROR("accept error: %s", strerror(errno));
    //             break;
    //         }
    //     }
    //     LOG_DEBUG("get new connection fd: %d", cfd);

    //     // 1. 创建TCP连接
    //     // 2. 分配
    // }
}

} // namespace net
} // namespace conet