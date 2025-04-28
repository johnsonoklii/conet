#include "conet/net/TcpServer.h"
#include "conet/base/coroutine/coroutine.h"

#include "conet/base/log/logger.h"
#include "conet/base/util/util.h"

namespace conet {
namespace net {

TcpServer::TcpServer(const InetAddress& local_addr)
: m_main_loop(new EventLoop)
, m_acceptor(new Acceptor(m_main_loop.get(), local_addr, true)) {

}

TcpServer::TcpServer(const std::string& ip, uint16_t port)
: m_main_loop(new EventLoop) {
    InetAddress listen_addr(ip, port);
    m_acceptor = std::unique_ptr<Acceptor>(new Acceptor(m_main_loop.get(), listen_addr, true)); // FIXME: 服务器的配置
}

TcpServer::~TcpServer() {}

void TcpServer::start() {
    LOG_DEBUG("TcpServer::start.");
    Coroutine::getMainCoroutine();

    // FIXME: 这里考虑使用协程池来统一管理协程的生命周期，目前会通过t_cur_coroutine和channel来维护其生命周期
    Coroutine::sptr co = std::make_shared<Coroutine>(kDefaultStackSize, std::bind(&TcpServer::accept, this));
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
        
        // TODO
        // 1. 创建TCP连接
        TcpConnection::sptr conn = std::make_shared<TcpConnection>(
                                                    m_main_loop.get(), cfd
                                                    , m_acceptor->getListenAddr()
                                                    , peer_addr);

        conn->setConnectionCallBack(m_connection_cb);
        conn->setMessageCallBack(m_message_cb);
        conn->setCloseCallBack(std::bind(&TcpServer::removeConnection, this, _1));
        
        // COMMENT: conn的生命周期由TcpServer管理
        m_connections[cfd] = conn;

        // 2. 分配
        // WARNING: 这里协程需要使用 conn.get()，否则会导致conn无法析构 
        // TODO: 原因? 如果用conn的shared_ptr，conn_co会持有conn，而channel会持有conn_co，conn又会持有channel，导致循环引用，无法析构
        Coroutine::sptr conn_co = std::make_shared<Coroutine>(kDefaultStackSize, std::bind(&TcpConnection::connectEstablished, conn.get()));
        m_main_loop->runCoroutine(conn_co);
    }
}

void TcpServer::removeConnection(TcpConnection* conn) {
    Coroutine::sptr co = std::make_shared<Coroutine>(kDefaultStackSize, std::bind(&TcpServer::removeConnectionInLoop, this, conn));
    m_main_loop->runCoroutine(co);
}

void TcpServer::removeConnectionInLoop(TcpConnection* conn) {
    m_main_loop->assertInLoopThread();
    m_connections.erase(conn->fd());
}

} // namespace net
} // namespace conet