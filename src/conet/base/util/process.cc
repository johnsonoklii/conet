#include "conet/base/util/process.h"

#include <unistd.h>
#include <sys/syscall.h>

namespace conet {

thread_local pid_t t_cached_tid = 0;

const char* ProcessInfo::getHostName() {
    thread_local char buf[256]{};
    if (buf[0] == -1) { return buf + 1; }
    if (::gethostname(buf + 1, sizeof(buf) - 1) == 0)
    {
        buf[0] = -1;
        return buf + 1;
    }
    return "unknownhost";
}

pid_t ProcessInfo::tid() {
    if (t_cached_tid == 0) {
        t_cached_tid = static_cast<pid_t>(::syscall(SYS_gettid));
    }
    return t_cached_tid;
}

} // namespace conet