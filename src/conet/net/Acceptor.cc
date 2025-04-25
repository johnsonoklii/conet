#include "conet/net/Acceptor.h"
#include "conet/net/EventLoop.h"
#include "conet/base/coroutine/hook.h"
#include "conet/base/coroutine/coroutine.h"
#include "conet/base/log/logger.h"

#include <unistd.h>
#include <fcntl.h>

using namespace conet::log;

namespace conet {
namespace net {

Acceptor::Acceptor(const InetAddress& listen_addr)
: m_listen_addr(listen_addr)
, m_accept_socket(Socket::createNonBlockSocket())
, m_accept_channel(m_accept_socket.fd())
, m_idle_fd(::open("/dev/null", O_RDONLY | O_CLOEXEC)) {
    m_accept_socket.bind(m_listen_addr);
    m_accept_socket.listen();
    // COMMENT: Channel的生命周期由Acceptor管理
    ChannelManager::getInstance().addChannel(&m_accept_channel);
}

int Acceptor::accept(InetAddress* peer_addr) {
    int ret = m_accept_socket.accept(peer_addr);
    if (ret < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            LOG_DEBUG("Acceptor::accept(): accept in nonblocking mode.");
        } else if (errno == EMFILE) {
            ::close(m_idle_fd);
            m_idle_fd = accept_hook(m_accept_socket.fd(), NULL, NULL);
            ::close(m_idle_fd);
            m_idle_fd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        } else {
            LOG_ERROR("Acceptor::accept(): %s", strerror(errno));
        }
        return -1;
    }

    return ret;
}

} // namespace net
} // namespace conet