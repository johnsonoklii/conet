#include "conet/net/tcp_connection.h"
#include "conet/base/log/logger.h"

namespace conet {
namespace net {

TcpConnection::TcpConnection(EventLoop* loop, int fd, const InetAddress& local_addr, const InetAddress& peer_addr)
: m_loop(loop)
, m_socket(fd)
, m_local_addr(local_addr)
, m_peer_addr(peer_addr)
, m_channel(loop, fd) {
    m_socket.setNonBlocking();
    // COMMENT: Channel的生命周期由TcpConnection管理
    ChannelManager::getInstance().addChannel(&m_channel);
}

TcpConnection::~TcpConnection() {
    LOG_DEBUG("TcpConnection::~TcpConnection() fd: %d", m_socket.fd());
}

void TcpConnection::send(const std::string& msg) {
    ::send(m_socket.fd(), msg.c_str(), msg.size(), 0);
}

void TcpConnection::connectEstablished() {
    assert(m_state == kConnecting);
    setState(kConnected);

    if (m_connection_cb) {
        m_connection_cb(this);
    }
    handleRead();
}

// TODO: LT模式和ET模式
void TcpConnection::handleRead() {
    m_loop->assertInLoopThread();
    
    while (true) {
        int save_errno = 0;
        ssize_t n = m_input_buffer.readSocket(m_socket, &save_errno);
        if (n > 0) {
            m_message_cb(this, &m_input_buffer);
        } else if (n == 0) {
            handleClose();
            LOG_DEBUG("TcpConnection::recv(): connection closed by peer.");
            return;
        } else {
            errno = save_errno;
            handleError();
        }  

        // COMMENT: 只读一次，然后加入协程队列，下次再读，防止饿死其他conn
        m_loop->queueInLoop(Coroutine::getCurrentCoroutine());
        Coroutine::yield();
    }
}

void TcpConnection::handleClose() {
    m_loop->assertInLoopThread();

    assert(m_state == kConnected);
    setState(kDisconnected);

    m_loop->removeChannel(&m_channel); // COMMENT: LT模式下，这里必须清除所有事件，否则对端关闭时，会一直触发EPOLLIN事件

    if (m_connection_cb) {
        m_connection_cb(this);
    }
    m_close_cb(this);
}

void TcpConnection::handleError() {
    int err = 0;

    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof(optval));

    if (::getsockopt(m_channel.fd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
        err = errno;
    } else {
        err = optval;
    }

    LOG_ERROR("TcpConnection::handleError(): fd=%s - SO_ERROR=%d\n", m_socket.fd(), err);
}

} // namespace net
} // namespace conet