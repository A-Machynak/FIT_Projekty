/**
 * @file SFTP_Common.h
 * @author Augustin Machynak
 * @brief Common SFTP defines/constants
 * @date 2022-04-20
 * 
 */
#ifndef __SFTP_COMMON_H__
#define __SFTP_COMMON_H__

#include <netdb.h>

namespace sftp {
    #define SFTP_SERVICE_NAME "IPK_SFTP"
    #define DEFAULT_PORT_STR "115"
    #define MAX_MSG_SIZE 512
    #define BACKLOG 10
    #define CHUNK_SIZE 8192

    typedef struct sockaddr_in sockaddr_in_t;
    typedef struct addrinfo addrinfo_t;

    typedef enum errorCode_e {
        NONE,
        DISCONNECT,
        GETIFADDRS_FAILED,
        GETNAMEINFO_FAILED,
        GETADDRINFO_FAILED,
        BIND_FAILED,
        LISTEN_FAILED,
        DIRECTORY_NOT_INITIALIZED,
        INTERFACE_NOT_FOUND,
        UNRECOVERABLE
    } errorCode_t;
}

#endif
