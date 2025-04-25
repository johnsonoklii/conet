#include "conet/net/TcpServer.h"
int main() {
    conet::net::TcpServer server("127.0.0.1", 7981);
    server.start();
}