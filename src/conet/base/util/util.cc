#include "conet/base/util/util.h"

#include <cstring>
#include <cstdio>
#include <cassert>
#include <sys/stat.h>

#include <stdexcept>


namespace conet {

thread_local char           t_errnobuf[512];
thread_local char           t_time[64];
thread_local int            t_timezone = -1;
thread_local struct std::tm t_tm;
thread_local struct std::tm t_gmtm;
thread_local time_t         t_lastSecond;

const char* Util::getErrInfo(int error_code) {
    auto p = ::strerror_r(error_code, t_errnobuf, sizeof(t_errnobuf));
    (void)p;
    return t_errnobuf;
}

const char* Util::getCurDateTime(bool is_time, time_t* now)
{
   time_t timer = time(nullptr);
   if (now != nullptr) {   // to reduce system call
      *now = timer;
   }

   if (t_lastSecond != timer) {
      t_lastSecond = timer;
      ::localtime_r(&timer,&t_tm);
   }
   // to subtract gmtime and localtime for obtain timezone
   if (t_timezone == -1) {
      time_t gm_time = std::mktime(::gmtime_r(&timer, &t_gmtm));
      t_timezone = static_cast<int>(timer - gm_time) / 3600;
   }
   int len;
   if (is_time) {
      len = std::snprintf(t_time, sizeof(t_time),
                          "%4d-%02d-%02d %02d:%02d:%02d +%02d",
                          t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday,
                          t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec, t_timezone);
      assert(len == 23);
   }
   else {
      len = std::snprintf(t_time, sizeof(t_time), "%4d-%02d-%02d",
                          t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday);
      assert(len == 10);
   }
   (void)len;
   return t_time;
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