#include "conet/net/tcp_server.h"
#include "conet/base/log/logger.h"

void onConnection(conet::net::TcpConnection* conn) {
    if (conn->connected()) {
        LOG_INFO("connection established, fd: %d, peer addr: %s.", conn->fd(), conn->peerAddr().toIpPort().c_str());
    } else {
        // LOG_INFO("connection closed, fd: %d, peer addr: %s.", conn->fd(), conn->peerAddr().toIpPort().c_str());
    }
}

void onMessage(conet::net::TcpConnection* conn, conet::net::Buffer* buffer) {
    std::string msg = buffer->retrieveAllAsString();
    LOG_INFO("recv msg: %s", msg.c_str());
    conn->send(msg);
}

int main() {
    conet::setHook(false);
    conet::net::TcpServer server("0.0.0.0", 7777, "test_server");
    server.setThreadNum(4);
    server.setConnectionCallBack(onConnection);
    server.setMessageCallBack(onMessage);
    server.start();
}