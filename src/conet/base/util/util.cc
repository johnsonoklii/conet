#include "conet/base/util/util.h"

#include <cstring>
#include <cstdio>
#include <cassert>
#include <sys/stat.h>

#include <stdexcept>

namespace conet {

thread_local char           t_errnobuf[512];

const char* Util::getErrInfo(int error_code) {
    auto p = ::strerror_r(error_code, t_errnobuf, sizeof(t_errnobuf));
    (void)p;
    return t_errnobuf;
}

void Util::mkDir(const std::string& dir) {
   struct stat info;
      if (stat(dir.c_str(), &info) != 0) {
         if (mkdir(dir.c_str(), 0755) != 0) {
            throw std::runtime_error("Failed to create directory: " + dir);
         }
   } else if (!(info.st_mode & S_IFDIR)) {
         throw std::runtime_error("Path exists but is not a directory: " + dir);
   }
}

std::string Util::getShortFileName(std::string&& file_name) {
   size_t last_slash_pos = file_name.find_last_of("/\\");
   if (last_slash_pos == std::string::npos) {
       return file_name;
   }
   return file_name.substr(last_slash_pos + 1);
}

} // namespace conet