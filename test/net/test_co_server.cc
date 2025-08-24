#include "conet/net/tcp_server.h"
#include "conet/base/log/logger.h"

#include <unistd.h>

void onConnection(conet::net::TcpConnection::sptr conn) {
    if (conn->connected()) {
        //LOG_INFO("connection established, fd: %d, peer addr: %s.", conn->fd(), conn->peerAddr().toIpPort().c_str());
    } else {
        // LOG_INFO("connection closed, fd: %d, peer addr: %s.", conn->fd(), conn->peerAddr().toIpPort().c_str());
    }
}

void onMessage(conet::net::TcpConnection::sptr conn, conet::net::Buffer* buffer) {
    std::string msg = buffer->retrieveAllAsString();
  
    conet::usleep(1000);
    // LOG_INFO("recv msg: %s", msg.c_str());
    conn->send(msg);
}

int main() {
    auto config = conet::log::GLOBAL_OPTION();
    config.setLevel(conet::log::LogLevel::kERROR);
    Logger::getInstance().setOption(config);

    conet::setHook(true);
    conet::net::TcpServer server("0.0.0.0", 7777, "test_server");
    server.setThreadNum(4);
    server.setConnectionCallBack(onConnection);
    server.setMessageCallBack(onMessage);
    server.start();
}