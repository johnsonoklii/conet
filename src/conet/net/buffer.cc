#include "conet/net/buffer.h"
#include "conet/net/socket.h"

#include <sys/uio.h>
#include <errno.h>

namespace conet {
namespace net {

const char Buffer::kCRLF[] = "\r\n";
ssize_t Buffer::readSocket(const Socket& socket, int* save_errno) {
    char extrabuf[65535];
    struct iovec vec[2];
    
    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + m_writer_index;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;
    const ssize_t n = socket.readv(vec, iovcnt);
    if (n < 0) {
        *save_errno = errno;
    } else if (static_cast<size_t>(n) <= writable) {
        m_writer_index += n;
    } else {
        m_writer_index = m_buffer.size();
        append(extrabuf, n - writable);
    }
    
    return n;
}

} // namespace net
} // namespace conet

