#ifndef CONET_INETADDRESS_H
#define CONET_INETADDRESS_H

#include <arpa/inet.h>

#include <string>
#include <cstdint>

namespace conet {
namespace net {

// TODO: 支持ipv6
class InetAddress {
public:
    InetAddress() = default;
    InetAddress(const sockaddr_in& addr);
    InetAddress(const std::string& ip, uint16_t port);

    void setSockAddr(const sockaddr_in& addr) { m_addr = addr; };
    sockaddr* getSockAddr() const { return (sockaddr*)&m_addr; }
    uint32_t getAddrLen() const { return m_addr_len; }

    std::string toIpPort() const;

private:
    sockaddr_in m_addr;
    uint32_t m_addr_len{0};
};
        
} // namespace net
} // namespace conet

#endif