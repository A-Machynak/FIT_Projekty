/**
 * @file SFTP_Server.h
 * @author Augustin Machynak
 * @brief SFTP Server library (for linux)
 * @date 2022-04-20
 * 
 */
#ifndef __SFTP_SERVER_H__
#define __SFTP_SERVER_H__

#include "SFTP_Common.h"

#include <cstring>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <thread>

// Networking
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>

// Working with directories
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctime>
#include <limits.h>

namespace sftp {
    typedef enum ip_prefer_e {
        ANY,
        IPV4,
        IPV6
    } ip_prefer_t;

    class Server {
    public:
        Server(std::string workingDirectory, std::string credentialsFile, std::string interface = "", std::string port = DEFAULT_PORT_STR);
        ~Server();

        void run();

        /**
         * Set whether to prefer ipv4, ipv6 or any
         * @param ip Preferable ip version or any
         */
        void setPreferableIp(ip_prefer_t ip);

        errorCode_t getErrorCode();

    private:
        std::string interface;
        std::string workingDirectory;
        std::string port;  // aka service

        int socketFd;

        std::unordered_map<std::string, std::string> credentials;
        errorCode_t errorCode = NONE;
        ip_prefer_t preferableIp = ANY;

        std::string tempOutput = "";

        void loop();

        std::string getInterfaceAddress();
        void readCredentialsFile(std::string credentialsFile);
        void setErrorCode(errorCode_t errorCode);
    };

    class UserHandler {
    public:
        UserHandler(int sFd, std::string wDir, const std::unordered_map<std::string, std::string> &creds);

        void handle();
    private:
        typedef enum handlerState_e {
            INIT,
            EXPECT_USER,
            EXPECT_PASS,
            LOGIN,
            DISCONNECT
        } handlerState_t;

        typedef enum responseCode_e {
            SUCCESS,    // +
            ERROR,      // -
            NUMBER,     //  
            LOGGED_IN   // !
        } responseCode_t;

        typedef struct cmd_s {
            std::string command;
            std::vector<std::string> params;
        } cmd_t;


        int socketFd;
        std::string baseWorkingDirectory;
        std::string workingDirectory;
        const std::unordered_map<std::string, std::string>& credentials;

        handlerState_t state = INIT;
        std::string username; // USER/ACCT

        void sendMessage(std::string message, responseCode_t rc);
        void sendFile(std::string file, uint32_t chunkSize);
        std::string receiveMessage();
        cmd_t toCommand(std::string s);

        void user(cmd_t cmd);
        void acct(cmd_t cmd);
        void pass(cmd_t cmd);
        void type(cmd_t cmd);
        void list(cmd_t cmd);
        void cdir(cmd_t cmd);
        void kill(cmd_t cmd);
        void name(cmd_t cmd);
        void retr(cmd_t cmd);
        void stor(cmd_t cmd);
    };

    // Directory entry
    typedef struct dirEntry_s {
        std::string name;
        struct stat s;

        std::string _str;

        std::string toString() {
            return name;
        }

        void updateVerbose() { // Just save this once on init
            _str = name + " \t";

            if((s.st_mode & S_IFMT) == S_IFREG) { // Regular file
                _str += "f";
            } else if((s.st_mode & S_IFMT) == S_IFDIR) { // Directory
                _str += "d";
            }
            _str += " \t" + std::to_string(s.st_size);
        }

        std::string toStringVerbose() {
            return _str;
        }
    } dirEntry_t;

    /**
     * @brief Get directories and files from a specific directory.
     *
     * @param workDirFullPath working directory - attempt to leave this directory (using "../" for example) results in failure
     * @param directory directory to retrieve files from relative to working directory
     * @return <string, vector> pair where vector contains directories/files and string is the error message (empty if ok - "")
     */
    std::pair<std::string, std::vector<dirEntry_t>> getDirectoryListing(std::string workDirFullPath, std::string directory);


    std::pair<std::string, std::string> getDirectoryRealpath(std::string workDirFullPath, std::string directory);
}

#endif
