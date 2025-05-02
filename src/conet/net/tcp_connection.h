#ifndef CONET_NET_TCP_CONNECTION_H
#define CONET_NET_TCP_CONNECTION_H

#include "conet/net/socket.h"
#include "conet/net/eventLoop.h"
#include "conet/net/inet_address.h"
#include "conet/net/buffer.h"
#include "conet/base/util/timestamp.h"

#include <memory>

namespace conet {
namespace net {

enum TcpMode { kLT, kET };


class TcpConnection: public std::enable_shared_from_this<TcpConnection> {
public:
    using sptr = std::shared_ptr<TcpConnection>;

    using MessageCallBack = std::function<void(TcpConnection*, Buffer*)>;
    using ConnectionCallBack = std::function<void(TcpConnection*)>;
    using CloseCallBack = std::function<void(TcpConnection*)>;

    TcpConnection(EventLoop* loop, int fd, const InetAddress& local_addr, const InetAddress& peer_addr, TcpMode mode=kLT);
    ~TcpConnection();

    void send(const std::string& msg);

    void connectEstablished();

    void setConnectionCallBack(const ConnectionCallBack& cb) { m_connection_cb = std::move(cb); }
    void setMessageCallBack(const MessageCallBack& cb) { m_message_cb = std::move(cb); }
    void setCloseCallBack(const CloseCallBack& cb) { m_close_cb = std::move(cb); }

    int fd() const { return m_socket.fd(); }
    Timestamp lastReadTime() const { return m_last_read_time; }
    
    bool connected() const { return m_state == kConnected; }
    bool disconnected() const { return m_state == kDisconnected; }

    void shutdown();

    InetAddress localAddr() { return m_local_addr; }
    InetAddress peerAddr() { return m_peer_addr; }

private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    void setState(StateE state) { m_state = state; }

    void shutdownInLoop();
    void sendInLoop(const std::string& msg);

    void handleReadLT();
    void handleReadET();
    
    void handleWriteLT();
    void handleWriteET();
    void handleClose();
    void handleError();
    

private:
    EventLoop* m_loop;
    Socket m_socket;
    InetAddress m_local_addr;
    InetAddress m_peer_addr;
    Channel m_channel;
    StateE m_state{kConnecting};
    Timestamp m_last_read_time;

    TcpMode m_mode{kLT};

    Buffer m_input_buffer;
    Buffer m_output_buffer;

    ConnectionCallBack m_connection_cb;
    MessageCallBack m_message_cb;
    CloseCallBack m_close_cb; 
};

} // namespace net
} // namespace conet

#endif