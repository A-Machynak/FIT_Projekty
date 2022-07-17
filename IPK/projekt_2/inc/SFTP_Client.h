/**
 * @file SFTP_Client.h
 * @author Augustin Machynak
 * @brief SFTP Client library (for linux)
 * @date 2022-04-20
 * 
 */
#ifndef __SFTP_CLIENT_H__
#define __SFTP_CLIENT_H__

#include "SFTP_Common.h"

#include <fstream>
#include <string>

// Networking
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>

// Directory
#include <dirent.h>
#include <sys/stat.h>

namespace sftp {
    class Client {
    public:
        Client(std::string ip, std::string workingDirectory, std::string port = "115");
        ~Client();

        void run();

        errorCode_t getErrorCode();

    private:
        std::string ip;
        std::string port;
        std::string workingDirectory;

        int socketFd;

        errorCode_t errorCode = NONE;

        std::string tempOutput = "";

        void loop();
        void sendMessage(std::string message);
        std::string receiveMessage();
        void retr(std::string fileName);
        void stor(std::string fileName);
        void setErrorCode(errorCode_t errorCode);
    };
}

#endif
