#ifndef CONET_UTIL_H
#define CONET_UTIL_H

#include <ctime>

#include <string>

namespace conet {

#define _1 std::placeholders::_1
#define _2 std::placeholders::_2
#define _3 std::placeholders::_3
#define _4 std::placeholders::_4
#define _5 std::placeholders::_5

struct Util {

    static const char* getErrInfo(int error_code);
    static const char* getCurDateTime(bool isTime, time_t* now = nullptr);

    static void mkDir(const std::string& dir);
    static std::string getShortFileName(std::string&& file_name);
};

}

#endif