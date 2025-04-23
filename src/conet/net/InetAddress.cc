#include "conet/net/InetAddress.h"

#include <cstring>

namespace conet {
namespace net {

InetAddress::InetAddress(const std::string& ip, uint16_t port)
: m_ip(ip), m_port(port), m_addr_len(sizeof(m_addr)) {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    m_addr.sin_port = htons(port);
    m_addr.sin_family = AF_INET;
}

sockaddr* InetAddress::getSockAddr() const {
    return (sockaddr*)&m_addr;
}

} // namespace net
} // namespace conet