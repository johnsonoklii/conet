#include "conet/net/tcp_server.h"
#include "conet/base/coroutine/coroutine.h"

#include "conet/base/log/logger.h"
#include "conet/base/util/util.h"

namespace conet {
namespace net {

TcpServer::TcpServer(const InetAddress& local_addr, const std::string& name)
: m_main_loop(new EventLoop)
, m_acceptor(new Acceptor(m_main_loop.get(), local_addr, true))
, m_eventloop_threadpool(new EventLoopThreadPool(m_main_loop.get(), name))
, m_name(name) {
    
}

TcpServer::TcpServer(const std::string& ip, uint16_t port, const std::string& name)
: m_main_loop(new EventLoop)
, m_eventloop_threadpool(new EventLoopThreadPool(m_main_loop.get(), name))
, m_name(name) {
    InetAddress listen_addr(ip, port);
    m_acceptor = std::unique_ptr<Acceptor>(new Acceptor(m_main_loop.get(), listen_addr, true)); // FIXME: 服务器的配置
}

TcpServer::~TcpServer() {}

void TcpServer::setThreadNum(int num) { 
    m_eventloop_threadpool->setThreadNum(num);
}

void TcpServer::start() {
    std::call_once(m_started, [this]() {
        LOG_DEBUG("TcpServer::start.");

        m_eventloop_threadpool->start(m_thread_init_cb);

        // FIXME: 这里考虑使用协程池来统一管理协程的生命周期，目前会通过t_cur_coroutine和channel来维护其生命周期
        Coroutine::sptr co = std::make_shared<Coroutine>(kDefaultStackSize, std::bind(&TcpServer::accept, this));
        Coroutine::resume(co);

        m_main_loop->runEvery(m_conn_timeout, std::bind(&TcpServer::timeoutTcpConnection, this));
        m_main_loop->loop();
    });
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
        
        EventLoop* sub_loop = m_eventloop_threadpool->getNextLoop();
        TcpConnection::sptr conn = std::make_shared<TcpConnection>(
                                                    sub_loop, cfd
                                                    , m_acceptor->getListenAddr()
                                                    , peer_addr);

        conn->setConnectionCallBack(m_connection_cb);
        conn->setMessageCallBack(m_message_cb);
        conn->setCloseCallBack(std::bind(&TcpServer::removeConnection, this, _1));
        
        // COMMENT: conn的生命周期由TcpServer管理
        m_connections[cfd] = conn;

        // WARNING: 这里协程需要使用 conn.get()，否则会导致conn无法析构 
        // 原因? 如果用conn的shared_ptr，conn_co会持有conn，而channel会持有conn_co，conn又会持有channel，导致循环引用，无法析构
        Coroutine::sptr conn_co = std::make_shared<Coroutine>(kDefaultStackSize, std::bind(&TcpConnection::connectEstablished, conn.get()));
        sub_loop->runCoroutine(conn_co);
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

/*
    FIXME: 
    1. 需要遍历所有conn，连接多了会很耗时
    2. 精度问题
       1s, 2s, 3s, 4s, 5s | 6s, 7s, 8s, 9s, 10s
        2.1 在等待2s后, conn接收数据，
        2.2 第5s触发timeoutTcpConnection，此时conn没超时。
        3.3 下次在10s触发timeoutTcpConnection。但实际上应该在6s时，conn就应该超时关闭

    考虑使用时间轮
*/
void TcpServer::timeoutTcpConnection() {
    LOG_DEBUG("TcpServer::timeoutTcpConnection().");
    Timestamp now = Timestamp::now();
    for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
        TcpConnection::sptr conn = it->second;
        int64_t pad_time = now.milliSecondsSinceEpoch() - conn->lastReadTime().milliSecondsSinceEpoch();
        if (pad_time > m_conn_timeout) {
            conn->shutdown();
        }
    }
}

} // namespace net
} // namespace conet