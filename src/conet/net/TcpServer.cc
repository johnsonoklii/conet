#include "conet/net/TcpServer.h"
#include "conet/base/coroutine/coroutine.h"

#include "conet/base/log/logger.h"

using namespace conet::log;

namespace conet {
namespace net {

TcpServer::TcpServer(const InetAddress& local_addr)
: m_main_loop(new EventLoop)
, m_acceptor(new Acceptor(local_addr)) {

}

TcpServer::TcpServer(const std::string& ip, uint16_t port)
: m_main_loop(new EventLoop) {
    InetAddress listen_addr(ip, port);
    m_acceptor = std::unique_ptr<Acceptor>(new Acceptor(listen_addr));
}

TcpServer::~TcpServer() {}

void TcpServer::start() {
    LOG_DEBUG("TcpServer::start.");
    Coroutine::getMainCoroutine();

    auto co_cb = [this] { accept(); };
    // FIXME: 这里考虑使用协程池来统一管理协程的生命周期，目前会通过t_cur_coroutine和channel来维护其生命周期
    Coroutine::sptr co = std::make_shared<Coroutine>(DEFAULT_STACK_SIZE, co_cb);
    Coroutine::resume(co);

    m_main_loop->loop();
}

void TcpServer::accept() {
    // FIXME: 如果一直有连接，其他的协程会一直阻塞，比如定时器
    while (true) {
        InetAddress peer_addr;
        int cfd = m_acceptor->accept(&peer_addr);
        if (cfd < 0) {
            Coroutine::yield();
            continue;
        }
        LOG_DEBUG("get new connection fd: %d, peer addr is: %s", cfd, peer_addr.toIpPort().c_str());
        
        // TODO
        // 1. 创建TCP连接
        
        // 2. 分配
    }
}

void TcpServer::newConnection(int cfd) {
    (void)cfd;
}

} // namespace net
} // namespace conet