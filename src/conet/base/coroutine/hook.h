#ifndef CONET_COROUTINE_HOOK_H
#define CONET_COROUTINE_HOOK_H

#include <sys/socket.h>


typedef int (*accept_fun_ptr_t)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

namespace conet {

void setHook();
int accept_hook(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

extern "C" {
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
}

} // namespace conet

#endif