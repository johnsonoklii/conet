#include "conet/net/tcp_connection.h"
#include "conet/base/log/logger.h"

#include <unistd.h>

namespace conet {
namespace net {

TcpConnection::TcpConnection(EventLoop* loop, int fd, const InetAddress& local_addr, const InetAddress& peer_addr, TcpMode mode)
: m_loop(loop)
, m_socket(fd)
, m_local_addr(local_addr)
, m_peer_addr(peer_addr)
, m_channel(loop, fd)
, m_last_read_time(Timestamp::now())
, m_mode(mode) {
    m_socket.setNonBlocking();
}

TcpConnection::~TcpConnection() {
    LOG_DEBUG("TcpConnection::~TcpConnection(): fd=%d.", m_socket.fd());
    ChannelManager::getInstance().removeChannel(&m_channel);
}

void TcpConnection::send(const std::string& msg) {
    Coroutine::sptr co = std::make_shared<Coroutine>(kDefaultStackSize, std::bind(&TcpConnection::sendInLoop, this, msg));
    m_loop->runCoroutine(co);
}

void TcpConnection::sendInLoop(const std::string& msg) {
    m_loop->assertInLoopThread();

    if (!connected()) {
        LOG_ERROR("TcpConnection::sendInLoop(): disconnected, give up send.");
        return;
    }

    if (m_channel.isWriting() || m_output_buffer.readableBytes() > 0) {
        m_output_buffer.append(msg.data(), msg.size());
        return;
    }

    bool flag = true;
    int left_bytes = msg.size();

    ssize_t n = ::write(m_socket.fd(), msg.data(), msg.size()); // FIXME: 测试，改为msg.size()
    if (n >= 0) {
        left_bytes -= n;
        if (static_cast<size_t>(n) == msg.size()) {
            LOG_DEBUG("TcpConnection::sendInLoop(): send all data.");
            return;
        }
    } else {
        LOG_ERROR("TcpConnection::sendInLoop(): %s.", strerror(errno));
        handleError();
        flag = false;
    }

    // 没写完，注册写事件
    if (flag && left_bytes > 0) {
        m_output_buffer.append(msg.data() + n, left_bytes);

        Coroutine::sptr co;
        if (m_mode == kLT) {
            co = std::make_shared<Coroutine>(kDefaultStackSize, std::bind(&TcpConnection::handleWriteLT, shared_from_this()));
        } else {
            co = std::make_shared<Coroutine>(kDefaultStackSize, std::bind(&TcpConnection::handleWriteET, shared_from_this()));
        }
        m_channel.enableWrite(co);
    }
}

void TcpConnection::connectEstablished() {
    assert(m_state == kConnecting);
    setState(kConnected);

    // COMMENT: Channel的生命周期由TcpConnection管理
    ChannelManager::getInstance().addChannel(&m_channel);

    if (m_connection_cb) {
        m_connection_cb(shared_from_this());
    }

    Coroutine::sptr co;
    if (m_mode == kLT) {
        co = std::make_shared<Coroutine>(kDefaultStackSize, std::bind(&TcpConnection::handleReadLT, shared_from_this()));
    } else {
        co = std::make_shared<Coroutine>(kDefaultStackSize, std::bind(&TcpConnection::handleReadET, shared_from_this()));
    }
    m_channel.enableRead(co);
}

void TcpConnection::handleReadLT() {
    m_loop->assertInLoopThread();
    while (true) {
        int save_errno = 0;
        ssize_t n = m_input_buffer.readSocket(m_socket, &save_errno);
        m_last_read_time = Timestamp::now();
        if (n > 0) {
            /*
                COMMENT: 这里需要shared_from_this()，如果出现超时断连，在删除conn前，对端发送了消息，
                        然后删除conn，然后再触发可读事件，此时就会core dump
            shutdown, 
                    对端send2,
                             对端收到shutdown，close,
                                                    触发读事件close, 删除conn,
                                                                        (因为网络延迟，此时才收到send2) 触发读事件，core dump         
            */
            m_message_cb(shared_from_this(), &m_input_buffer);
        } else if (n == 0) {
            handleClose();
            return;
        } else {
            errno = save_errno;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // COMMENT: LT模式下，不用处理
            } else if (errno == EPIPE || errno == ECONNRESET) {
                LOG_ERROR("TcpConnection::handleReadLT(): %s.", strerror(errno));
                handleClose();
                return;
            } else {
                LOG_ERROR("TcpConnection::handleReadLT(): %s.", strerror(errno));
                handleError();
            }
        }  

        // COMMENT: LT模式下，只读一次
        Coroutine::yield();
    } 
}

void TcpConnection::handleReadET() {
    m_loop->assertInLoopThread();
    
    while (true) {
        int save_errno = 0;
        ssize_t n = m_input_buffer.readSocket(m_socket, &save_errno);
        m_last_read_time = Timestamp::now();
        if (n > 0) {
            m_message_cb(shared_from_this(), &m_input_buffer);
        } else if (n == 0) {
            handleClose();
            return;
        } else {
            errno = save_errno;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // COMMENT: ET模式下，需要将数据全部读完，没有数据可读，就退出当前协程，等待下次读事件触发
                Coroutine::yield();
            } else if (errno == EPIPE || errno == ECONNRESET) {
                LOG_ERROR("TcpConnection::handleReadET(): %s.", strerror(errno));
                handleClose();
                return;
            } else {
                LOG_ERROR("TcpConnection::handleReadET(): %s.", strerror(errno));
                handleError();
            }
        }  
    }
}

void TcpConnection::handleWriteLT() {
    m_loop->assertInLoopThread();

    while (true) {
        if (m_output_buffer.readableBytes() > 0) {
            size_t n = m_socket.write(m_output_buffer.peek(), m_output_buffer.readableBytes());
            if (n > 0) {
                m_output_buffer.retrieve(n);
                if (m_output_buffer.readableBytes() == 0) {
                    LOG_DEBUG("TcpConnection::sendInLoop(): send all data.");
                    m_channel.disableWrite();
                    if (m_state == kDisconnecting) { // COMMENT: 之前想关闭时，还要写数据。现在写完了, 就关闭
                        shutdownInLoop();
                    }
                    return;
                }
            } else if (n == 0) {
                // COMMENT: LT模式下，不需要重新注册写事件，下次可写了会继续触发写事件
                LOG_DEBUG("TcpConnection::handleWrite(): send 0 bytes.");
            } else {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // COMMENT: 在 LT 模式下，不需要重新注册写事件
                } else if (errno == EPIPE || errno == ECONNRESET) {
                    LOG_ERROR("TcpConnection::handleWrite(): %s.", m_channel.fd(), strerror(errno));
                    handleClose();
                    return;
                } else {
                    LOG_ERROR("TcpConnection::handleWrite(): %s.", m_channel.fd(), strerror(errno));
                    handleError();
                }
            }
        } else {
            m_channel.disableWrite();
            if (m_state == kDisconnecting) {
                shutdownInLoop();
            }
            return;
        }

        Coroutine::yield();
    }
}

void TcpConnection::handleWriteET() {
    m_loop->assertInLoopThread();

    while (true) {
        if (m_output_buffer.readableBytes() > 0) {
            size_t n = m_socket.write(m_output_buffer.peek(), m_output_buffer.readableBytes());
            if (n > 0) {
                m_output_buffer.retrieve(n);
                if (m_output_buffer.readableBytes() == 0) {
                    LOG_DEBUG("TcpConnection::sendInLoop(): send all data.");
                    m_channel.disableWrite();
                    if (m_state == kDisconnecting) { // COMMENT: 之前想关闭时，还要写数据。现在写完了, 就关闭
                        shutdownInLoop();
                    }
                    return;
                }
            } else if (n == 0) {
                // COMMENT: ET模式下，需要重新注册写事件
                LOG_DEBUG("TcpConnection::handleWrite(): send 0 bytes.");
                if (m_output_buffer.readableBytes() > 0) {
                    m_channel.enableWrite(Coroutine::getCurrentCoroutine());
                }
                Coroutine::yield();
            } else {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // COMMENT: 在 ET 模式下，需要重新注册写事件
                    if (m_output_buffer.readableBytes() > 0) {
                        m_channel.enableWrite(Coroutine::getCurrentCoroutine());
                    }
                    Coroutine::yield();
                } else if (errno == EPIPE || errno == ECONNRESET) {
                    LOG_ERROR("TcpConnection::handleWrite(): %s.", m_channel.fd(), strerror(errno));
                    handleClose();
                    return;
                } else {
                    LOG_ERROR("TcpConnection::handleWrite(): %s.", m_channel.fd(), strerror(errno));
                    handleError();
                }
            }
        } else {
            m_channel.disableWrite();
            if (m_state == kDisconnecting) {
                shutdownInLoop();
            }
            return;
        }
    }
}

void TcpConnection::handleClose() {
    m_loop->assertInLoopThread();

    assert(m_state == kConnected || m_state == kDisconnecting);
    setState(kDisconnected);

    m_loop->removeChannel(&m_channel); // COMMENT: LT模式下，这里必须清除所有事件，否则对端关闭时，会一直触发EPOLLIN事件

    if (m_connection_cb) {
        m_connection_cb(shared_from_this());
    }
    m_close_cb(shared_from_this());
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

    LOG_ERROR("TcpConnection::handleError(): fd=%d - SO_ERROR=%s.", m_socket.fd(), strerror(err));
}

void TcpConnection::shutdown() {
    Coroutine::sptr co = std::make_shared<Coroutine>(kDefaultStackSize, std::bind(&TcpConnection::shutdownInLoop, this));
    m_loop->runCoroutine(co);
}


/* 情况一：
    1. 超时，shutdownInLoop: 正在写，设置kDisconnecting
    2. 设置完kDisconnecting, 对端关闭，触发handleClose: 设置kDisconnected，移除conn
*/

/* 情况二：
    1. 超时，shutdownInLoop: 没写，直接shutdown
    2. 两种情况:
        2.1. shutdown发送完成，对端关闭，触发handleClose: 设置kDisconnected，移除conn
        2.2. shutdown发送过程中，对端发了一个close，触发handleClose: 设置kDisconnected，移除conn
*/

/* 情况三：
    1. 对端关闭，触发handleClose: 设置kDisconnected，移除conn
    2. 超时，shutdownInLoop：只处理kConnected和kDisconnecting
*/
void TcpConnection::shutdownInLoop() {
    m_loop->assertInLoopThread();

    if (m_state == kConnected || m_state == kDisconnecting) { // COMMENT: 这里还要处理kDisconnecting，写完后，会再shutdownInLoop
        // COMMENT: 如果还有数据要发送，将数据发送完后再关闭
        if (m_channel.isWriting()) {
            setState(kDisconnecting);
        } else {
            m_socket.shutdownWrite();
        }
    }                          
}

} // namespace net
} // namespace conet