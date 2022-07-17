/**
 * @file SFTP_Server.cpp
 * @author Augustin Machynak
 * @brief SFTP Server library (for linux)
 * @date 2022-04-20
 * 
 */

#include "../inc/SFTP_Server.h"
//#define _DEBUG_
#include "../inc/Utils.h"

#include <iostream>

namespace sftp {
    void threadExec(UserHandler handler) {
        handler.handle();
    }


    Server::Server(std::string workingDirectory, std::string credentialsFile, std::string interface, std::string port) {
        this->workingDirectory = workingDirectory;
        this->interface = interface;
        this->port = port;

        readCredentialsFile(credentialsFile);
    }

    Server::~Server() {
        // Intentionally empty
    }

    void Server::run() {
        addrinfo_t hints {}, *res, *rp = NULL;
        std::string addr;

        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        if(preferableIp == IPV4) {
            hints.ai_family = AF_INET;
            addr = "0.0.0.0";
        } else if(preferableIp == IPV6) {
            hints.ai_family = AF_INET6;
            addr = "::";
        } else {
            hints.ai_family = AF_UNSPEC;
            addr = "";
        }

        if(this->interface != "") {
            // Find ip address of an interface
            addr = getInterfaceAddress();

            if(getErrorCode() == 0 && addr == "") {
                _LOG_ERR("Interface \"" << this->interface << "\" wasn't found. List of available interfaces:\n" << tempOutput);
                setErrorCode(INTERFACE_NOT_FOUND);
                return;
            }
            _LOG_INFO("Found interface " << this->interface << " with ip " << addr);
        }


        int returnCode;
        if(addr == "") {
            returnCode = getaddrinfo(NULL, port.c_str(), &hints, &res);
        } else {
            returnCode = getaddrinfo(addr.c_str(), port.c_str(), &hints, &res);
        }

        if(returnCode != 0) {
            _LOG_ERR("getaddrinfo() failed. Error(" << returnCode << "): " << gai_strerror(returnCode));
            setErrorCode(GETADDRINFO_FAILED);
            return;
        }

        if(addr == "") {
            // Since we set ai_family AF_UNSPEC flag, we have to get the real address from the result
            char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

            if(getnameinfo((struct sockaddr*)res, sizeof(struct sockaddr*), hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
                addr = std::string(hbuf);
            } else {
                addr = "[Address couldn't be retrieved]";
            }
        }

        // Source: getaddrinfo() man pages

        /* getaddrinfo() returns a list of address structures.
            Try each address until we successfully bind(2).
            If socket(2) (or bind(2)) fails, we (close the socket
            and) try the next address. */
        tempOutput = "";
        for(rp = res; rp != NULL; rp = rp->ai_next) {
            socketFd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

            if(socketFd == -1) {
                tempOutput.append("Failed creating socket (errno: " + utils::errnoToStr() + "). Trying another address if available...\n");
                continue; // Fail
            }
            if(bind(socketFd, rp->ai_addr, rp->ai_addrlen) == 0) {
                break; // Success
            }
            tempOutput.append("Failed binding to socket (errno: " + utils::errnoToStr() + "). Trying another address if available...\n");
            close(socketFd); // Fail
        }
        tempOutput.append("No more addresses available");

        freeaddrinfo(res);
        if(rp == NULL) { // No address succeeded
            _LOG_ERR("Couldn't bind to any address. Log: \n" << tempOutput);
            setErrorCode(BIND_FAILED);
            return;
        }

        // Reuse the socket address
        int tru = 1;
        if(setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &tru, sizeof(tru))) {
            _LOG_WARN("Failed setting SO_REUSEADDR for socket");
        }

        _LOG_INFO("Bound successfully (IP/host: " << addr << " Port: " << this->port << ")");

        _LOG_INFO("Listen()ing on socket...");
        int result = listen(socketFd, BACKLOG);
        if(result != 0) {
            shutdown(socketFd, SHUT_RDWR);
            if(errno == EADDRINUSE) {
                _LOG_ERR("Socket listen() call failed: Another socket is already listening on the same port.");
            } else {
                _LOG_ERR("Socket listen() call failed unexpectedly: " << utils::errnoToStr());
            }
            setErrorCode(LISTEN_FAILED);
            return;
        }

        loop();
    }

    void Server::loop() {
        sockaddr_in_t socketAddrTemp;
        int newSocketFd;
        unsigned int size = sizeof(sockaddr_in_t);

        // thread function
        auto fn = [](UserHandler *h) { 
            h->handle();
            delete h;
        };

        // Create new thread for each user
        while((newSocketFd = accept(socketFd, (struct sockaddr*)&socketAddrTemp, &size))) {
            UserHandler *h = new UserHandler(newSocketFd, this->workingDirectory, this->credentials);
            std::thread tx(fn, h);
            tx.detach();
        }

        close(socketFd);
    }

    std::string Server::getInterfaceAddress() {
        tempOutput = "";

        // Source: getifaddrs() man pages 
        struct ifaddrs *ifaddr;
        int family, s;
        char host[NI_MAXHOST];
        std::string foundHost = "";

        if(getifaddrs(&ifaddr) == -1) {
            setErrorCode(GETIFADDRS_FAILED);
            return "";
        }

        /* Walk through linked list, maintaining head pointer so we can free list later. */
        for(struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
            if(ifa->ifa_addr == NULL)
                continue;

            family = ifa->ifa_addr->sa_family;
            if(family != AF_INET && family != AF_INET6) {
                continue;
            }

            size_t structSize = (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
            s = getnameinfo(ifa->ifa_addr,
                        structSize,
                        host, NI_MAXHOST,
                        NULL, 0, NI_NUMERICHOST);
            
            if(s != 0) {
                setErrorCode(GETNAMEINFO_FAILED);
                return "";
            }

            tempOutput.append(ifa->ifa_name);
            if(family == AF_INET) {
                tempOutput.append("\t AF_INET (" + std::to_string(family) + "): ");
            } else {
                tempOutput.append("\t AF_INET6 (" + std::to_string(family) + "): ");
            }
            tempOutput.append(std::string(host) + "\n");

            if(strcmp(ifa->ifa_name, this->interface.c_str())) {
                // Different interface
                continue;
            }

            if(foundHost == "") {
                // Look until you find the first one (probably IPv4)
                foundHost = std::string(host);
            }

            if(preferableIp == IPV4 && family == AF_INET) {
                // Found IPv4
                foundHost = std::string(host);
            } else if(preferableIp == IPV6 && family == AF_INET6) {
                // Found IPv6
                foundHost = std::string(host);
            }
        }

        freeifaddrs(ifaddr);
        return foundHost;
    }

    void Server::readCredentialsFile(std::string credentialsFile) {
        std::ifstream f(credentialsFile);
        char c;
        bool readingUsername = true;
        bool escapeNext = false;
        std::string username = "", password = "";

        // Read and save each username:password pair
        while(f.get(c)) {
            if(c == '\r') {
                continue;
            }

            if(!escapeNext) {
                if(c == '\\') {
                    escapeNext = true;
                    continue;
                }
                if(c == ':') {
                    readingUsername = false;
                    continue;
                }
            }

            escapeNext = false;
            if(readingUsername) {
                username.push_back(c);
            } else if(c == '\n') {
                credentials[username] = password;
                readingUsername = true;
                username = "";
                password = "";
            } else {
                password.push_back(c);
            }
        }
        f.close();
    }

    void Server::setPreferableIp(ip_prefer_t ip) {
        this->preferableIp = ip;
    }

    errorCode_t Server::getErrorCode() {
        return this->errorCode;
    }

    void Server::setErrorCode(errorCode_t errorCode) {
        this->errorCode = errorCode;
    }

    UserHandler::UserHandler(int sFd, std::string wDir, const std::unordered_map<std::string, std::string> &creds) 
        : credentials(creds) {

        this->socketFd = sFd;
        char fullPath[PATH_MAX];
        if(!realpath(wDir.c_str(), fullPath)) {
            _LOG_WARN("Couldn't resolve fullpath for " << wDir);
        }
        this->baseWorkingDirectory = std::string(fullPath);
        this->workingDirectory = "";
    }

    void UserHandler::handle() {
        cmd_t cmd;

        while(state != DISCONNECT) {
            if(state == INIT) {
                // Send a welcome message
                sendMessage(SFTP_SERVICE_NAME " - SFTP Service", SUCCESS);
                state = EXPECT_USER;
            } else {
                if(cmd.command == "USER") {
                    user(cmd);
                } else if(cmd.command == "ACCT") {
                    acct(cmd);
                } else if(cmd.command == "PASS") {
                    pass(cmd);
                } else if(cmd.command == "TYPE") {
                    type(cmd);
                } else if(cmd.command == "LIST") {
                    list(cmd);
                } else if(cmd.command == "CDIR") {
                    cdir(cmd);
                } else if(cmd.command == "KILL") {
                    kill(cmd);
                } else if(cmd.command == "NAME") {
                    name(cmd);
                } else if(cmd.command == "DONE") {
                    sendMessage("", SUCCESS);
                    break;
                } else if(cmd.command == "RETR") {
                    retr(cmd);
                } else if(cmd.command == "STOR") {
                    stor(cmd);
                } else {
                    sendMessage("Unknown command: \"" + cmd.command + "\"", ERROR);
                }
            }

            // Receive message and convert it to command
            std::string msg = receiveMessage();
            if(state != DISCONNECT) {
                cmd = toCommand(msg);
            }
        }
        
        close(socketFd);
    }

    std::string UserHandler::receiveMessage() {
        int receivedMsgSize = MAX_MSG_SIZE;
        char receivedMsg[MAX_MSG_SIZE];
        std::string msg = "";

        while(receivedMsgSize == MAX_MSG_SIZE) {
            receivedMsgSize = recv(socketFd, (void*)receivedMsg, MAX_MSG_SIZE, 0);
            if(receivedMsgSize <= 0) {
                _LOG_DEBUG("User disconnected");
                state = DISCONNECT;
                return "";
            }
            msg += std::string(receivedMsg);
        }
        return msg;
    }

    void UserHandler::sendMessage(std::string message, responseCode_t rc) {
        std::string msg = "";
        if(rc == responseCode_t::SUCCESS) {
            msg += "+ ";
        } else if(rc == responseCode_t::LOGGED_IN) {
            msg += "! ";
        } else if(rc == responseCode_t::ERROR) {
            msg += "- ";
        } else {
            msg += " ";
        }
        msg += message + '\0';

        int expectSent = msg.size() + 1;
        _LOG_DEBUG("Sending (" << expectSent << "): " << msg);
        int sent = send(socketFd, msg.data(), expectSent, 0);
        
        if(sent != -1) {
            while(sent != expectSent) {
                // send() didn't send all the data, keep send()ing the next ones
                std::string next_response = msg.substr(sent);
                expectSent = next_response.length();
                sent = send(socketFd, next_response.data(), expectSent, 0);
            }
        } // else couldn't send data (error)
    }

    void UserHandler::sendFile(std::string file, uint32_t chunkSize) {
        std::ifstream f(file);
        char *buffer = new char[chunkSize];
        
        while(!f.eof()) {
            // Keep reading until eof
            f.read(buffer, chunkSize);
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
    }

    UserHandler::cmd_t UserHandler::toCommand(std::string s) {
        cmd_t cmd;

        bool first = true;
        std::string currentStr = "";

        // split string by spaces
        for(size_t i = 0; i < s.length(); i++) {
            if(s[i] == '\r' || s[i] == '\n') {
                continue; // ignore CR/LF
            }

            if(s[i] == ' ') {
                if(first) {
                    cmd.command = currentStr;
                    first = false;
                } else {
                    cmd.params.push_back(currentStr);
                }
                currentStr = "";
            } else {
                currentStr += s[i];
            }
        }

        if(first) {
            cmd.command = currentStr;
        } else {
            cmd.params.push_back(currentStr);
        }

        return cmd;
    }

    void UserHandler::user(cmd_t cmd) {
        if(state == LOGIN) {
            sendMessage("You are already logged in", ERROR);
            return;
        }

        if(cmd.params.size() != 1) {
            // Missing user
            sendMessage("Expected USER user-id", ERROR);
            return;
        }

        auto entry = credentials.find(cmd.params[0]);
        if(entry == credentials.end()) {
            // User doesn't exist
            sendMessage("User not found", ERROR);
            return;
        }

        if(entry->second == "") {
            // User doesn't require password
            state = LOGIN;
            sendMessage(entry->first + " logged in", LOGGED_IN);
            return;
        }

        username = entry->first;
        state = EXPECT_PASS;

        sendMessage("User-id valid, send password", SUCCESS);
    }

    void UserHandler::acct(cmd_t cmd) {
        if(state == LOGIN) {
            sendMessage("You are already logged in", ERROR);
            return;
        }

        if(cmd.params.size() != 1) {
            // Missing user
            sendMessage("Expected ACCT account", ERROR);
            return;
        }

        auto entry = credentials.find(cmd.params[0]);
        if(entry == credentials.end()) {
            // Account doesn't exist
            sendMessage("Account not found", ERROR);
            return;
        }

        if(entry->second == "") {
            // Account doesn't require password
            state = LOGIN;
            sendMessage(entry->first + " logged in", LOGGED_IN);
            return;
        }

        username = entry->first;
        state = EXPECT_PASS;
        sendMessage("Account valid, send password", SUCCESS);
    }

    void UserHandler::pass(cmd_t cmd) {
        if(state == LOGIN) {
            sendMessage("You are already logged in", ERROR);
            return;
        }

        if(state != EXPECT_PASS) {
            sendMessage("Select ACCT account or USER user-id to use first", ERROR);
            return;
        }

        if(cmd.params.size() != 1) {
            // Missing param
            sendMessage("Expected 1 parameter (PASS password)", ERROR);
            return;
        }

        auto entry = credentials.find(username);
        if(entry == credentials.end()) {
            sendMessage("Internal error - User not found, try again", ERROR);
            state = EXPECT_USER;
            return;
        }

        if(entry->second != cmd.params[0]) {
            // Incorrect password
            sendMessage("Wrong password, try again", ERROR);
            return;
        }

        state = LOGIN;
        sendMessage("Logged in", LOGGED_IN);
    }

    void UserHandler::type(cmd_t cmd) {
        //sendMessage("This command does literally nothing (successfully though!)", SUCCESS);

        if(cmd.params.size() != 1) {
            sendMessage("Type not valid", ERROR);
            return;
        }

        if(cmd.params[0] == "A") {
            sendMessage("Using Ascii mode", SUCCESS);
        } else if(cmd.params[0] == "B") {
            sendMessage("Using Binary mode", SUCCESS);
        } else if(cmd.params[0] == "C") {
            sendMessage("Using Continuous mode", SUCCESS);
        } else {
            sendMessage("Type not valid", ERROR);
        }
    }

    void UserHandler::list(cmd_t cmd) {
        if(state != LOGIN) {
            sendMessage("Not logged in", ERROR);
            return;
        }

        if(cmd.params.size() < 1 || (cmd.params[0] != "F" && cmd.params[0] != "V")) {
            // Missing param
            sendMessage("Expected LIST { F | V } directory-path", ERROR);
            return;
        }

        std::string tempDir = "";
        if(cmd.params.size() > 1) {
            tempDir = cmd.params[1];
        }

        if(workingDirectory != "") {
            tempDir += workingDirectory;
        }

        auto pair = getDirectoryListing(baseWorkingDirectory, tempDir);
        if(pair.first != "") {
            sendMessage("Couldn't list content because: " + pair.first, ERROR);
            return;
        }

        std::string msg = tempDir + "\n";
        for(auto &d : pair.second) {
            if(cmd.params[0] == "F") {
                msg += d.toString() + "\n";
            } else {
                msg += d.toStringVerbose() + "\n";
            }
        }
        msg[msg.length()-1] = '\0'; // don't insert the last newline

        sendMessage(msg, SUCCESS);
    }

    void UserHandler::cdir(cmd_t cmd) {
        if(state != LOGIN) {
            sendMessage("Can't connect to directory because: Not logged in", ERROR);
            return;
        }

        if(cmd.params.size() != 1) {
            // Missing param
            sendMessage("Can't connect to directory because: Expected CDIR new-directory", ERROR);
            return;
        }

        // Current working directory + input param
        std::string path = "";
        if(workingDirectory != "") {
            path = workingDirectory + "/";
        }
        path += cmd.params[0];

        auto p = getDirectoryRealpath(baseWorkingDirectory, path);
        if(p.first != "") {
            sendMessage("Can't connect to directory because: " + p.first, ERROR);
            return;
        }
        workingDirectory = p.second.substr(baseWorkingDirectory.length());
        sendMessage("Changed working dir to " + workingDirectory, LOGGED_IN);
    }

    void UserHandler::kill(cmd_t cmd) {
        if(state != LOGIN) {
            sendMessage("Not deleted because: Not logged in", ERROR);
            return;
        }

        if(cmd.params.size() != 1) {
            // Missing param
            sendMessage("Not deleted because: Expected 1 parameter (KILL file-spec)", ERROR);
            return;
        }

        // Current working directory + input param
        std::string path = "";
        if(workingDirectory != "") {
            path = workingDirectory + "/";
        }
        path += cmd.params[0];
        auto p = getDirectoryRealpath(baseWorkingDirectory, path);
        if(p.first != "") {
            sendMessage("Not deleted because: " + p.first, ERROR);
            return;
        }

        // Figure out if its a file or a folder
        struct stat s;
        if(stat(p.second.c_str(), &s) != 0) {
            sendMessage("Not deleted because: Couldn't retrieve information about this file", ERROR);
            return;
        }

        if((s.st_mode & S_IFMT) == S_IFDIR) { // Directory
            if(rmdir(p.second.c_str()) != 0) {
                sendMessage("Not deleted because: " + utils::errnoToStr(), ERROR);
                return;
            }
        } else { // assume its some file and try unlink()
            if(unlink(p.second.c_str()) != 0) {
                sendMessage("Not deleted because: " + utils::errnoToStr(), ERROR);
                return;
            }
        }
        sendMessage(path + " deleted", SUCCESS);
    }

    void UserHandler::name(cmd_t cmd) {
        if(state != LOGIN) {
            sendMessage("Not logged in", ERROR);
            return;
        }

        if(cmd.params.size() != 1) {
            // Missing param
            sendMessage("Expected NAME old-file-spec", ERROR);
            return;
        }

        // Current working directory + input param
        std::string path = "";
        if(workingDirectory != "") {
            path = workingDirectory + "/";
        }
        path += cmd.params[0];
        auto p = getDirectoryRealpath(baseWorkingDirectory, path);
        if(p.first != "") {
            sendMessage(p.first, ERROR);
            return;
        }

        sendMessage("File exists", SUCCESS);

        // Wait for TOBE
        std::string msg = receiveMessage();
        if(state == DISCONNECT) {
            return;
        }

        cmd_t cmd2 = toCommand(msg);
        if(cmd2.command != "TOBE" || cmd2.params.size() != 1) {
            sendMessage("File wasn't renamed because: Expected TOBE <new-file-spec>", ERROR);
            return;
        }

        std::string path2 = baseWorkingDirectory + "/";
        if(workingDirectory != "") {
            path2 += workingDirectory + "/";
        }
        path2 += cmd2.params[0];

        // Try to resolve the full path and figure out, if someone is trying to escape the working directory
        // - similar to getDirectoryRealpath() except that we have to create the file, if it doesn't exist
        char fullPath[PATH_MAX];
        // Get the real path to the file
        if(!realpath(path2.c_str(), fullPath)) {
            // File doesn't exist - nothing (potentionally) important will be overwritten, we can just create temporary one
            std::ofstream outFile(path2.c_str());
            outFile.close();
            if(!realpath(path2.c_str(), fullPath)) {
                // shouldn't happen
                sendMessage("File wasn't renamed because: Unexpected error while renaming file", ERROR);
                _LOG_DEBUG("Couldn't resolve path " << path2.c_str());
                return;
            }
            // we don't need it anymore
            unlink(path2.c_str());
        }

        // check if we are in the base working directory
        std::string fullPathStr(fullPath);
        size_t pos = fullPathStr.find(baseWorkingDirectory);
        if(pos != 0) {
            sendMessage("File wasn't renamed because: An attempt to leave the working directory was made", ERROR);
            return;
        }

        if(rename(p.second.c_str(), fullPathStr.c_str()) != 0) { // should be safe to rename at this point
            sendMessage("File wasn't renamed because: " + utils::errnoToStr(), ERROR);
            return;
        }

        sendMessage(cmd.params[0] + " renamed to " + cmd2.params[0], SUCCESS);
    }

    void UserHandler::retr(cmd_t cmd) {
        if(state != LOGIN) {
            sendMessage("Not logged in", ERROR);
            return;
        }

        if(cmd.params.size() != 1) {
            // Missing param
            sendMessage("Expected RETR file-spec", ERROR);
            return;
        }

        // Current working directory + input param
        std::string path = "";
        if(workingDirectory != "") {
            path = workingDirectory + "/";
        }
        path += cmd.params[0];
        auto p = getDirectoryRealpath(baseWorkingDirectory, path);
        if(p.first != "") {
            sendMessage(p.first, ERROR);
            return;
        }

        struct stat s;
        if(stat(p.second.c_str(), &s) != 0) {
            sendMessage("Couldn't retrieve file information", ERROR);
            return;
        }

        // <number-of-bytes-that-will-be-sent>
        sendMessage(std::to_string(s.st_size), NUMBER);

        // Wait for SEND / STOP
        std::string msg = receiveMessage();
        if(state == DISCONNECT) {
            return;
        }
        cmd_t cmd2 = toCommand(msg);

        if(cmd2.command == "STOP") {
            sendMessage("ok, RETR aborted", SUCCESS);
            return;
        }

        if(cmd2.command != "SEND") {
            sendMessage("Expected SEND or STOP. RETR aborted", ERROR);
            return;
        }

        sendFile(p.second, CHUNK_SIZE);
    }

    void UserHandler::stor(cmd_t cmd) {
         if(state != LOGIN) {
            sendMessage("Not logged in", ERROR);
            return;
        }

        if(cmd.params.size() != 2 || (cmd.params[0] != "NEW" && cmd.params[0] != "OLD" && cmd.params[0] != "APP")) {
            // Missing param
            sendMessage("Expected STOR { NEW | OLD | APP } file-spec", ERROR);
            return;
        }

        // Current working directory + input param
        std::string path = baseWorkingDirectory + "/";
        if(workingDirectory != "") {
            path += workingDirectory + "/";
        }
        path += cmd.params[1];

        // Make sure we aren't leaving the working directory
        char fullPath[PATH_MAX];
        bool fileExists = true;
        if(!realpath(path.c_str(), fullPath)) {
            fileExists = false;
        }
        std::string fullPathStr = std::string(fullPath);
        size_t pos = fullPathStr.find(baseWorkingDirectory);
        if(pos != 0) {
            _LOG_DEBUG("Possible attempt to leave the working directory (" << fullPathStr << ")");
            sendMessage("Permission denied", ERROR);
            return;
        }
        
        std::ofstream f;
        if(cmd.params[0] == "NEW") {
            if(fileExists) {
                sendMessage("File exists, but system doesn't support generations", ERROR);
                return;
            } else {
                sendMessage("File does not exist, will create new file", SUCCESS);
            }
            f.open(fullPathStr);
        } else if(cmd.params[0] == "OLD") {
            if(fileExists) {
                sendMessage("Will write over old file", SUCCESS);
                unlink(fullPathStr.c_str());
            } else {
                sendMessage("Will create new file", SUCCESS);
            }
            f.open(fullPathStr);
        } else if(cmd.params[0] == "APP") {
            if(fileExists) {
                sendMessage("Will append to file", SUCCESS);
                f.open(fullPathStr, std::ios_base::app);
            } else {
                sendMessage("Will create file", SUCCESS);
                f.open(fullPathStr);
            }
        }
        
        // Wait for SIZE <number-of-bytes-in-file>
        std::string msg = receiveMessage();
        if(state == DISCONNECT) {
            f.close();
            return;
        }
        cmd_t cmd2 = toCommand(msg);

        if(cmd2.command != "SIZE" || cmd2.params.size() != 1) {
            sendMessage("Expected SIZE <number-of-bytes-in-file>. Aborting", ERROR);
            _LOG_DEBUG("Received " << msg);
            f.close();
            return;
        }
        auto sizeP = utils::toNumber(cmd2.params[0]);
        if(sizeP.first != 0) {
            sendMessage("Expected SIZE <number-of-bytes-in-file>. Aborting", ERROR);
            _LOG_DEBUG("Received " << msg);
            f.close();
            return;
        }

        sendMessage("ok, waiting for file", SUCCESS);

        // Receive the file
        int expectedSize = sizeP.second;
        int receivedSize = 0;
        char buffer[CHUNK_SIZE];
        while(receivedSize < expectedSize) {
            int rec = recv(socketFd, (void*)buffer, CHUNK_SIZE, 0);
            if(rec <= 0) {
                _LOG_DEBUG("User disconnected");
                return;
            }
            receivedSize += rec;
            f.write(buffer, rec);
        }
        f.close();
        sendMessage("Saved " + cmd.params[1], SUCCESS);
    }

    std::pair<std::string, std::vector<dirEntry_t>> getDirectoryListing(std::string workDirFullPath, std::string directory) {
        using strVecPair = std::pair<std::string, std::vector<dirEntry_t>>;

        struct dirent *dir;
        DIR *dirp;
        std::vector<dirEntry_t> vec;

        auto realDirPath = getDirectoryRealpath(workDirFullPath, directory);
        if(realDirPath.first != "") {
            return strVecPair(realDirPath.first, vec);
        }

        dirp = opendir(realDirPath.second.c_str());
        if(!dirp) {
            _LOG_ERR("Couldn't open directory " << realDirPath.second);
            return strVecPair("Couldn't open directory", vec);
        }

        // Ok, directory opened
        while((dir = readdir(dirp))) {
            if(!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")) {
                continue;
            }
            dirEntry_t entry;
            std::string filePath = realDirPath.second + "/" + dir->d_name;

            // get file/dir details
            entry.name = std::string(dir->d_name);
            if(stat(filePath.c_str(), &entry.s) != 0) {
                _LOG_WARN("Failed retrieving information about file \"" << filePath << "\"!");
                continue;
            }
            entry.updateVerbose();

            vec.push_back(entry);
        }
        closedir(dirp);

        return strVecPair("", vec);
    }

    std::pair<std::string, std::string> getDirectoryRealpath(std::string workDirFullPath, std::string directory) {
        using StrPair = std::pair<std::string, std::string>;

        if(directory == "." || directory == "") {
            return StrPair("", workDirFullPath);
        }

        std::string realDirPath = workDirFullPath + "/" + directory;

        // Make sure we aren't leaving the working directory
        char fullPath[PATH_MAX];

        if(!realpath(realDirPath.c_str(), fullPath)) {
            _LOG_DEBUG("Couldn't get realpath (" << realDirPath << ")");
            return StrPair("Directory/File not found", "");
        }

        std::string fullPathStr = std::string(fullPath);
        size_t pos = fullPathStr.find(workDirFullPath);
        if(pos != 0) {
            _LOG_DEBUG("Possible attempt to leave the working directory (" << fullPathStr << ")");
            return StrPair("Directory/File not found", "");
        }
        return StrPair("", fullPathStr);
    }
}
