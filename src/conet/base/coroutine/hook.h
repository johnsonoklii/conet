#ifndef CONET_COROUTINE_HOOK_H
#define CONET_COROUTINE_HOOK_H

#include <sys/socket.h>


typedef int (*accept_fun_ptr_t)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef int (*readv_fun_ptr_t)(int sockfd, const struct iovec *iovec, int count);

namespace conet {

void setHook();
int accept_hook(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
size_t readv_hook(int sockfd, const struct iovec *iovec, int count);

extern "C" {
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
size_t readv(int sockfd, const struct iovec *iovec, int count);

}

} // namespace conet

#endif