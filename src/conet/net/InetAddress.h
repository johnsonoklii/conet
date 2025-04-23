#ifndef CONET_INETADDRESS_H
#define CONET_INETADDRESS_H

#include <arpa/inet.h>

#include <string>
#include <cstdint>

namespace conet {
namespace net {
    
class InetAddress {
public:
    InetAddress(const std::string& ip, uint16_t port);

    sockaddr* getSockAddr() const;
    uint32_t getAddrLen() const { return m_addr_len; }

private:
    std::string m_ip;
    uint16_t m_port{0};
    sockaddr_in m_addr;
    uint32_t m_addr_len{0};
};
        
} // namespace net
} // namespace conet

#endif