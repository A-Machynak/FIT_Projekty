/**
 * @file SFTP_Client.cpp
 * @author Augustin Machynak
 * @brief SFTP Client library (for linux)
 * @date 2022-04-20
 * 
 */

#include "../inc/SFTP_Client.h"
//#define _DEBUG_
#include "../inc/Utils.h"

namespace sftp {
    Client::Client(std::string ip, std::string workingDirectory, std::string port) {
        this->ip = ip;
        this->workingDirectory = workingDirectory;
        this->port = port;
    }

    Client::~Client() {
        // Intentionally empty
    }

    void Client::run() {
        // test the working directory
        DIR *dirp = opendir(workingDirectory.c_str());;
        if(!dirp) {
            _LOG_ERR("Couldn't open directory " << workingDirectory);
            setErrorCode(DIRECTORY_NOT_INITIALIZED);
            return;
        }
        closedir(dirp);

        // setup network stuff
        addrinfo_t hints {}, *res, *rp = NULL;

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        int returnCode = getaddrinfo(this->ip.c_str(), port.c_str(), &hints, &res);
        if(returnCode != 0) {
            _LOG_ERR("getaddrinfo() failed. Error(" << returnCode << "): " << gai_strerror(returnCode));
            setErrorCode(GETADDRINFO_FAILED);
            return;
        }

        tempOutput = "";
        // Connect to the first result we can
        for(rp = res; rp != NULL; rp = rp->ai_next) {
            socketFd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if(socketFd == -1) {
                tempOutput.append("Failed creating socket (errno: " + utils::errnoToStr() + "). Trying another address if available...\n");
                continue; // Fail
            }

            if(connect(socketFd, rp->ai_addr, rp->ai_addrlen) == 0) {
                break; // Success
            }

            tempOutput.append("Failed connecting (errno: " + utils::errnoToStr() + "). Trying another address if available...\n");
            close(socketFd); // Fail
        }
        tempOutput.append("No more addresses available");

        freeaddrinfo(res);
        if(rp == NULL) { // No address succeeded
            _LOG_ERR("Couldn't connect using any available address. Log: \n" << tempOutput);
            setErrorCode(BIND_FAILED);
            return;
        }

        _LOG_INFO("Connected successfully (IP/host: " << this->ip << " Port: " << this->port << ")");
        _LOG_INFO("Write \"DONE\" to disconnect\n");

        loop();
    }

    void Client::loop() {
        std::string receivedMsg;
        std::string inputCmd = "";

        while(true) {
            // Wait for response
            receivedMsg = receiveMessage();
            if(getErrorCode() != NONE) {
                _LOG_INFO("Lost connection to host");
                break;
            }

            // Print the server response
            std::cout << receivedMsg << std::endl;
            std::flush(std::cout);

            if(inputCmd.find("RETR") == 0 && receivedMsg[0] != '-') {
                retr(inputCmd.substr(5));
                if(getErrorCode() != NONE) {
                    break;
                }
            }
            if(inputCmd.find("STOR") == 0 && receivedMsg[0] != '-') {
                stor(inputCmd.substr(9));
                if(getErrorCode() != NONE) {
                    break;
                }
            }

            if(inputCmd == "DONE") {
                _LOG_INFO("Disconnecting...");
                break;
            }

            inputCmd = "";
            // Read input
            while(inputCmd == "") {
                getline(std::cin, inputCmd);
            }

            sendMessage(inputCmd);
        }

        close(socketFd);
    }

    void Client::sendMessage(std::string message) {
        int expectSent = message.length() + 1;
        int sent = send(socketFd, message.data(), expectSent, 0);
        
        if(sent != -1) {
            while(sent != expectSent) {
                // send() didn't send all the data, keep send()ing the next ones
                std::string next_response = message.substr(sent);
                expectSent = next_response.length();
                sent = send(socketFd, next_response.data(), expectSent, 0);
            }
        } // else couldn't send data (error)
    }

    std::string Client::receiveMessage() {
        int receivedMsgSize = MAX_MSG_SIZE;
        char receivedMsg[MAX_MSG_SIZE];
        std::string msg = "";

        while(receivedMsgSize == MAX_MSG_SIZE) {
            receivedMsgSize = recv(socketFd, (void*)receivedMsg, MAX_MSG_SIZE, 0);
            if(receivedMsgSize <= 0) {
                _LOG_DEBUG("Host disconnected");
                setErrorCode(DISCONNECT);
                return "";
            }
            msg += std::string(receivedMsg);
        }
        return msg;
    }

    void Client::retr(std::string fileName) {
        std::string inputCmd = "";
        // Read input
        while(inputCmd == "") {
            getline(std::cin, inputCmd);
        }
        sendMessage(inputCmd);

        if(inputCmd != "SEND") {
            // Not sending, wait for response
            std::string receivedMsg = receiveMessage();
            if(getErrorCode() != NONE) {
                _LOG_INFO("Lost connection to host");
                return;
            }
            // Print the server response
            std::cout << receivedMsg << std::endl;
            std::flush(std::cout);
            return;
        }

        _LOG_DEBUG("Receiving " << fileName);
        std::ofstream f;
        auto pos = fileName.rfind('/');
        if(pos != std::string::npos) {
            f.open(workingDirectory + "/" + fileName.substr(pos));
        } else {
            f.open(workingDirectory + "/" + fileName);
        }

        int receivedSize = CHUNK_SIZE;
        char buffer[CHUNK_SIZE];
        while(receivedSize >= CHUNK_SIZE) {
            receivedSize = recv(socketFd, (void*)buffer, CHUNK_SIZE, 0);
            if(receivedSize <= 0) {
                _LOG_DEBUG("Host disconnected");
                setErrorCode(DISCONNECT);
                return;
            }
            f.write(buffer, receivedSize);
            // Should probably check the size of the file.. eh
        }
        _LOG_DEBUG("Received");
        f.close();
    }

    void Client::stor(std::string fileName) {
        std::string path = workingDirectory + "/" + fileName;
        struct stat s;
        if(stat(path.c_str(), &s) != 0) {
            _LOG_ERR("Couldn't retrieve information about " << fileName << " - Aborting");
            sendMessage("-");
            std::string receivedMsg = receiveMessage();
            return;
        }
        std::cout << "SIZE " + std::to_string(s.st_size) << std::endl;
        sendMessage("SIZE " + std::to_string(s.st_size));

        std::string receivedMsg = receiveMessage();
        if(getErrorCode() != NONE) {
            _LOG_INFO("Lost connection to host");
            return;
        }
        // Print the server response
        std::cout << receivedMsg << std::endl;
        std::flush(std::cout);

        if(receivedMsg[0] != '+') {
            _LOG_DEBUG("Failed");
            return;
        }

        std::ifstream f(path.c_str());
        char *buffer = new char[CHUNK_SIZE];
        while(!f.eof()) {
            // Keep reading until eof
            f.read(buffer, CHUNK_SIZE);
            int readSize = f.gcount();
            int sent = 0;
            
            while(sent < readSize) {
                // Keep sending until the end of current read chunk
                int sendSize = readSize - sent;
                sent += send(socketFd, &buffer[sent], sendSize, 0);
                _LOG_DEBUG("Sending (" << sendSize << "): " << buffer);
            }
        }
        _LOG_DEBUG("File sent");
        f.close();
        delete buffer;

        receivedMsg = receiveMessage();
        if(getErrorCode() != NONE) {
            _LOG_INFO("Lost connection to host");
            return;
        }
        // Print the server response
        std::cout << receivedMsg << std::endl;
        std::flush(std::cout);
    }

    void Client::setErrorCode(errorCode_t errorCode) {
        this->errorCode = errorCode;
    }

    errorCode_t Client::getErrorCode() {
        return errorCode;
    }
}
