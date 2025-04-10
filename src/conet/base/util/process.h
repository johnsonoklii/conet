#ifndef CONET_PROCESS_H
#define CONET_PROCESS_H

#include <sys/types.h>

namespace conet {

extern thread_local int t_cached_tid;

struct ProcessInfo {
    static const char* getHostName();
    static pid_t tid();
};

} // namespace conet

#endif