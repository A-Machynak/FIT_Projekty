/**
 * @file Utils.h
 * @author Augustin Machynak
 * @brief Utilities
 * @date 2022-04-20
 * 
 */
#ifndef __UTILS_H__
#define __UTILS_H__

#include <utility>
#include <vector>
#include <string>
#include <iostream>

namespace utils {
    #define _LOG_INFO(msg) { std::cerr << "[sftp INFO]: " << msg << std::endl; }
    #define _LOG_WARN(msg) { std::cerr << "[sftp WARNING]: " << msg << std::endl; }
    #define _LOG_ERR(msg) { std::cerr << "[sftp ERROR]: " << msg << std::endl; }

    #ifdef _DEBUG_
    #define _LOG_DEBUG(msg) { std::cerr << "[sftp DEBUG]: " << msg << std::endl; }
    #else
    #define _LOG_DEBUG(msg) { }
    #endif

    #define IS_DIGIT(x) ( (int)(x) >= ((int)('0')) && (int)(x) <= ((int)('9')) )

    inline std::string errnoToStr() {
        switch(errno) {
            case EACCES: return "EACCES";
            case EPERM: return "EPERM";
            case EAFNOSUPPORT: return "EAFNOSUPPORT";
            case EADDRINUSE: return "EADDRINUSE";
            case EBADF: return "EBADF";
            case EINVAL: return "EINVAL";
            case ENOTSOCK: return "ENOTSOCK";
            case EADDRNOTAVAIL: return "EADDRNOTAVAIL";
            case EFAULT: return "EFAULT";
            case ELOOP: return "ELOOP";
            case ENAMETOOLONG: return "ENAMETOOLONG";
            case ENOENT: return "ENOENT";
            case ENOMEM: return "ENOMEM";
            case ENOTDIR: return "ENOTDIR";
            case EROFS: return "EROFS";
            case EAGAIN: return "EAGAIN";
            case EALREADY: return "EALREADY";
            case ECONNREFUSED: return "ECONNREFUSED";
            case EINPROGRESS: return "EINPROGRESS";
            case EINTR: return "EINTR";
            case EISCONN: return "EISCONN";
            case ENETUNREACH: return "ENETUNREACH";
            case EPROTOTYPE: return "EPROTOTYPE";
            case ETIMEDOUT: return "ETIMEDOUT";
            case ENOTCONN: return "ENOTCONN";
            case ECONNRESET: return "ECONNRESET";
        }
        return "Unknown errno - " + std::to_string(errno);
    }

    inline std::vector<std::pair<std::string, std::string>> parseArgFlags(int argc, char **argv) {
        using strPair = std::pair<std::string, std::string>;
        std::vector<strPair> vec;
        bool expectFlag = true;
        std::string flag = "";

        for(uint16_t i = 1; i < argc; i++) {
            std::string value(argv[i]);

            if(value[0] == '-') {
                if(!expectFlag) {
                    vec.push_back(strPair(flag, ""));
                }
                flag = value;
                expectFlag = false;
            } else {
                vec.push_back(strPair(flag, value));
                expectFlag = true;
            }
        }
        return vec;
    }

    inline std::pair<int, int> toNumber(std::string num) {
        std::pair<int, int> p;

        if(num.length() > 8) {
            // just assume that the number is too big
            p.first = 1;
            return p;
        }

        p.first = 0;
        for(size_t i = 0; i < num.length(); i++) {
            if(!IS_DIGIT(num[i])) {
                p.first = 1;
                return p;
            }
        }
        p.second = atoi(num.c_str());
        return p;
    }
}

#endif
