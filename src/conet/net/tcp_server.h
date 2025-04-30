#ifndef CONET_NET_TCP_SERVER_H
#define CONET_NET_TCP_SERVER_H

#include "conet/net/eventLoop.h"
#include "conet/net/acceptor.h"
#include "conet/net/tcp_connection.h"
#include "conet/net/eventloop_threadpool.h"

namespace conet {
namespace net {

class TcpServer {
public:
    using ConnectionMap = std::unordered_map<int, TcpConnection::sptr>;
    using MessageCallBack = std::function<void(TcpConnection*, Buffer*)>;
    using ConnectionCallBack = std::function<void(TcpConnection*)>;

    TcpServer(const InetAddress& listen_addr, const std::string& name);
    TcpServer(const std::string& ip, uint16_t port, const std::string& name);
    ~TcpServer();

    void start();

    void setThreadNum(int num);

    void setThreadInitCallback(const EventLoopThread::ThreadInitCallback& cb) { m_thread_init_cb = cb; }
    void setConnectionCallBack(const ConnectionCallBack& cb) { m_connection_cb = cb; }
    void setMessageCallBack(const MessageCallBack& cb) { m_message_cb = cb; }

private:
    void accept();
    void removeConnection(TcpConnection* conn);
    void removeConnectionInLoop(TcpConnection* conn);

    void timeoutTcpConnection();

private:
    EventLoop::uptr m_main_loop;
    Acceptor::uptr m_acceptor;
    EventLoopThreadPool::uptr m_eventloop_threadpool;

    ConnectionMap m_connections;
    
    int m_conn_timeout{1000*60}; // 60s 
    std::string m_name;
    std::once_flag m_started;
    
    EventLoopThread::ThreadInitCallback m_thread_init_cb;
    ConnectionCallBack m_connection_cb;
    MessageCallBack m_message_cb;
};

} // namespace net
} // namespace conet

#endif