#include "conet/net/Acceptor.h"
#include "conet/net/InetAddress.h"
#include "conet/net/EventLoop.h"

int main() {
    conet::net::EventLoop loop;
    conet::net::InetAddress addr("127.0.0.1", 7981);
    conet::net::Acceptor acceptor(&loop, addr);
    acceptor.listen();

    getchar();
}