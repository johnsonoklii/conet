#ifndef CONET_COROUTINE_HOOK_H
#define CONET_COROUTINE_HOOK_H

#include <sys/socket.h>


typedef int (*accept_fun_ptr_t)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef ssize_t (*readv_fun_ptr_t)(int sockfd, const struct iovec *iovec, int count);
typedef ssize_t (*write_fun_ptr_t)(int sockfd, const char* buf, size_t len);

namespace conet {

void setHook();
int accept_hook(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
ssize_t readv_hook(int sockfd, const struct iovec *iovec, int count);
ssize_t write_hook(int sockfd, const char* buf, size_t len);

extern "C" {
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
ssize_t readv(int sockfd, const struct iovec *iovec, int count);
ssize_t write(int sockfd, const char* buf, size_t len);
}

} // namespace conet

#endif