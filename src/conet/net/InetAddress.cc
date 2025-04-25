#include "conet/net/InetAddress.h"

#include <cstring>

namespace conet {
namespace net {

InetAddress::InetAddress(const sockaddr_in& addr): m_addr(addr) {
}

InetAddress::InetAddress(const std::string& ip, uint16_t port)
: m_addr_len(sizeof(m_addr)) {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    m_addr.sin_port = htons(port);
    m_addr.sin_family = AF_INET;
}

std::string InetAddress::toIpPort() const {
    std::string ip = inet_ntoa(m_addr.sin_addr);
    std::string port = std::to_string(ntohs(m_addr.sin_port));
    return ip + ":" + port;
}

} // namespace net
} // namespace conet